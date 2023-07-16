#include <GL/glu.h>
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
	glColor3f(1, 1, 0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2f(__player.position.x, __player.position.y);
	glEnd();
}

void drawPlayerWand2D(Player __player)
{
	Vec2 wand = (__player.direction * PLAYER_WAND_LENGTH).correctToView();
	glColor3f(1, 1, 0);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(__player.position.x, __player.position.y);
	glVertex2f(__player.position.x + wand.x, __player.position.y + wand.y);
	glEnd();
}

void drawPlayer2D(Player _player)
{
	drawPlayerBody2D(_player);
	drawPlayerWand2D(_player);
}

int mapColumns = 8, mapRows = 8, mapBlockSideLength = 64;

int map[] =
	{
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		0,
		1,
		0,
		0,
		0,
		0,
		1,
		1,
		0,
		1,
		0,
		0,
		0,
		0,
		1,
		1,
		0,
		1,
		0,
		0,
		0,
		0,
		1,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		1,
		0,
		0,
		0,
		0,
		1,
		0,
		1,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
};

void drawSingleBlock2D(int currentMapColumn, int currentMapRow, int blockPadding)
{
	glBegin(GL_QUADS);
	glVertex2i(mapBlockSideLength * currentMapColumn, mapBlockSideLength * currentMapRow);
	glVertex2i(mapBlockSideLength * currentMapColumn, mapBlockSideLength * (currentMapRow + 1) - blockPadding);
	glVertex2i(mapBlockSideLength * (currentMapColumn + 1) - blockPadding, mapBlockSideLength * (currentMapRow + 1) - blockPadding);
	glVertex2i(mapBlockSideLength * (currentMapColumn + 1) - blockPadding, mapBlockSideLength * currentMapRow);
	glEnd();
}

void drawMap2D()
{
	for (int currentColumn = 0; currentColumn < mapColumns; currentColumn++)
		for (int currentRow = 0; currentRow < mapRows; currentRow++)
		{
			if (map[(mapColumns * currentRow) + currentColumn] == 1)
				glColor3f(1, 1, 1);
			else
				glColor3f(0, 0, 0);

			drawSingleBlock2D(currentColumn, currentRow, 2);
		}
}

bool pointIsInWall(float x, float y)
{

	int mapBlockIndex = (mapColumns * (((int)y) / mapBlockSideLength)) + (((int)x) / mapBlockSideLength);
	// Bail out if we're looking at left point that's out of bounds.
	if (mapBlockIndex > 63 || mapBlockIndex < 0)
		return true;
	bool mapPoint = (map[mapBlockIndex] == 1);
	return mapPoint;
}

bool pointIsInWall(Vec2 position)
{
	int mapBlockIndex =
		(mapColumns * (((int)position.y) / mapBlockSideLength)) + (((int)position.x) / mapBlockSideLength);
	if (mapBlockIndex > 63 || mapBlockIndex < 0)
		return 1;
	return (map[mapBlockIndex] == 1);
}

struct RayEndPoint findRayEndPoint(Vec2 playerPosition, float rayAngle)
{
	Vec2 currentPosition = Vec2(playerPosition);

	for (int i = 0; !pointIsInWall(currentPosition); ++i)
	{
		currentPosition.x = (float)((((int)currentPosition.x) / mapBlockSideLength) + i) * mapBlockSideLength;
		currentPosition.y = tan(rayAngle) * currentPosition.x;
	}

	return RayEndPoint{.rayX = currentPosition.x, .rayY = currentPosition.y, .rayAngle = rayAngle};
}

void drawSingleRay(Vec2 position, struct RayEndPoint _rayEndPoint)
{
	glColor3f(0, 1, 0);
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex2f(position.x, position.y);
	glVertex2f(_rayEndPoint.rayX, _rayEndPoint.rayY);
	glEnd();
}

void drawRays(Player _player)
{
	struct RayEndPoint *rayEndPointArrayAddress = rayEndPointArray;

	float rayAngle = _player.playerAngle - (M_PI * 0.25);

	for (int i = 0; i < RAY_COUNT; ++i, rayAngle += 0.02)
	{
		struct RayEndPoint rayEndPoint = findRayEndPoint(_player.position, rayAngle);
		*rayEndPointArrayAddress++ = rayEndPoint; // Append point to array
		drawSingleRay(_player.position, rayEndPoint);
	}
}

void drawSingleColumn(float _columnHeight, int position)
{
	glColor3f(0, 1, 0);
	glLineWidth(9);
	glBegin(GL_LINES);
	glVertex2f((WIDTH / 2.0) + ((RAY_COUNT - 1) - position) * 9, (HEIGHT / 4.0) - _columnHeight / 2);
	glVertex2f((WIDTH / 2.0) + ((RAY_COUNT - 1) - position) * 9, (HEIGHT / 4.0) + _columnHeight / 2);
	glEnd();
}

void drawColumns(Player _player)
{
	for (int i = 0; i < (RAY_COUNT - 1); i++)
	{
		float columnHeight = fabs(cos(rayEndPointArray[i].rayAngle) / (rayEndPointArray[i].rayX - _player.position.x)) * 2.5e4;
		drawSingleColumn(columnHeight, i);
	}
}
