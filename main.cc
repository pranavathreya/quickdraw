#include <memory>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "variables.h"
#include "rendering3D.h"
#include "physics.h"
#include "input.h"
#include "system.h"

void init(Player *player)
{
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, WIDTH, HEIGHT, 0);
	player->position = Vec2{.x = 300, .y = 300};
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


#include "networkTools.h"
int main(int argc, char *argv[])
{
	int clientFileDes, serverFileDes;

	Player player;
	// This will automatically clean up when we quit.
	std::unique_ptr<WindowContext> ctx = sdl_init();
	init(&player);
	clientFileDes = bindOrConnectToAddress(argv[1], argv[2], 1);
	serverFileDes = bindOrConnectToAddress(argv[1], argv[3], 0);
	mainLoop(&player, ctx->window, &display, serverFileDes);
}
