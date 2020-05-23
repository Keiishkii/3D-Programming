#include "AnimatedModel.h"

#include <iostream>

#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))

AnimatedModel::AnimatedModel(std::string _modelPath, FbxManager * _manager, FbxScene * _scene, bool &_success, glm::vec3 _transform) : transform(_transform)
{
	bool sceneSuccsess = true;

	FbxString localFilePath(_modelPath.c_str());

	sceneSuccsess = LoadScene(_manager, _scene, localFilePath.Buffer());

	if (sceneSuccsess)
	{
		FbxNode* routeNode = _scene->GetRootNode();

		int vertexCount = 0;

		std::vector<GLfloat> textureCoordsVector;
		
		std::shared_ptr<VertexBuffer> vertexPositions = std::make_shared<VertexBuffer>();
		std::shared_ptr<VertexBuffer> textureUVs = std::make_shared<VertexBuffer>();
		std::shared_ptr<VertexBuffer> vertexNormals = std::make_shared<VertexBuffer>();
		std::shared_ptr<VertexBuffer> vertexTangents = std::make_shared<VertexBuffer>();
		std::shared_ptr<VertexBuffer> vertexBitangents = std::make_shared<VertexBuffer>();

		if (routeNode)
		{
			operateOnNode(routeNode, vertexPositions, textureUVs, vertexNormals, vertexCount);

			for (int i = 0; i < vertexCount; i += 3)
			{
				glm::vec3 v0 ((*vertexPositions).getData().at(i),(*vertexPositions).getData().at((int)i + 1),(*vertexPositions).getData().at((int)i + 2) );
				glm::vec3 v1 ((*vertexPositions).getData().at((int)i + 3), (*vertexPositions).getData().at((int)i + 4), (*vertexPositions).getData().at((int)i + 5));
				glm::vec3 v2 ((*vertexPositions).getData().at((int)i + 6), (*vertexPositions).getData().at((int)i + 7), (*vertexPositions).getData().at((int)i + 8));

				glm::vec2 uv0 ((*textureUVs).getData().at(i), (*textureUVs).getData().at((int)i + 1));
				glm::vec2 uv1 ((*textureUVs).getData().at((int)i + 2), (*textureUVs).getData().at((int)i + 3));
				glm::vec2 uv2 ((*textureUVs).getData().at((int)i + 4), (*textureUVs).getData().at((int)i + 5));

				glm::vec3 deltaPos1 = v1 - v0;
				glm::vec3 deltaPos2 = v2 - v0;

				glm::vec2 deltaUV1 = uv1 - uv0;
				glm::vec2 deltaUV2 = uv2 - uv0;

				float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

				glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
				glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

				(*vertexTangents).add(tangent);
				(*vertexTangents).add(tangent);
				(*vertexTangents).add(tangent);

				(*vertexBitangents).add(bitangent);
				(*vertexBitangents).add(bitangent);
				(*vertexBitangents).add(bitangent);
			}
		}

		VAO.setVertexCount(vertexCount);

		VAO.setBuffer("VertexPositions", vertexPositions);
		VAO.setBuffer("TextureUVs", textureUVs);
		VAO.setBuffer("VertexNormals", vertexNormals);
		VAO.setBuffer("VertexTangents", vertexTangents);
		VAO.setBuffer("VertexBitangents", vertexBitangents);
	}
	else
	{
		_success = false;
	}
}

void AnimatedModel::draw(GLuint _programID, const glm::mat4& _projection, const glm::mat4& _veiwing)
{
	GLint modelID = glGetUniformLocation(_programID, "in_Model");
	GLint projectionID = glGetUniformLocation(_programID, "in_Projection");
	GLint invVeiwingID = glGetUniformLocation(_programID, "in_InverseVeiwing");

	glm::mat4 model(1.0f);
	model = glm::translate(model, transform);

	glUseProgram(_programID);
	glBindVertexArray(getVertexArrayObject()->getID());
	//glBindTexture(GL_TEXTURE_2D, textureId);

	glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(projectionID, 1, GL_FALSE, glm::value_ptr(_projection));
	glUniformMatrix4fv(invVeiwingID, 1, GL_FALSE, glm::value_ptr(_veiwing));

	// Draw to the screen
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawArrays(GL_TRIANGLES, 0, getVertexArrayObject()->getVertexCount());
}

void AnimatedModel::operateOnNode(FbxNode* _node, std::shared_ptr<VertexBuffer> _vertexPositions, std::shared_ptr<VertexBuffer> _textureUVs, std::shared_ptr<VertexBuffer> _normals, int& _vertexCount)
{
	if (!((*_node).GetNodeAttribute() == NULL))
	{
		int attributeCount = (*_node).GetNodeAttributeCount();
		for (int i = 0; i < attributeCount; i++)
		{
			FbxNodeAttribute::EType lAttributeType;
			lAttributeType = ((*_node).GetNodeAttributeByIndex(i)->GetAttributeType());

			switch (lAttributeType)
			{
				case FbxNodeAttribute::eSkeleton:
				{
					FbxSkeleton* skeleton = (FbxSkeleton*) _node->GetNodeAttribute();

					if (skeleton != NULL)
					{
						//if (skeleton->GetSkeletonType() == FbxSkeleton::eRoot)
						{
							//std::cout << "found " << skeleton->GetSkeletonType() << std::endl;
						}
					}

					break;
				}
				case FbxNodeAttribute::eMesh:
				{
					FbxMesh* mesh = (*_node).GetMesh();

					LoadUVInformation(mesh, _textureUVs);

					LoadMaterials(mesh);

					if (mesh != NULL)
					{
						int currentPolygonCount = mesh->GetPolygonCount();

						int g = 0;
						for (int p = 0; p < currentPolygonCount; p++)
						{
							int* vertciesArray = mesh->GetPolygonVertices();

							for (int v = 0; v < mesh->GetPolygonSize(p); v++)
							{
								_vertexCount++;

								FbxVector4 controlPoint = mesh->GetControlPointAt(vertciesArray[v + (p * mesh->GetPolygonSize(p))]);
								FbxVector4 normal;

								mesh->GetPolygonVertexNormal(p, v, normal);

								glm::vec3 vertexPosition(controlPoint.mData[0], controlPoint.mData[1], controlPoint.mData[2]);
								glm::vec3 vertexNormal(normal.mData[0], normal.mData[1], normal.mData[2]);

								(*_vertexPositions).add(vertexPosition);
								(*_normals).add(vertexNormal);

							}
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
			operateOnNode(((*_node).GetChild(i)), _vertexPositions, _textureUVs, _normals, _vertexCount);
		}
	}
}

void AnimatedModel::LoadMaterials(FbxMesh* pMesh)
{
	FbxNode* node = pMesh->GetNode();

	int matCount = node->GetMaterialCount();
	
	for (int i = 0; i < matCount; i++)
	{
		FbxSurfaceMaterial* material = node->GetMaterial(i);
	}
}

void AnimatedModel::LoadUVInformation(FbxMesh* pMesh, std::shared_ptr<VertexBuffer> _textureUVs)
{
	//get all UV set names
	FbxStringList lUVSetNameList;
	pMesh->GetUVSetNames(lUVSetNameList);

	//iterating over all uv sets
	for (int lUVSetIndex = 0; lUVSetIndex < lUVSetNameList.GetCount(); lUVSetIndex++)
	{
		//get lUVSetIndex-th uv set
		const char* lUVSetName = lUVSetNameList.GetStringAt(lUVSetIndex);
		const FbxGeometryElementUV* lUVElement = pMesh->GetElementUV(lUVSetName);

		if (!lUVElement)
			continue;

		// only support mapping mode eByPolygonVertex and eByControlPoint
		if (lUVElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
			lUVElement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
			return;

		//index array, where holds the index referenced to the uv data
		const bool lUseIndex = lUVElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int lIndexCount = (lUseIndex) ? lUVElement->GetIndexArray().GetCount() : 0;

		//iterating through the data by polygon
		const int lPolyCount = pMesh->GetPolygonCount();

		if (lUVElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					FbxVector2 lUVValue;

					//get the index of the current vertex in control points array
					int lPolyVertIndex = pMesh->GetPolygonVertex(lPolyIndex, lVertIndex);

					//the UV index depends on the reference mode
					int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyVertIndex) : lPolyVertIndex;
					
					lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

					//User TODO:
					//Print out the value of UV(lUVValue) or log it to a file
				}
			}
		}
		else if (lUVElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			int lPolyIndexCounter = 0;
			for (int lPolyIndex = 0; lPolyIndex < lPolyCount; ++lPolyIndex)
			{
				// build the max index array that we need to pass into MakePoly
				const int lPolySize = pMesh->GetPolygonSize(lPolyIndex);
				for (int lVertIndex = 0; lVertIndex < lPolySize; ++lVertIndex)
				{
					if (lPolyIndexCounter < lIndexCount)
					{
						FbxVector2 lUVValue;

						//the UV index depends on the reference mode
						int lUVIndex = lUseIndex ? lUVElement->GetIndexArray().GetAt(lPolyIndexCounter) : lPolyIndexCounter;

						lUVValue = lUVElement->GetDirectArray().GetAt(lUVIndex);

						glm::vec2 vertexPosition(lUVValue.mData[0], lUVValue.mData[1]);
						(*_textureUVs).add(vertexPosition);

						lPolyIndexCounter++;
					}
				}
			}
		}
	}
}

VertexArray* AnimatedModel::getVertexArrayObject()
{
	return &VAO;
}

bool AnimatedModel::LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
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

	//FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		//FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		//FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		//FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		//FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		//FBXSDK_printf("\n");

		for (i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			//FBXSDK_printf("    Animation Stack %d\n", i);
			//FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			//FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			//FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			//FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			//FBXSDK_printf("\n");
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