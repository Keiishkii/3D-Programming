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

#include "Shaders.h"

#include "VertexBuffer.h"
#include "VertexArray.h"
#include "AnimatedModel.h"

#include "InputHandler.h"

#define WINDOW_WIDTH 800	
#define WINDOW_HEIGHT 600

#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))

std::shared_ptr<AnimatedModel> animatedModel;
//std::shared_ptr<VertexArray> vao;

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
void gameLoop(SDL_Window* _window, const GLuint& _programID);
void FBX_DestroyManager(FbxManager*& _manager);

void translateViewingMatrix(glm::mat4& veiwing, Transform& _transform);

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

void startup(SDL_Window*& _window, FbxManager*& _manager, FbxScene*& _scene)
{
	std::string path = "models/xenoblade/shulk.fbx";

	bool modelLoaded = true;
	animatedModel = std::make_shared<AnimatedModel>(path, _manager, _scene, modelLoaded);
	
	if (modelLoaded)
	{
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

		gameLoop(_window, programId);
	}
	else
	{
		std::cout << "failed to find the model :(" << std::endl;
	}
}

void gameLoop(SDL_Window* window, const GLuint& programId)
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
		glBindVertexArray(animatedModel->getVertexArrayObject()->getID());
		//glBindTexture(GL_TEXTURE_2D, textureId);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(invVeiwingLoc, 1, GL_FALSE, glm::value_ptr(veiwing));

		// Draw to the screen
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDrawArrays(GL_TRIANGLES, 0, animatedModel->getVertexArrayObject()->getVertexCount());

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