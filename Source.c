#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535
#define WIDTH 1024 
#define HEIGHT 512
#define PLAYERWANDLENGTH 18.0
#define RAYADDENDUM 1.5

struct RayEndPoint
{
	float rayX;
	float rayY;
	float rayAngle;
};

float playerX, playerY; // player position
float playerDeltaX, playerDeltaY, playerAngle; // player change in position and angle
float blockPadding=2;
float displacementFactor = 0.5;
float angularUnitOfChange = 0.15;
struct RayEndPoint rayEndPointArray[53];


void init()
{
	glClearColor(0.3,0.3,0.3,0);
	gluOrtho2D(0,WIDTH,HEIGHT,0);
	playerX = 300;
       	playerY = 300;
	playerAngle = PI * 0.5;
	playerDeltaX = cos(playerAngle) *  PLAYERWANDLENGTH;
	playerDeltaY = sin(playerAngle) * PLAYERWANDLENGTH;
}

void drawPlayer()
{
	glColor3f(1,1,0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2f(playerX,playerY);
	glEnd();
	
	glColor3f(1,1,0);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(playerX,playerY);
	glVertex2f(playerX+playerDeltaX,playerY-playerDeltaY);
	glEnd();
}

int mapColumns=8, mapRows=8, mapBlockSize=64;

int map[]=
{
	1,1,1,1,1,1,1,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
};

int pointIsInWall(float x, float y)
{
	int mapBlockIndex = (mapColumns*(((int)y)/mapBlockSize)) + (((int)x)/mapBlockSize);
	return (map[mapBlockIndex]==1);
}

void drawMap2D()
{
	for (int j=0; j<mapColumns; j++)
		for (int i=0; i<mapRows; i++)
		{
			if (map[ (mapColumns*i) + j]==1)
			       	glColor3f(1,1,1);
			else
				glColor3f(0,0,0);

			glBegin(GL_QUADS);
			glVertex2i(mapBlockSize*j,                  mapBlockSize*i);
			glVertex2i(mapBlockSize*j,                  mapBlockSize*(i+1)-blockPadding);
			glVertex2i(mapBlockSize*(j+1)-blockPadding, mapBlockSize*(i+1)-blockPadding);
			glVertex2i(mapBlockSize*(j+1)-blockPadding, mapBlockSize*i);
			glEnd();
		}
}

void drawRays()
{
	struct RayEndPoint* rayEndPointArrayAddress = rayEndPointArray;	
	for (float rayAngle=playerAngle-(PI*0.25); rayAngle<playerAngle+(PI*0.25); rayAngle+=0.03)
	{
		struct RayEndPoint rayEndPoint = { playerX, playerY, rayAngle };
		float i = 1;
		while(!pointIsInWall(rayEndPoint.rayX, rayEndPoint.rayY))
		{
			rayEndPoint.rayX = playerX + cos(rayAngle)*PLAYERWANDLENGTH*i;
			rayEndPoint.rayY = playerY - sin(rayAngle)*PLAYERWANDLENGTH*i;
			i += 1.0;
		}
		*rayEndPointArrayAddress++ = rayEndPoint;	// Append point to array
		
		glColor3f(0,1,0);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2f(playerX, playerY);
		glVertex2f(rayEndPoint.rayX, rayEndPoint.rayY);
		glEnd();
	}
}

void drawColumns()
{
	for (int i=52; i!=0; i--)
	{
		float columnHeight = fabs( cos(rayEndPointArray[i].rayAngle) / (rayEndPointArray[i].rayX - playerX) ) * 2.5e4;

		glColor3f(0,1,0);
		glLineWidth(9);
		glBegin(GL_LINES);
		glVertex2f(512+i*9, 256-columnHeight/2);
		glVertex2f(512+i*9, 256+columnHeight/2);
		glEnd();
		
		printf("rayEndPointArray[%d]: %f, %f, %f, %f\n", i, rayEndPointArray[i].rayX, rayEndPointArray[i].rayY, rayEndPointArray[i].rayAngle, columnHeight);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawPlayer();
	drawRays();
	drawColumns();
	glutSwapBuffers();
}

void modifyPositionOfPlayer(unsigned char key)
{
	float newPlayerX = playerX;
       	float newPlayerY = playerY;
	float angularUnitOfChange = 0.15;
	
	if (key == 'a')
		playerAngle += angularUnitOfChange;	
	if (key == 'd')
		playerAngle -= angularUnitOfChange;
	
	if (key == 'w')
	{
		newPlayerX += displacementFactor * playerDeltaX;
		newPlayerY -= displacementFactor * playerDeltaY;
	}
	if (key == 's')
	{
		newPlayerX -= displacementFactor * playerDeltaX;
		newPlayerY += displacementFactor * playerDeltaY;
	}

	if (pointIsInWall(newPlayerX, newPlayerY))
	{
		newPlayerX = playerX;
		newPlayerY = playerY;
	}
	else
	{
		playerX = newPlayerX;
		playerY = newPlayerY;
	}
		
	playerDeltaX = cos(playerAngle) * PLAYERWANDLENGTH;
	playerDeltaY = sin(playerAngle) * PLAYERWANDLENGTH;	

}
void buttons(unsigned char key, int x, int y)
{
	modifyPositionOfPlayer(key);	
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
	glutMainLoop();
}
