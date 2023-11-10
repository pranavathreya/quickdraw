#include <stdio.h>
#include <SDL2/SDL.h>

#define CLIENT

#include "variables.h"
#include "rendering3D.h"
#include "physics.h"
#include "input.h"
#include "system.h"
#include "networkTools.h"

void init(Player *player)
{
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, WIDTH, HEIGHT, 0);
	player->position = (Vec2) {.x = 300, .y = 300};
	player->playerAngle = M_PI * 0.5;
}

void display(Player *player)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//drawMap2D();
	//drawPlayer2D(*player);
	drawRays(*player);
	drawColumns(*player);
}


int main(int argc, char *argv[])
{
	ClientState clientState;
	int clientFileDes, serverFileDes;

	Player *player = player_new();
	WindowContext *ctx = sdl_init();
	init(player);
	clientFileDes = bindToAddress("localhost", argv[2]);
	serverFileDes = connectToAddress(argv[1], argv[3]);
	
	clientState.player = player;
	mainLoop(&clientState, ctx->window, &display, serverFileDes);
	
	free(player);
	sdl_quit(ctx);
}
