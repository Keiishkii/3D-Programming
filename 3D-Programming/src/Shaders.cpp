#include "Shaders.h"

#include <exception>

namespace Shaders
{
	const GLchar* Shaders::vertexShaderSrc =
		"attribute vec3 in_Position;												" \
		"attribute vec3 in_Normal;													" \
		"attribute vec2 in_TextureCoord;											" \
		"attribute vec3 in_Tangent;													" \
		"attribute vec3 in_Bitangent;												" \
		"																			" \
		"uniform mat4 in_InverseVeiwing;											" \
		"uniform mat4 in_Projection;												" \
		"uniform mat4 in_Model;														" \
		"																			" \
		"varying vec2 out_TextureCoord;												" \
		"varying vec3 out_Normal;													" \
		"varying vec3 out_FragmentPosition;											" \
		"																			" \
		"																			" \
		"void main()																" \
		"{																			" \
		"	out_TextureCoord = in_TextureCoord;										" \
		"	out_Normal = in_Normal;													" \
		"	out_FragmentPosition = vec3(in_Model * vec4(in_Position, 1.0));			" \
		"																			" \
		"	gl_Position = in_Projection * in_InverseVeiwing * in_Model * vec4(in_Position, 1.0);		" \
		"}																			";

	const GLchar* Shaders::fragmentShaderSrc =
		"uniform sampler2D in_Texture;												" \
		"uniform sampler2D in_NormalMap;												" \
		"																			" \
		"varying vec2 out_TextureCoord;												" \
		"varying vec3 out_Normal;													" \
		"varying vec3 out_FragmentPosition;											" \
		"																			" \
		"																			" \
		"void main()																" \
		"{																			" \
		"	vec3 ambiantColour = vec3(1, 1, 1);										" \
		"	float ambiantIntensity = 0.4;											" \
		"																			" \
		"	vec3 diffuseColour = vec3(1, 1, 1);										" \
		"	vec3 diffusePosition = vec3(20, 20, 20);								" \
		"																			" \
		"	vec3 normal = normalize(out_Normal);									" \
		"	vec3 lightDirection = normalize(diffusePosition - out_FragmentPosition);" \
		"																			" \
		"	float diffuseIntensity = max(dot(normal, lightDirection), 0.0);			" \
		"																			" \
		"	vec3 ambiantLighting = ambiantIntensity * ambiantColour;				" \
		"	vec3 diffuseLighting = diffuseIntensity * diffuseColour;				" \
		"																			" \
		"	vec3 fragmentlighting = min((diffuseLighting + ambiantLighting), 1);	" \
		"																			" \
		"	vec4 texture = texture2D( in_Texture , vec2(out_TextureCoord.x, 1 - out_TextureCoord.y));					" \
		"																			" \
		"	gl_FragColor = texture * vec4(fragmentlighting, 1.0);							" \
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