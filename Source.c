#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535
#define WIDTH 1024 
#define HEIGHT 512
#define PLAYERWANDLENGTH 18.0
#define RAYADDENDUM 1.5

struct point
{
	float x;
	float y;
};

float px, py; // player position
float pdx, pdy, pa; // player change in position and angle
float blockPadding=2;
float displacementFactor = 0.5;
float angularUnitOfChange = 0.15;
struct point rayEndPoints[200];


void init()
{
	glClearColor(0.3,0.3,0.3,0);
	gluOrtho2D(0,WIDTH,HEIGHT,0);
	px = 300;
       	py = 300;
	pa = PI * 0.5;
	pdx = cos(pa) *  PLAYERWANDLENGTH;
	pdy = sin(pa) * PLAYERWANDLENGTH;
}

void drawPlayer()
{
	glColor3f(1,1,0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2f(px,py);
	glEnd();
	
	glColor3f(1,1,0);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(px,py);
	glVertex2f(px+pdx,py-pdy);
	glEnd();
}

int mapLength=8, mapHeight=8, mapBlockSize=64;

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
	int mapBlockIndex = (mapLength*(((int)y)/mapBlockSize)) + (((int)x)/mapBlockSize);
	return (map[mapBlockIndex]==1);
}

void drawMap2D()
{
	for (int x=0; x<mapLength; x++)
		for (int y=0; y<mapHeight; y++)
		{
			if (map[ (mapLength*y) + x]==1)
			       	glColor3f(1,1,1);
			else
				glColor3f(0,0,0);

			glBegin(GL_QUADS);
			glVertex2i(mapBlockSize*x,                  mapBlockSize*y);
			glVertex2i(mapBlockSize*x,                  mapBlockSize*(y+1)-blockPadding);
			glVertex2i(mapBlockSize*(x+1)-blockPadding, mapBlockSize*(y+1)-blockPadding);
			glVertex2i(mapBlockSize*(x+1)-blockPadding, mapBlockSize*(y));
			glEnd();
		}
}

void drawRays()
{
	struct point* rayEndPointsAddress = rayEndPoints;	
	for (float rayAngle=pa-(PI*0.25); rayAngle<pa+(PI*0.25); rayAngle+=0.03)
	{
		glColor3f(0,1,0);
		glLineWidth(1);
		glBegin(GL_LINES);
		glVertex2f(px, py);
		
		struct point rayEndPoint = { px, py };

		float i = 1;
		while(!pointIsInWall(rayEndPoint.x, rayEndPoint.y))
		{
			rayEndPoint.x = px + cos(rayAngle)*PLAYERWANDLENGTH*i;
			rayEndPoint.y = py - sin(rayAngle)*PLAYERWANDLENGTH*i;
			i += 2; //1.0e-5;
		}
		*rayEndPointsAddress++ = rayEndPoint;	// Append point to array
		glVertex2f(rayEndPoint.x, rayEndPoint.y);
		glEnd();
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawMap2D();
	drawPlayer();
	drawRays();
	glutSwapBuffers();
}

void modifyPositionOfPlayer(unsigned char key)
{
	float new_px = px;
       	float new_py = py;
	float angularUnitOfChange = 0.15;
	
	if (key == 'a')
		pa += angularUnitOfChange;	
	if (key == 'd')
		pa -= angularUnitOfChange;
	
	if (key == 'w')
	{
		new_px += displacementFactor * pdx;
		new_py -= displacementFactor * pdy;
	}
	if (key == 's')
	{
		new_px -= displacementFactor * pdx;
		new_py += displacementFactor * pdy;
	}

	if (pointIsInWall(new_px, new_py))
	{
		new_px = px;
		new_py = py;
	}
	else
	{
		px = new_px;
		py = new_py;
	}
		
	pdx = cos(pa) * PLAYERWANDLENGTH;
	pdy = sin(pa) * PLAYERWANDLENGTH;	

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
