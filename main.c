#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include "rendering3D.h"
#include "physics.h"

Player player;

void init()
{
	glClearColor(0.3,0.3,0.3,0);
	gluOrtho2D(0,WIDTH,HEIGHT,0);
	player.playerX = 300;
       	player.playerY = 300;
	player.playerAngle = PI * 0.5;
	player.playerWandVectorX = cos(player.playerAngle) *  PLAYER_WAND_LENGTH;
	player.playerWandVectorY = sin(player.playerAngle) * PLAYER_WAND_LENGTH;
	player.mouseRelativeX = 0;
	player.mouseRelativeY = 0;
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawPlayer2D(player);
	drawRays(player);
	drawColumns(player);
	glutSwapBuffers();
}

void buttons(unsigned char key, int x, int y)
{
	player = keyboardMovementManager(key, player);
	glutPostRedisplay();
}

void mouse(int x, int y) {
	player = mouseModifyPlayerOrientation(x, y, player);
	glutPostRedisplay();
}

int main(argc, argv)
int argc;
char *argv[];
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutCreateWindow("KBros\' Raycaster");
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(buttons);
	glutPassiveMotionFunc(mouse);
	glutMotionFunc(mouse);
	glutMainLoop();
}
