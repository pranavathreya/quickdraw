#include <GL/glut.h>
#include "variables.h"


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

			drawSingleBlock2D(currentColumn, currentRow, 1);
		}
}

int pointIsInWall(float x, float y)
{
	int mapBlockIndex = (mapColumns*(((int)y)/mapBlockSideLength)) + (((int)x)/mapBlockSideLength);
	
	if (mapBlockIndex > 63 || mapBlockIndex < 0)
		return 1;

	return (map[mapBlockIndex]==1);
}

typedef struct Ray
{
	float directionVectorX;
	float directionVectorY;
	float rayEndX;
	float rayEndY;
	float rayAngle;
	float rayLength;
	int isHorizontalTraversing;
	int isGreen;
} Ray;

void drawSingleRay(int redOrGreen, int lineWidth, Ray __ray, Player ___player)
{
	redOrGreen ? glColor3f(1,0,0) : glColor3f(0,1,0);
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	glVertex2f(___player.playerX, ___player.playerY);
	glVertex2f(__ray.rayEndX, __ray.rayEndY);
	glEnd();
}

struct Ray rayArray[RAY_COUNT];

float calculateRayLength(Ray ___ray, Player ____player)
{
	return sqrt(pow(___ray.rayEndX-____player.playerX, 2) + pow(___ray.rayEndY-____player.playerY, 2));
}

Ray findRayEndPointHorizontalTraversal(Ray __ray, Player ___player)
{
	for (int i=0; !pointIsInWall(__ray.rayEndX, __ray.rayEndY); ++i)
	{
		__ray.rayEndX = (__ray.directionVectorX > 0) ? (((int)(___player.playerX) >> 6)+(i+1)) << 6 : (((int)(___player.playerX) >> 6)-i << 6)-1;
		__ray.rayEndY = ___player.playerY + (__ray.directionVectorY/fabs(__ray.directionVectorY)) * 
		       	(fabs(__ray.rayEndX - ___player.playerX) * fabs(__ray.directionVectorY / __ray.directionVectorX));
	}
	__ray.rayLength = calculateRayLength(__ray, ___player);
	__ray.isHorizontalTraversing = 1;
	__ray.isGreen = 1;
	drawSingleRay(0, 1, __ray, ___player);
	return __ray;

}

Ray findRayEndPointVerticalTraversal(Ray __ray, Player ___player)
{
	for (int i=0; !pointIsInWall(__ray.rayEndX, __ray.rayEndY); ++i)
	{
		__ray.rayEndY = (__ray.directionVectorY > 0) ? (((int)(___player.playerY) >> 6)+(i+1)) << 6 : (((int)(___player.playerY) >> 6)-i << 6)-1;
		__ray.rayEndX = ___player.playerX + (__ray.directionVectorX/fabs(__ray.directionVectorX)) *
			(fabs(__ray.rayEndY - ___player.playerY) * fabs(__ray.directionVectorX / __ray.directionVectorY));
	}
	__ray.rayLength = calculateRayLength(__ray, ___player);
	__ray.isHorizontalTraversing = 0;
	__ray.isGreen = 0;
	drawSingleRay(1, 3, __ray, ___player);
	return __ray;

}

Ray findRayEndPoint(Ray _ray, Player __player)
{
	Ray _rayVerticalTraversal = findRayEndPointVerticalTraversal(_ray, __player);
	Ray _rayHorizontalTraversal = findRayEndPointHorizontalTraversal(_ray, __player);
	
	return (_rayVerticalTraversal.rayLength <= _rayHorizontalTraversal.rayLength) ? _rayVerticalTraversal : _rayHorizontalTraversal;
}	

void drawRays(Player _player)
{
	struct Ray* rayArrayAddress = rayArray;	

	float rayAngle = _player.playerAngle-(PI*0.25);
	
	for (int i=0; i<RAY_COUNT; ++i, rayAngle+=0.02)
	{
		Ray ray = { cos(rayAngle), -1*sin(rayAngle), _player.playerX, _player.playerY, rayAngle };

		ray = findRayEndPoint(ray, _player);
		*rayArrayAddress++ = ray;	// Append point to array
	}
}

void drawSingleColumn(int redOrGreen, float _columnHeight, int position)
{
		redOrGreen ? glColor3f(0,0.7,0) : glColor3f(0,0.8,0);
		glLineWidth(9);
		glBegin(GL_LINES);
		glVertex2f((WIDTH/2.0)+((RAY_COUNT-1)-position)*7, (HEIGHT/2.0)-_columnHeight/2);
		glVertex2f((WIDTH/2.0)+((RAY_COUNT-1)-position)*7, (HEIGHT/2.0)+_columnHeight/2);
		glEnd();
}

void drawColumns(Player _player)
{
	for (int i=0; i<(RAY_COUNT-1); i++)
	{
		float columnHeight = rayArray[i].isHorizontalTraversing ? (1/fabs(rayArray[i].rayEndX - _player.playerX)) * 2.5e4 : (1/rayArray[i].rayLength) * 2.5e4;
		fprintf(stdout, "columnHeight: %f\n", columnHeight);
		columnHeight = columnHeight > HEIGHT ? HEIGHT : columnHeight;
		drawSingleColumn(!rayArray[i].isGreen, columnHeight, i);
	}
}



