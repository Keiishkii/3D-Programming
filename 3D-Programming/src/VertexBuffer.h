#ifndef VERTEXBUFFER
#define VERTEXBUFFER

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <vector>

class VertexBuffer
{
private:
	GLuint id = 0;
	int components = 0;
	std::vector<GLfloat> data;
	bool dirty = false;
public:
	VertexBuffer();

	GLuint getID();

	void add(const glm::vec2& value);
	void add(const glm::vec3& value);
	void add(const glm::vec4& value);
};

#endif
