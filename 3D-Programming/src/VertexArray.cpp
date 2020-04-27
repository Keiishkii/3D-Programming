#include "VertexArray.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &id);

	if (!id)
	{
		throw std::exception();
	}
}

void VertexArray::setVertexCount(int _vertexCount)
{
	vertexCount = _vertexCount;
}

GLuint VertexArray::getID()
{
	if (dirty)
	{
		glBindVertexArray(id);

			//VertexPositions
		glBindBuffer(GL_ARRAY_BUFFER, vertexPositions->getID());
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

			//TexturesPositions
		//glBindBuffer(GL_ARRAY_BUFFER, textureVboId);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
		//glEnableVertexAttribArray(1);

		// Reset the state
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		dirty = false;
	}

	return id;
}

void VertexArray::setBuffer(std::string _buffer, VertexBuffer* _content)
{
	if (_buffer._Equal("VertexPositions"))
	{
		vertexPositions = _content;
	}

	dirty = true;
}