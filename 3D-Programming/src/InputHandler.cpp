#include "InputHandler.h"
#include <SDL2/SDL.h>

Inputs InputHandler::inputs {false};


Inputs* InputHandler::getInputs()
{
	return &inputs;
}

void InputHandler::processInputs()
{
	SDL_Event event = { 0 };

	inputs.xrel = 0;
	inputs.yrel = 0;
	inputs.mouseWheelY = 0;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_MOUSEMOTION:
			inputs.xrel = event.motion.xrel;
			inputs.yrel = event.motion.yrel;
			break;
		case SDL_MOUSEWHEEL:
			inputs.mouseWheelY = event.wheel.y;
			break;
		case SDL_QUIT:
			inputs.quit = true;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				inputs.quit = true;
				break;
			case SDLK_w:
				inputs.key_W = true;
				break;
			case SDLK_a:
				inputs.key_A = true;
				break;
			case SDLK_s:
				inputs.key_S = true;
				break;
			case SDLK_d:
				inputs.key_D = true;
				break;

			case SDLK_SPACE:
				inputs.key_SPACE = true;
				break;
			case SDLK_LSHIFT:
				inputs.key_LSHIFT = true;
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				inputs.key_W = false;
				break;
			case SDLK_a:
				inputs.key_A = false;
				break;
			case SDLK_s:
				inputs.key_S = false;
				break;
			case SDLK_d:
				inputs.key_D = false;
				break;

			case SDLK_SPACE:
				inputs.key_SPACE = false;
				break;
			case SDLK_LSHIFT:
				inputs.key_LSHIFT = false;
				break;
			}
			break;
		}
	}
}