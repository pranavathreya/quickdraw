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
	Vec2 speed = Vec2::zero();
	Vec2 position = Vec2::zero();
	float playerAngle = 0.0f;
} Player;

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
	Vec2 rayEndPosition = Vec2::zero();
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

struct InputState
{
	bool q = false;
	bool forward = false;
	bool left = false;
	bool back = false;
	bool right = false;
	bool strafeLeft = false;
	bool strafeRight = false;
	uint32_t mouseX = 0;
	uint32_t mouseY = 0;
	int32_t mouseDX = 0;
	int32_t mouseDY = 0;

	bool noButtons()
	{
		return !forward && !back && !strafeLeft && !strafeRight;
	}

	void resetMouseDelta()
	{
		mouseDX = 0;
		mouseDY = 0;
	}
};

struct WindowContext
{
	SDL_Window *window;
	SDL_GLContext glCtx;

	~WindowContext();
};

/**
 * @brief Destroys this context at the end of its lifetime.
 * Use this with std::unique_ptr for best results.
 * 
 */
WindowContext::~WindowContext()
{
	printf("Quitting...\n");
	SDL_GL_DeleteContext(glCtx);
	SDL_DestroyWindow(window);
	SDL_Quit();
	printf("Bye.\n");
}
