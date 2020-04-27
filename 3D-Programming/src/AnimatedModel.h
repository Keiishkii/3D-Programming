#ifndef ANIMATEDMODEL
#define ANIMATEDMODEL

#include "VertexArray.h"
#include "Joint.h"

#include <fbxsdk.h>

class AnimatedModel
{
private:
	std::vector<Joint> rootJoints;
	VertexArray VAO;

public:
	AnimatedModel(std::string _modelPath, FbxManager* _manager, FbxScene* _scene, bool &_success);
	VertexArray* getVertexArrayObject();

	void operateOnNode(FbxNode* _node, std::vector<glm::vec3>& _positionsV, std::vector<GLfloat>& _textureCoordV, int& _vertexCount);
	bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);
};

#endif // !ANIMATEDMODEL