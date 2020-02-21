#pragma once

namespace Shader
{
	// Vertex Shader
	const GLchar *vertexShaderSrc =
		"attribute vec3 in_Position;												" \
		"attribute vec4 in_Colour;													" \
		"																			" \
//		"uniform mat4 in_Projection;												" 
//		"uniform mat4 in_Model;														" 
		"																			" \
		"varying vec4 out_Colour;													" \
		"																			" \
		"																			" \
		"void main()																" \
		"{																			" \
		"	out_Colour = in_Colour;													" \
		"																			" \
		"	gl_Position = vec4(in_Position, 1.0);									" \
//		"	gl_Position = in_Projection * in_Model * vec4(in_Position, 1.0);		" 
		"}												";

	// Fragment Shader
	const GLchar *fragmentShaderSrc =
		"varying vec4 out_Colour;													" \
		"void main()																" \
		"{																			" \
		"	gl_FragColor = out_Colour;												" \
		"}																			";

}