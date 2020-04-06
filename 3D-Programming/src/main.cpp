#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fbxsdk.h>

#include <exception>
#include <iostream>
#include <vector>
#include <memory>

#include "Model.h"
#include "Shaders.h"

#include "VertexBuffer.h"

#include "InputHandler.h"

#define WINDOW_WIDTH 800	
#define WINDOW_HEIGHT 600

#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))

struct Transform
{
	glm::mat4 currentTransform = glm::mat4(1);

	glm::vec3 currentOrientation{ 0 };
	glm::vec3 currentPosition{ 0 };

	glm::vec3 delta_XYZ{ 0 };
	glm::vec3 delta_R_XYZ{ 0 };
	float translateMultiplier = 1;
};

bool initialiseSDKs(SDL_Window*& _window, FbxManager*& _manager, FbxScene*& _scene);

void startup(SDL_Window*& _window, FbxManager*& _manager, FbxScene*& _scene);
void unpackModel(std::vector<GLfloat>& _positionsV, std::vector<GLfloat>& _textureCordV, Model& _object);
void uploadToGraphicsCard(std::vector<glm::vec3>& _positionsV, std::vector<GLfloat>& _textureCordV, GLuint& _vaoId);
void gameLoop(SDL_Window* _window, const GLuint& _programId, const GLuint& _vaoId, const GLuint& textureId, int const _polygonCount);
void FBX_DestroyManager(FbxManager*& _manager);

void translateViewingMatrix(glm::mat4& veiwing, Transform& _transform);

void operateOnNode(FbxNode* _node, std::vector<glm::vec3>& _positionsV, std::vector<GLfloat>& _textureCoordV, int& _vertexCount);

////

bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);

//
std::shared_ptr<VertexBuffer> vb;

int main(int argc, char *argv[])
{
	SDL_Window* window = nullptr;
	FbxManager* fbxManager = nullptr;
	FbxScene* fbxScene = nullptr;
	
	if (initialiseSDKs(window, fbxManager, fbxScene))
	{
		startup(window, fbxManager, fbxScene);
	}
	else
	{
		std::cout << "program failed" << std::endl;
	}

	FBX_DestroyManager(fbxManager);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

bool initialiseSDKs(SDL_Window*& _window, FbxManager*& _manager, FbxScene*& _scene)
{
	bool programSuccess = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw std::exception();
		programSuccess = false;
	}
	else
	{
		_window = SDL_CreateWindow("Triangle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

		if (!SDL_GL_CreateContext(_window))
		{
			throw std::exception();
			programSuccess = false;
		}
		else
		{
			if (SDL_SetRelativeMouseMode(SDL_TRUE))
			{
				throw std::exception();
				programSuccess = false;
			}
			else
			{
				if (glewInit() != GLEW_OK)
				{
					throw std::exception();
					programSuccess = false;
				}
				else
				{
					_manager = FbxManager::Create();

					if (!_manager)
					{
						FBXSDK_printf("Error: Unable to create FBX Manager!\n");
						programSuccess = false;
					}
					else
					{
						FbxIOSettings* ios = FbxIOSettings::Create(_manager, IOSROOT);
						_manager->SetIOSettings(ios);

						FbxString lPath = FbxGetApplicationDirectory();
						_manager->LoadPluginsDirectory(lPath.Buffer());

						_scene = FbxScene::Create(_manager, "My Scene");
						if (!_scene)
						{
							FBXSDK_printf("Error: Unable to create FBX scene!\n");
							programSuccess = false;
						}
					}
				}
			}
		}
	}

	return programSuccess;
}

void operateOnNode(FbxNode* _node, std::vector < glm::vec3 > & _positionsV, std::vector<GLfloat>& _textureCoordV, int& _vertexCount)
{
	std::cout << "Node name: " << (*_node).GetName() << std::endl;

	if (!((*_node).GetNodeAttribute() == NULL))
	{
		int attributeCount = (*_node).GetNodeAttributeCount();
		for (int i = 0; i < attributeCount; i++)
		{
			FbxNodeAttribute::EType lAttributeType;
			lAttributeType = ((*_node).GetNodeAttributeByIndex(i)->GetAttributeType());

			switch (lAttributeType)
			{
				case FbxNodeAttribute::eMesh:
				{
					FbxMesh* mesh = (*_node).GetMesh();
					
					int currentPolygonCount = mesh->GetPolygonCount();
					std::cout << "Polygon count of mesh: " << currentPolygonCount << std::endl;

					int g = 0;
					for (int p = 0; p < currentPolygonCount; p++)
					{
						int* vertciesArray = mesh->GetPolygonVertices();
					//	mesh->texture
						for (int v = 0; v < mesh->GetPolygonSize(p); v++)
						{
							_vertexCount++;
							FbxVector4 controlPoint = mesh->GetControlPointAt(vertciesArray[v + (p * mesh->GetPolygonSize(p))]);
							_positionsV.push_back(glm::vec3(controlPoint.mData[0], controlPoint.mData[1], controlPoint.mData[2]));
						}
					}

					break;
				}
			}
		}
	}

	int children = (*_node).GetChildCount();

	if (children > 0)
	{
		for (int i = 0; i < children; i++)
		{
			operateOnNode(((*_node).GetChild(i)), _positionsV, _textureCoordV, _vertexCount);
		}
	}
}

void startup(SDL_Window*& _window, FbxManager*& _manager, FbxScene*& _scene)
{
	bool sceneSuccsess = true;
	
	std::string path = "models/xenoblade/shulk.fbx";

	FbxString localFilePath(path.c_str());

	sceneSuccsess = LoadScene(_manager, _scene, localFilePath.Buffer());
	
	int polygonCount = 0;
	int vertexCount = 0;

	std::vector<glm::vec3> positionsVector;
	std::vector<GLfloat> textureCoordsVector;

	if (sceneSuccsess)
	{
		FbxNode* routeNode = _scene->GetRootNode();

		if (routeNode)
		{
			operateOnNode(routeNode, positionsVector, textureCoordsVector, vertexCount);
			std::cout << "Polygon count: " << polygonCount << std::endl;
			std::cout << "Vertex varaible value count: " << positionsVector.size() << std::endl;
		}

		//int w = 0;
		//int h = 0;
		//unsigned char* texture = stbi_load("models/Monado Texture.png", &w, &h, NULL, 4);
		//Model objectToRender("models/Monado.obj", glm::vec4(0, -2.5, -20, 1));

		//if (!texture)
		//{
		//	throw std::exception();
		//}

		////

		// Create and bind a texture.
		GLuint textureID = 0;
		//glGenTextures(1, &textureID);

		//if (!textureID)
		//{
		//	throw std::exception();
		//}

		//glBindTexture(GL_TEXTURE_2D, textureID);

		// Upload the image data to the bound texture unit in the GPU
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
		//	GL_UNSIGNED_BYTE, texture);

		// Free the loaded data because we now have a copy on the GPU
		//free(texture);

		// Generate Mipmap so the texture can be mapped correctly
		//glGenerateMipmap(GL_TEXTURE_2D);

		// Unbind the texture because we are done operating on it
		//glBindTexture(GL_TEXTURE_2D, 0);
		
					//unpackModel(positionsVector, textureCoordsVector, objectToRender);

		// Generate the VAO and upload it to the graphics card
		GLuint vaoId = 0;
		uploadToGraphicsCard(positionsVector, textureCoordsVector, vaoId);

		// Compile the vertex shader
		GLuint vertexShaderID = 0;
		Shaders::Shaders::generateVertexShader(vertexShaderID);

		// Compile the fragment shader
		GLuint fragmentShaderID = 0;
		Shaders::Shaders::generateFragmentShader(fragmentShaderID);

		// Create new shader program and attach our shader objects
		GLuint programId = glCreateProgram();

		glAttachShader(programId, vertexShaderID);
		glAttachShader(programId, fragmentShaderID);


		glBindAttribLocation(programId, 0, "in_Position");
		glBindAttribLocation(programId, 1, "in_TextureCoord");


		// Perform the link and check for failure
		GLint success = 0;
		glLinkProgram(programId);
		glGetProgramiv(programId, GL_LINK_STATUS, &success);

		if (!success)
		{
			throw std::exception();
		}

		// Detach and destroy the shader objects. These are no longer needed
		// because we now have a complete shader program.
		glDetachShader(programId, vertexShaderID);
		glDeleteShader(vertexShaderID);
		glDetachShader(programId, fragmentShaderID);
		glDeleteShader(fragmentShaderID);

		/////// ------------------------------------------------------------------

		gameLoop(_window, programId, vaoId, textureID, vertexCount);
	}
	else
	{
		std::cout << "failed to find the model :(" << std::endl;
	}
}

void unpackModel(std::vector<GLfloat>& _positionsV, std::vector<GLfloat>& _textureCordV, Model& _object)
{
	for (int i = 0; i < (*_object.getTrigList()).size(); i++)
	{
		_positionsV.push_back((*_object.getTrigList()).at(i).getVertex1().x);
		_positionsV.push_back((*_object.getTrigList()).at(i).getVertex1().y);
		_positionsV.push_back((*_object.getTrigList()).at(i).getVertex1().z);

		_positionsV.push_back((*_object.getTrigList()).at(i).getVertex2().x);
		_positionsV.push_back((*_object.getTrigList()).at(i).getVertex2().y);
		_positionsV.push_back((*_object.getTrigList()).at(i).getVertex2().z);

		_positionsV.push_back((*_object.getTrigList()).at(i).getVertex3().x);
		_positionsV.push_back((*_object.getTrigList()).at(i).getVertex3().y);
		_positionsV.push_back((*_object.getTrigList()).at(i).getVertex3().z);

		_textureCordV.push_back((*_object.getTrigList()).at(i).getTextCoord1().x);
		_textureCordV.push_back((*_object.getTrigList()).at(i).getTextCoord1().y);

		_textureCordV.push_back((*_object.getTrigList()).at(i).getTextCoord2().x);
		_textureCordV.push_back((*_object.getTrigList()).at(i).getTextCoord2().y);

		_textureCordV.push_back((*_object.getTrigList()).at(i).getTextCoord3().x);
		_textureCordV.push_back((*_object.getTrigList()).at(i).getTextCoord3().y);

	}

	std::cout << "Colour Size -> " << _textureCordV.size() << std::endl;
	std::cout << "Position Size -> " << _positionsV.size() << std::endl;

}

void uploadToGraphicsCard(std::vector<glm::vec3>& _positionsV, std::vector<GLfloat>& _textureCordV, GLuint& _vaoId)
{
	/////// ------------------------------------------------------------------			-- Apply Positions and Colours to VBO
	GLuint positionsVboId = 0;
	//GLuint textureVboId = 0;

	vb = std::make_shared<VertexBuffer>();

	for (int i = 0; i < _positionsV.size(); i++)
	{
		vb->add(_positionsV.at(i));
	}

	//glGenBuffers(1, &textureVboId);

	//if (!textureVboId)
	//{
	//	throw std::exception();
	//}

	//glBindBuffer(GL_ARRAY_BUFFER, textureVboId);
	//glBufferData(GL_ARRAY_BUFFER, _textureCordV.size() * sizeof(_textureCordV.at(0)), &_textureCordV.at(0), GL_STATIC_DRAW);

	// Reset Bind
	/////// ------------------------------------------------------------------			 -- Apply VBO to VAO

	glGenVertexArrays(1, &_vaoId);

	if (!_vaoId)
	{
		throw std::exception();
	}

	glBindVertexArray(_vaoId);

	glBindBuffer(GL_ARRAY_BUFFER, vb->getID());
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, textureVboId);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	//glEnableVertexAttribArray(1);

	// Reset the state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void gameLoop(SDL_Window* window, const GLuint& programId, const GLuint& vaoId, const GLuint& textureId, int const _vertexCount)
{
	GLint modelLoc = glGetUniformLocation(programId, "in_Model");
	GLint projectionLoc = glGetUniformLocation(programId, "in_Projection");
	GLint invVeiwingLoc = glGetUniformLocation(programId, "in_InverseVeiwing");

	//GLint textureLoc = glGetUniformLocation(programId, "in_Texture");
	glm::mat4 model(1.0f);
	glm::mat4 veiwing = glm::mat4(1);
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.f);

	Transform transform;

	model = glm::translate(model, glm::vec3(0, -5, -20));

	Inputs* input = InputHandler::getInputs();
	InputHandler::processInputs();

	while (!(*input).quit)
	{
		InputHandler::processInputs();
		translateViewingMatrix(veiwing, transform);

		int width = 0, height = 0;
		SDL_GetWindowSize(window, &width, &height);

		glViewport(0, 0, width, height);
		projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.f);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		// Drawing operation
		// Instruct OpenGL to use our shader program and our VAO
		glUseProgram(programId);
		glBindVertexArray(vaoId);
		//glBindTexture(GL_TEXTURE_2D, textureId);


		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(invVeiwingLoc, 1, GL_FALSE, glm::value_ptr(veiwing));


		// Draw 3 vertices (a triangle)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_TRIANGLES, 0, _vertexCount);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		// Reset the state
		//glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glUseProgram(0);

		/////// ------------------------------------------------------------------

		SDL_GL_SwapWindow(window);
	}
}

void translateViewingMatrix(glm::mat4& veiwing, Transform& _transform)
{
	Inputs* input = InputHandler::getInputs();
	
	_transform.delta_XYZ = { 0,0,0 };
	_transform.delta_R_XYZ = { 0,0,0 };

	float movementSpeed = 0.6;
	float rotationSpeed = 0.6;

	if ((*input).mouseWheelY != 0)
	{
		_transform.translateMultiplier += ((*input).mouseWheelY * 0.01);
	}

		// Up and Down
	if ((*input).key_A)
		_transform.delta_XYZ.x += 1;
	if ((*input).key_D)
		_transform.delta_XYZ.x -= 1;

		// Left and Right
	if ((*input).key_LSHIFT)
		_transform.delta_XYZ.y += 1;
	if ((*input).key_SPACE)
		_transform.delta_XYZ.y -= 1;

		// Forwards and Back
	if ((*input).key_W)
		_transform.delta_XYZ.z += 1;
	if ((*input).key_S)
		_transform.delta_XYZ.z -= 1;

	// Roll
	_transform.delta_R_XYZ.x += -(0.1 * pow(abs((*input).yrel), 1.5) * copysign(1.0, (*input).yrel));// *((float)(*input).yrel / abs((float)(*input).yrel)));
	// Pitch
	_transform.delta_R_XYZ.y += -(0.1 * pow(abs((*input).xrel), 1.5) * copysign(1.0, (*input).xrel));// *((float)(*input).xrel / abs((float)(*input).xrel)));

	glm::mat4 deltaOffset = glm::mat4(1);
	deltaOffset = glm::translate(deltaOffset, glm::vec3(_transform.delta_XYZ.x, _transform.delta_XYZ.y, _transform.delta_XYZ.z));

	glm::mat4 temp(1.0f);
	temp = glm::rotate(temp, _transform.currentOrientation.y, glm::vec3(0, 1, 0));
	temp = glm::rotate(temp, _transform.currentOrientation.x, glm::vec3(1, 0, 0));
	temp = glm::translate(temp, glm::vec3(_transform.delta_XYZ.x, _transform.delta_XYZ.y, _transform.delta_XYZ.z));

	_transform.currentPosition += glm::vec3(temp * glm::vec4(0,0,0,1)) *- 0.01f;
	_transform.currentOrientation.x += _transform.delta_R_XYZ.x * 0.01f;
	_transform.currentOrientation.y += _transform.delta_R_XYZ.y * 0.01f;

	veiwing = glm::mat4(1);
	veiwing = glm::translate(veiwing, _transform.currentPosition);
	veiwing = glm::rotate(veiwing, _transform.currentOrientation.y, glm::vec3(0, 1, 0));
	veiwing = glm::rotate(veiwing, _transform.currentOrientation.x, glm::vec3(1, 0, 0));
	
	veiwing = glm::inverse(veiwing);
}

void FBX_DestroyManager(FbxManager*& _manager)
{
	if (_manager)
	{
		_manager->Destroy();
	}
}

bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus)
	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

		if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		FBXSDK_printf("\n");

		for (i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			FBXSDK_printf("    Animation Stack %d\n", i);
			FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			FBXSDK_printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);

	if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
	{
		FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}