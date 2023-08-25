#pragma once

#ifdef CLIENT

#include <SDL2/SDL.h>

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
			istate->left = state;
		}
		else
		{
			istate->strafeLeft = state;
		}
		break;
	case SDLK_s:
		istate->back = state;
		break;
	case SDLK_d:
		if (e.key.keysym.mod & KMOD_CTRL)
		{
			istate->right = state;
		}
		else
		{
			istate->strafeRight = state;
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
			state.q = 1;
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_q)
			{
				state.q = 1;
				break;
			}
			setButtonState(e, &state, 1);
			break;
		case SDL_KEYUP:
			setButtonState(e, &state, 0);
			break;
		case SDL_MOUSEMOTION:
			state.mouseX = e.motion.x;
			state.mouseY = e.motion.y;
			state.mouseDX = e.motion.xrel;
			state.mouseDY = e.motion.yrel;
			break;
		};
	}

	return state;
}

#endif