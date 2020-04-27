#include "AnimatedModel.h"

#include <iostream>

#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))


AnimatedModel::AnimatedModel(std::string _modelPath, FbxManager * _manager, FbxScene * _scene, bool &_success)
{
	bool sceneSuccsess = true;

	FbxString localFilePath(_modelPath.c_str());

	sceneSuccsess = LoadScene(_manager, _scene, localFilePath.Buffer());

	if (sceneSuccsess)
	{
		FbxNode* routeNode = _scene->GetRootNode();

		int polygonCount = 0;
		int vertexCount = 0;

		std::vector<glm::vec3> positionsVector;
		std::vector<GLfloat> textureCoordsVector;

		if (routeNode)
		{
			operateOnNode(routeNode, positionsVector, textureCoordsVector, vertexCount);
			std::cout << "Polygon count: " << polygonCount << std::endl;
			std::cout << "Vertex varaible value count: " << positionsVector.size() << std::endl;
		}
		
		VertexBuffer* positionsVBO = new VertexBuffer();

		for (int i = 0; i < positionsVector.size(); i++)
		{
			positionsVBO->add(positionsVector.at(i));
		}

		VAO.setVertexCount(vertexCount);
		VAO.setBuffer("VertexPositions", positionsVBO);
	}
	else
	{
		_success = false;
	}
}


void AnimatedModel::operateOnNode(FbxNode* _node, std::vector<glm::vec3>& _positionsV, std::vector<GLfloat>& _textureCoordV, int& _vertexCount)
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