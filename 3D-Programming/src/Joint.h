#ifndef JOINT
#define JOINT

#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Joint
{
	int jointID = 0;

	glm::mat4 transorm = glm::mat4(1);

	std::vector<Joint> childJoints;
};

#endif // !JOINT
