#ifndef TRIANGLE
#define TRIANGLE

#include <vector>

#include <cmath>
#include "vec3.hpp"
#include "vec4.hpp"
#include "geometric.hpp"

class Triangle
{
private:
	static int IDCounter;

	glm::vec3 colour{ 255, 255, 255 };

	glm::vec4 normal{ 0 };

	glm::vec4 vertex_1{ 0 };
	glm::vec4 vertex_2{ 0 };
	glm::vec4 vertex_3{ 0 };
public:
	int ID = 0;

	Triangle() { ID = IDCounter++; }
	Triangle(glm::vec4 _normal, glm::vec4 _vertex1, glm::vec4 _vertex2, glm::vec4 _vertex3) : normal(_normal), vertex_1(_vertex1), vertex_2(_vertex2), vertex_3(_vertex3) { ID = IDCounter++; }

	glm::vec3 getColour() { return colour; }
	
	glm::vec4 getNormal() { return normal; }

	glm::vec4 getVertex1() { return vertex_1; }
	glm::vec4 getVertex2() { return vertex_2; }
	glm::vec4 getVertex3() { return vertex_3; }

	void setVertex1(glm::vec4 newVertex_1) { vertex_1 = newVertex_1; }
	void setVertex2(glm::vec4 newVertex_2) { vertex_2 = newVertex_2; }
	void setVertex3(glm::vec4 newVertex_3) { vertex_3 = newVertex_3; }
};

#endif // !TRIANGLE
