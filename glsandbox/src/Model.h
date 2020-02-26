#ifndef MODEL
#define MODEL

#include "Triangle.h"

#include <string>
#include <fstream>
#include <vector>

#include <glm/glm.hpp>

class Model
{
private:
	glm::vec4 position{ 0, 0, 0, 1 };
	std::vector<Triangle> triangleList;

public:
	Model(std::string _path);
	Model(std::string _path, glm::vec4 _position);

	void loadModel(std::string _path);

	glm::vec3 getPostion() { return position; }
	void setPostion(glm::mat4x4 _newPos);
	std::vector<Triangle>* getTrigList() { return &triangleList; }
};

#endif // !MODEL
