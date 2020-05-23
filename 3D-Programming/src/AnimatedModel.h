#ifndef ANIMATEDMODEL
#define ANIMATEDMODEL

#include "VertexArray.h"
#include "Joint.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <fbxsdk.h> 

class AnimatedModel
{
private:
	glm::vec3 transform = glm::vec3(0, 0, 0);

	std::vector<Joint> rootJoints;
	VertexArray VAO;

	void LoadUVInformation(FbxMesh* _mesh, std::shared_ptr<VertexBuffer> _textureUVs);
	void LoadMaterials(FbxMesh* pMesh);
	bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename);
	void operateOnNode(FbxNode* _node, std::shared_ptr<VertexBuffer> _vertexPositions, std::shared_ptr<VertexBuffer> _textureUVs, std::shared_ptr<VertexBuffer> _normals, int& _vertexCount);

public:
	AnimatedModel(std::string _modelPath, FbxManager* _manager, FbxScene* _scene, bool& _success, glm::vec3 _transform);
	void draw(GLuint _programID, const glm::mat4& _projection, const glm::mat4& _veiwing);
	VertexArray* getVertexArrayObject();
};

#endif // !ANIMATEDMODEL