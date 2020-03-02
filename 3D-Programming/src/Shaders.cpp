#include "Shaders.h"

#include <exception>

namespace Shaders
{
	const GLchar* Shaders::vertexShaderSrc =
		"attribute vec3 in_Position;												" \
		"attribute vec2 in_TextureCoord;											" \
		"																			" \
		"uniform mat4 in_InverseVeiwing;											" \
		"uniform mat4 in_Projection;												" \
		"uniform mat4 in_Model;														" \
		"																			" \
		"varying vec2 out_TextureCoord;												" \
		"																			" \
		"																			" \
		"void main()																" \
		"{																			" \
		"	out_TextureCoord = in_TextureCoord;										" \
		"																			" \
		"	gl_Position = in_Projection * in_InverseVeiwing * in_Model * vec4(in_Position, 1.0);		" \
		"}																			";

	const GLchar* Shaders::fragmentShaderSrc =
		"uniform sampler2D in_Texture;												" \
		"varying vec2 out_TextureCoord;												" \
		"																			" \
		"																			" \
		"void main()																" \
		"{																			" \
		"	vec4 text = texture2D( in_Texture , vec2(out_TextureCoord.x, 1 - out_TextureCoord.y));					" \
		"	gl_FragColor = vec4(out_TextureCoord, 0.0, 1.0);													" \
		"}																			";

	void Shaders::generateVertexShader(GLuint& _shaderID)
	{
		_shaderID = glCreateShader(GL_VERTEX_SHADER);

		glShaderSource(_shaderID, 1, &vertexShaderSrc, NULL);
		glCompileShader(_shaderID);

		GLint success = 0;

		glGetShaderiv(_shaderID, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			throw std::exception();
		}
	}

	void Shaders::generateFragmentShader(GLuint& _shaderID)
	{
		_shaderID = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(_shaderID, 1, &fragmentShaderSrc, NULL);
		glCompileShader(_shaderID);

		GLint success = 0;

		glGetShaderiv(_shaderID, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			throw std::exception();
		}
	}
}