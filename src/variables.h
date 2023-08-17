#pragma once

#include <SDL2/SDL.h>

#include "mathdefinitions.h"

#define WIDTH 512 
#define HEIGHT 512 
#define PLAYER_WAND_LENGTH 18.0
#define RAY_COUNT 200
#define MSG_SIZE 500 

typedef struct Player
{
	Vec2 speed;
	Vec2 position;
	float playerAngle;
} Player;

Player *player_new() 
{
	Vec2 speed = vec2_zero();
	Vec2 position = vec2_zero();
	float playerAngle = 0.0f;

	Player *player = (Player *) malloc(sizeof(Player));
	player->speed = speed;
	player->position = position;
	player->playerAngle = playerAngle;

	return player;
}

typedef struct Color
{
	float red;
	float green;
	float blue;
} Color;

// TODO: stdize fieldnames, compress dirVec into one field
typedef struct Ray
{
	float directionVectorX;
	float directionVectorY;
	Vec2 rayEndPosition;
	float rayAngle;
	float rayLength;
	int isHorizontalTraversing;
	Color color;
} Ray;

typedef struct Column
{
	int columnX;
	float columnHeight;
	float columnWidth;
	Color color;
} Column;

typedef struct
{
	uint8_t q;
	uint8_t forward;
	uint8_t left;
	uint8_t back;
	uint8_t right;
	uint8_t strafeLeft;
	uint8_t strafeRight;
	uint32_t mouseX;
	uint32_t mouseY;
	int32_t mouseDX;
	int32_t mouseDY;
} InputState;

InputState inputstate_new() {
	return (InputState) {
		.q = 0,
		.forward = 0,
		.left = 0,
		.back = 0,
		.right = 0,
		.strafeLeft = 0,
		.strafeRight = 0,
		.mouseX = 0,
		.mouseY = 0,
		.mouseDX = 0,
		.mouseDY = 0,
	};
}

uint8_t input_state_no_buttons(InputState istate)
{
	return !istate.forward && !istate.back && !istate.strafeLeft && !istate.strafeRight;
}

void reset_mouse_delta(InputState *istate)
{
	istate->mouseDX = 0;
	istate->mouseDY = 0;
}

typedef struct
{
	SDL_Window *window;
	SDL_GLContext glCtx;
} WindowContext;

/**
 * @brief Destroys this context at the end of its lifetime.
 * Use this with std::unique_ptr for best results.
 * 
 */
void sdl_quit(WindowContext *ctx)
{
	printf("Quitting...\n");
	SDL_GL_DeleteContext(ctx->glCtx);
	SDL_DestroyWindow(ctx->window);
	SDL_Quit();
	printf("Bye.\n");
}
