#pragma once

#include <SDL2/SDL.h>
#include <iostream>

#include "variables.h"
#include "physics.h"

void setButtonState(SDL_Event e, InputState *istate, int state)
{
	switch (e.key.keysym.sym)
	{
	case SDLK_w:
		istate->forward = state;
		break;
	case SDLK_a:
		if (e.key.keysym.mod & KMOD_CTRL)
		{
			istate->strafeLeft = state;
		}
		else
		{
			istate->left = state;
		}
		break;
	case SDLK_s:
		istate->back = state;
		break;
	case SDLK_d:
		if (e.key.keysym.mod & KMOD_CTRL)
		{
			istate->strafeRight = state;
		}
		else
		{
			istate->right = state;
		}
		break;
	};
}

InputState processInputEvents(InputState state)
{
	// Handle events on queue
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		switch (e.type)
		{
		case SDL_QUIT:
			state.q = true;
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_q)
			{
				state.q = true;
				break;
			}
			setButtonState(e, &state, true);
			break;
		case SDL_KEYUP:
			setButtonState(e, &state, false);
			break;
		case SDL_MOUSEMOTION:
			state.mouseX = e.motion.x;
			state.mouseY = e.motion.y;
			state.mouseDX = e.motion.xrel;
			state.mouseDY = e.motion.yrel;
			break;
		};
	}

	std::cout << "input handled." 
			  << "w: " << state.forward << std::endl
			  << "a: " << state.strafeLeft << std::endl
			  << "s: " << state.back << std::endl
			  << "d: " << state.strafeRight << std::endl;
	return state;
}