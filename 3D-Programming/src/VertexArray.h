#ifndef VERTEXARRAY
#define VERTEXARRAY

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <map>
#include <string>
#include <memory>

#include "VertexBuffer.h"


class VertexArray
{
private:
	GLuint id = 0;
	int vertexCount = 0;

	VertexBuffer* vertexPositions = nullptr;

	std::map<std::string, VertexBuffer *> vertexBuffers;

	bool dirty = false;
public:
	VertexArray();
	void setVertexCount(int _vertexCount);
	void setBuffer(std::string _buffer, VertexBuffer * _content);
	int getVertexCount() { return vertexCount; }
	GLuint getID();
};

#endif