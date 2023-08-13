#pragma once

#include <SDL2/SDL.h>

#include "input.h"
#include "physics.h"

/**
 * @brief To correctly manage this pointer, the caller must call sdl_quit()
 * 
 * @return WindowContext* 
 */
WindowContext *sdl_init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		printf("Could not init sdl! %s", SDL_GetError());
		return NULL;
	}

	SDL_Window *window =
		SDL_CreateWindow("KBros\' Raycaster", 100, 100, WIDTH, HEIGHT,
						 SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (window == NULL)
	{
		printf("Could not create sdl window! %s", SDL_GetError());
		SDL_Quit();
		return NULL;
	}

	// Use OpenGL 2.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// Create context
	SDL_GLContext gContext = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	WindowContext *ctx = (WindowContext *) malloc(sizeof(WindowContext));
	ctx->glCtx = gContext;
	ctx->window = window;
	return ctx;
}

void mainLoop(Player *player, SDL_Window *window, void displayCallback(Player *)) {
	InputState istate = inputstate_new();

	uint64_t NOW = SDL_GetPerformanceCounter();
	uint64_t LAST = 0;
	double deltaTime = 0;

	// While application is running
	while (1)
	{
		LAST = NOW;
		NOW = SDL_GetPerformanceCounter();

		deltaTime =
			(double)((NOW - LAST) * 1000 / (double)SDL_GetPerformanceFrequency());

		istate = processInputEvents(istate);

		if (istate.q)
			break;

		*player = updatePhysics(*player, istate, deltaTime);

		reset_mouse_delta(&istate);

		displayCallback(player);
		SDL_GL_SwapWindow(window);
	}
}