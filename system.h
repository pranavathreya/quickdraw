#pragma once

#include <memory>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <netdb.h>

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

void sendMessageToServer(int serverFileDes, char* messageBuffer, const Player *player,
	       InputState istate, double deltaTime)
{
	int errsv, bytesWritten;

	memset(messageBuffer, '\0', MSG_SIZE);
	snprintf(messageBuffer, MSG_SIZE, "%f\n%f\n%f\n%f\n%f\n", 
			player->speed.x,
			player->speed.y,
			player->position.x,
			player->position.y,
			player->playerAngle);

	bytesWritten = write(serverFileDes, messageBuffer, MSG_SIZE);
	if (bytesWritten != MSG_SIZE) 
	{
		fprintf(stderr, "partial/failed write: %d/%d\n", bytesWritten, MSG_SIZE);
		errsv = errno;
		gai_strerror(errsv);
		exit(EXIT_FAILURE);
	}

	fprintf(stderr, "client: wrote %d bytes of messageBuffer to server:\n%s",
		       	bytesWritten, messageBuffer);
}

void mainLoop(Player *player, SDL_Window *window, void (*displayCallback)(Player *), int serverFileDes) {
	InputState istate;

	uint64_t NOW = SDL_GetPerformanceCounter();
	uint64_t LAST = 0;
	double deltaTime = 0;
	char messageBuffer[MSG_SIZE];

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
		
		sendMessageToServer(serverFileDes, messageBuffer, player, istate, deltaTime);
		*player = inputManager(*player, istate, deltaTime);
		istate.resetMouseDelta();

		displayCallback(player);
		SDL_GL_SwapWindow(window);
	}
}
