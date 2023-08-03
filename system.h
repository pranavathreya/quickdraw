#pragma once

#include <memory>
#include <SDL2/SDL.h>

#include "input.h"

std::unique_ptr<WindowContext> sdl_init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		printf("Could not init sdl! %s", SDL_GetError());
		return nullptr;
	}

	SDL_Window *window =
		SDL_CreateWindow("KBros\' Raycaster", 100, 100, WIDTH, HEIGHT,
						 SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (window == nullptr)
	{
		printf("Could not create sdl window! %s", SDL_GetError());
		SDL_Quit();
		return nullptr;
	}

	// Use OpenGL 2.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// Create context
	SDL_GLContext gContext = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	return std::unique_ptr<WindowContext>(
		new WindowContext{.window = window, .glCtx = gContext});
}


#include <sys/socket.h>
#include "networkTools.h"
#include <netdb.h>
#include <unistd.h>

#define BUF_SIZE 18 

void mainLoop(Player *player, SDL_Window *window, void (*displayCallback)(Player *), int serverFileDes, char* message) {
	InputState istate;

	uint64_t NOW = SDL_GetPerformanceCounter();
	uint64_t LAST = 0;
	double deltaTime = 0;

	// While application is running
	while (true)
	{
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime =
			(double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

		istate = processInputEvents(istate);

		if (istate.q)
			break;

		write(serverFileDes, message, sizeof(message));
		*player = inputManager(*player, istate, deltaTime);
		istate.resetMouseDelta();

		displayCallback(player);
		SDL_GL_SwapWindow(window);
	}
}
