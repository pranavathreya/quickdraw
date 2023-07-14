#include <GL/glut.h>
#include "variables.h"

struct RayEndPoint
{
	float rayX;
	float rayY;
	float rayAngle;
};

struct RayEndPoint rayEndPointArray[RAY_COUNT];

void drawPlayerBody2D(Player __player)
{
	glColor3f(1,1,0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2f(__player.playerX,__player.playerY);
	glEnd();
}

void drawPlayerWand2D(Player __player)
{
	glColor3f(1,1,0);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(__player.playerX				, __player.playerY);
	glVertex2f(__player.playerX + __player.playerWandVectorX, __player.playerY - __player.playerWandVectorY);
	glEnd();
}

void drawPlayer2D(Player _player)
{
	drawPlayerBody2D(_player);
	drawPlayerWand2D(_player);
}

int mapColumns=8, mapRows=8, mapBlockSideLength=64;

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


void drawSingleBlock2D(int currentMapColumn, int currentMapRow, int blockPadding)
{
	glBegin(GL_QUADS);
	glVertex2i(mapBlockSideLength*currentMapColumn,                  mapBlockSideLength*currentMapRow);
	glVertex2i(mapBlockSideLength*currentMapColumn,                  mapBlockSideLength*(currentMapRow+1)-blockPadding);
	glVertex2i(mapBlockSideLength*(currentMapColumn+1)-blockPadding, mapBlockSideLength*(currentMapRow+1)-blockPadding);
	glVertex2i(mapBlockSideLength*(currentMapColumn+1)-blockPadding, mapBlockSideLength*currentMapRow);
	glEnd();
}

void drawMap2D()
{
	for (int currentColumn=0; currentColumn<mapColumns; currentColumn++)
		for (int currentRow=0; currentRow<mapRows; currentRow++)
		{
			if (map[ (mapColumns*currentRow) + currentColumn]==1)
			       	glColor3f(1,1,1);
			else
				glColor3f(0,0,0);

			drawSingleBlock2D(currentColumn, currentRow, 2);
		}
}

int pointIsInWall(float x, float y)
{
	int mapBlockIndex = (mapColumns*(((int)y)/mapBlockSideLength)) + (((int)x)/mapBlockSideLength);
	return (map[mapBlockIndex]==1);
}

struct RayEndPoint findRayEndPoint( float _playerX, float _playerY, float _rayAngle)
{
	struct RayEndPoint rayEndPoint = { _playerX, _playerY, _rayAngle };

	for(int i=0; !pointIsInWall(rayEndPoint.rayX, rayEndPoint.rayY); ++i)
	{
		rayEndPoint.rayX = (float) ((( (int)_playerX) / mapBlockSideLength ) + i ) * mapBlockSideLength;
		printf("rayX = %f; tan = %f\n", rayEndPoint.rayX, tan(rayEndPoint.rayAngle));
		rayEndPoint.rayY = tan(rayEndPoint.rayAngle) * rayEndPoint.rayX ;
	}


	return rayEndPoint;
}	

void drawSingleRay(float _playerX, float _playerY, struct RayEndPoint _rayEndPoint)
{
	glColor3f(0,1,0);
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex2f(_playerX, _playerY);
	glVertex2f(_rayEndPoint.rayX, _rayEndPoint.rayY);
	glEnd();
}

void drawRays(Player _player)
{
	struct RayEndPoint* rayEndPointArrayAddress = rayEndPointArray;	

	float rayAngle = _player.playerAngle-(PI*0.25);
	
	for (int i=0; i<RAY_COUNT; ++i, rayAngle+=0.02)
	{
		struct RayEndPoint rayEndPoint = findRayEndPoint(_player.playerX, _player.playerY, rayAngle);
		*rayEndPointArrayAddress++ = rayEndPoint;	// Append point to array
		drawSingleRay(_player.playerX, _player.playerY, rayEndPoint);	
	}
}

void drawSingleColumn(float _columnHeight, int position)
{
		glColor3f(0,1,0);
		glLineWidth(9);
		glBegin(GL_LINES);
		glVertex2f((WIDTH/2.0)+((RAY_COUNT-1)-position)*9, (HEIGHT/4.0)-_columnHeight/2);
		glVertex2f((WIDTH/2.0)+((RAY_COUNT-1)-position)*9, (HEIGHT/4.0)+_columnHeight/2);
		glEnd();
}

void drawColumns(Player _player)
{
	for (int i=0; i<(RAY_COUNT-1); i++)
	{
		float columnHeight = fabs( cos(rayEndPointArray[i].rayAngle) / (rayEndPointArray[i].rayX - _player.playerX) ) * 2.5e4;
		drawSingleColumn(columnHeight, i);
	}
}



