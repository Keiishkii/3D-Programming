#ifndef SHADERS
#define SHADERS

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

namespace Shaders
{
	class Shaders
	{
	public:
		static void generateVertexShader(GLuint& _shaderID);
		static void generateFragmentShader(GLuint& _shaderID);
	private:
		static const GLchar* vertexShaderSrc;
		static const GLchar* fragmentShaderSrc;
	};
}

#endif // !SHADERS
