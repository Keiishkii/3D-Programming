#include <SDL2/SDL.h>
#include <GL/glew.h>

#include <exception>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "Model.h"
#include "Shaders.h"

void initialiseWindowAndOpenGL(SDL_Window** _window);
void gameLoop(SDL_Window* window, const GLuint& programId, const GLuint& vaoId, int const _positionsListSize);
void addTriangle(std::vector<GLfloat>& positionsV, std::vector<GLfloat>& coloursV, GLfloat* positionsA, int positionSize, GLfloat* coloursA, int colourSize);

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

int main(int argc, char *argv[])
{
	SDL_Window* window;
	initialiseWindowAndOpenGL(&window);

	Model objectToRender("models/Monado.obj", glm::vec4(0, -2.5, -20, 1));
		
	/////// ------------------------------------------------------------------			-- Create Position and Colours

	std::vector<GLfloat> positionsVector;
	std::vector<GLfloat> coloursVector;
	
	GLfloat colours[] = { 1.0f, 0.0f, 0.0f};

	int vertexListSize = (*objectToRender.getTrigList()).size() * 3;


	for (int i = 0; i < (*objectToRender.getTrigList()).size(); i++)
	{
		positionsVector.push_back((*objectToRender.getTrigList()).at(i).getVertex1().x);
		positionsVector.push_back((*objectToRender.getTrigList()).at(i).getVertex1().y);
		positionsVector.push_back((*objectToRender.getTrigList()).at(i).getVertex1().z);
		
		positionsVector.push_back((*objectToRender.getTrigList()).at(i).getVertex2().x);
		positionsVector.push_back((*objectToRender.getTrigList()).at(i).getVertex2().y);
		positionsVector.push_back((*objectToRender.getTrigList()).at(i).getVertex2().z);

		positionsVector.push_back((*objectToRender.getTrigList()).at(i).getVertex3().x);
		positionsVector.push_back((*objectToRender.getTrigList()).at(i).getVertex3().y);
		positionsVector.push_back((*objectToRender.getTrigList()).at(i).getVertex3().z);

		for (int j = 0; j < 3; j++)
		{
			coloursVector.push_back(0.95);
			coloursVector.push_back(0.05);
			coloursVector.push_back(0.05);
			coloursVector.push_back(1);
		}
	}	

	std::cout << "Colour Size -> " << coloursVector.size() << std::endl;
	std::cout << "Position Size -> " << positionsVector.size() << std::endl;

	/////// ------------------------------------------------------------------			-- Apply Positions and Colours to VBO
	GLuint positionsVboId = 0;
	GLuint coloursVboId = 0;

	glGenBuffers(1, &positionsVboId);
	glGenBuffers(1, &coloursVboId);

	if (!positionsVboId)
	{
		throw std::exception();
	}

	if (!coloursVboId)
	{
		throw std::exception();
	}

	glBindBuffer(GL_ARRAY_BUFFER, positionsVboId);
	glBufferData(GL_ARRAY_BUFFER, positionsVector.size() * sizeof(positionsVector.at(0)), &positionsVector.at(0), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, coloursVboId);
	glBufferData(GL_ARRAY_BUFFER, coloursVector.size() * sizeof(coloursVector.at(0)), &coloursVector.at(0), GL_STATIC_DRAW);

	// Reset Bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/////// ------------------------------------------------------------------			 -- Apply VBO to VAO
	GLuint vaoId = 0;

	glGenVertexArrays(1, &vaoId);

	if (!vaoId)
	{
		throw std::exception();
	}

	glBindVertexArray(vaoId);

	glBindBuffer(GL_ARRAY_BUFFER, positionsVboId);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, coloursVboId);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)0);
	glEnableVertexAttribArray(1);

	// Reset the state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/////// ------------------------------------------------------------------			 -- Generate the Vertex Shader

	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &Shader::vertexShaderSrc, NULL);
	glCompileShader(vertexShaderId);
	GLint success = 0;
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);

	/////// ------------------------------------------------------------------			 -- Generate the Fragment Shader

	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderId, 1, &Shader::fragmentShaderSrc, NULL);
	glCompileShader(fragmentShaderId);
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		throw std::exception();
	}

	if (!success)
	{
		throw std::exception();
	}

	/////// ------------------------------------------------------------------

	// Create new shader program and attach our shader objects
	GLuint programId = glCreateProgram();

	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);

	// Ensure the VAO "position" attribute stream gets set as the first position
	// during the link.

	glBindAttribLocation(programId, 0, "in_Position");
	glBindAttribLocation(programId, 1, "in_Colour");

	// Perform the link and check for failure
	glLinkProgram(programId);
	glGetProgramiv(programId, GL_LINK_STATUS, &success);

	if (!success)
	{
		throw std::exception();
	}

	// Detach and destroy the shader objects. These are no longer needed
	// because we now have a complete shader program.
	glDetachShader(programId, vertexShaderId);
	glDeleteShader(vertexShaderId);
	glDetachShader(programId, fragmentShaderId);
	glDeleteShader(fragmentShaderId);

	/////// ------------------------------------------------------------------

	gameLoop(window, programId, vaoId, positionsVector.size());

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void gameLoop(SDL_Window* window, const GLuint& programId, const GLuint& vaoId, int const _positionsListSize)
{
	bool quit = false;

	float xt = 0;
	float yt = 0;
	float zt = 0;

	GLint modelLoc = glGetUniformLocation(programId, "in_Model");
	GLint projectionLoc = glGetUniformLocation(programId, "in_Projection");

	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.f);

	bool oneDown = false;
	bool twoDown = false;

	while (!quit)
	{
		SDL_Event event = { 0 };

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_1:
					oneDown = true;
					break;
				case SDLK_2:
					twoDown = true;
					break;
				}
				break;
			case SDL_KEYUP:
				switch (event.key.keysym.sym)
				{
				case SDLK_1:
					oneDown = false;
					break;
				case SDLK_2:
					twoDown = false;
					break;
				}
			}
		}

		if (oneDown)
		{
			yt += 0.5;
		}
		if (twoDown)
		{
			zt += 0.5;
		}

		int width = 0;
		int height = 0;
		SDL_GetWindowSize(window, &width, &height);
		glViewport(0, 0, width, height);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.f);
		/////// ------------------------------------------------------------------

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/////// ------------------------------------------------------------------

		// Instruct OpenGL to use our shader program and our VAO
		glUseProgram(programId);
		glBindVertexArray(vaoId);

		glm::mat4 model(1.0f);
		model = glm::translate(model, glm::vec3(0, 0, -2.5f));
		model = glm::rotate(model, glm::radians(xt), glm::vec3(1, 0, 0));
		model = glm::rotate(model, glm::radians(yt), glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(zt), glm::vec3(0, 0, 1));

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		glEnable(GL_DEPTH_TEST);

		// Draw 3 vertices (a triangle)
		glDrawArrays(GL_TRIANGLES, 0, _positionsListSize / 3);

		glDisable(GL_DEPTH_TEST);

		// Reset the state
		glBindVertexArray(0);
		glUseProgram(0);

		/////// ------------------------------------------------------------------

		xt += 0.5;

		SDL_GL_SwapWindow(window);
	}
}

void addTriangle(std::vector<GLfloat>& positionsV, std::vector<GLfloat>& coloursV, GLfloat* positionsA, int positionSize, GLfloat* coloursA, int colourSize)
{
	std::cout << positionSize << std::endl;

	for (int i = 0; i < positionSize; i++)
	{
		positionsV.push_back(positionsA[i]);
	}

	for (int i = 0; i < colourSize; i++)
	{
		coloursV.push_back(coloursA[i]);
	}
}

void initialiseWindowAndOpenGL(SDL_Window** _window)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		throw std::exception();
	}

	*_window = SDL_CreateWindow("Triangle", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);

	if (!SDL_GL_CreateContext(*_window))
	{
		throw std::exception();
	}

	if (glewInit() != GLEW_OK)
	{
		throw std::exception();
	}

}