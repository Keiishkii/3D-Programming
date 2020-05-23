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

	std::shared_ptr<VertexBuffer> vertexPositions;
	std::shared_ptr<VertexBuffer> textureUVs;
	std::shared_ptr<VertexBuffer> vertexNormals;
	std::shared_ptr<VertexBuffer> vertexTangents;
	std::shared_ptr<VertexBuffer> vertexBitangents;

	bool dirty = false;
public:
	VertexArray();

	GLuint getID();
	int getVertexCount() { return vertexCount; }
	std::shared_ptr<VertexBuffer> getVertexList() { return vertexPositions; }
	std::shared_ptr<VertexBuffer> getTextureUVList() { return textureUVs; }

	void setVertexCount(int _vertexCount);
	void setBuffer(std::string _buffer, std::shared_ptr<VertexBuffer> _content);
};

#endif