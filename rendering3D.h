#include <GL/glu.h>
#include "variables.h"


void drawPlayerBody2D(Player _player)
{
	glColor3f(1, 1, 0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2f(__player.position.x, __player.position.y);
	glEnd();
}

void drawPlayerWand2D(Player _player)
{
	Vec2 wand = (Vec2::angleToUnit(__player.playerAngle) * PLAYER_WAND_LENGTH);
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

int map[]=
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,
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

			drawSingleBlock2D(currentColumn, currentRow, 1);
		}
}

bool pointIsInWall(Vec2 position)
{
	int mapBlockIndex =
		(mapColumns * (((int)position.y) / mapBlockSideLength)) + (((int)position.x) / mapBlockSideLength);
	if (mapBlockIndex > 63 || mapBlockIndex < 0)
		return 1;
	return (map[mapBlockIndex] == 1);
}

void drawSingleRay(int redOrGreen, int lineWidth, Ray ray, Player _player)
{
	redOrGreen ? glColor3f(1,0,0) : glColor3f(0,1,0);
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	glVertex2f(_player.position.x, _player.position.y);
	glVertex2f(ray.rayEndX, ray.rayEndY);
	glEnd();
}

struct Ray rayArray[RAY_COUNT];

float calculateRayLength(Ray ray, Player _player)
{
	return sqrt(pow(ray.rayEndX-_player.playerX, 2) + pow(ray.rayEndY-_player.playerY, 2));
}

// TODO: Replace Player refs with Vec2 position
Ray findRayEndPointHorizontalTraversal(Ray ray, Player _player)
{
	for (int i=0; !pointIsInWall(ray.rayEndPosition); ++i)
	{
		ray.rayEndX = (ray.directionVectorX > 0) ? (((int)(_player.position.x) >> 6)+(i+1)) << 6 : (((int)(_player.position.x) >> 6)-i << 6)-1;
		ray.rayEndY = _player.position.y + (ray.directionVectorY/fabs(ray.directionVectorY)) * 
		       	(fabs(ray.rayEndX - _player.position.x) * fabs(ray.directionVectorY / ray.directionVectorX));
	}
	ray.rayLength = calculateRayLength(ray, _player);
	ray.isHorizontalTraversing = 1;
	
	ray.color.red = 0;
       	ray.color.green = 0.72;
	ray.color.blue = 0;
	
	drawSingleRay(0, 1, ray, _player);
	fprintf(stdout, "horizontal: rayLength: %f, rayEndX: %f, rayEndY: %f\n", ray.rayLength, ray.rayEndX, ray.rayEndY);
	return ray;

}

Ray findRayEndPointVerticalTraversal(Ray ray, Player _player)
{
	for (int i=0; !pointIsInWall(ray.rayEndPosition); ++i)
	{
		ray.rayEndY = (ray.directionVectorY > 0) ? (((int)(_player.position.y) >> 6)+(i+1)) << 6 : (((int)(_player.position.y) >> 6)-i << 6)-1;
		ray.rayEndX = _player.position.x + (ray.directionVectorX/fabs(ray.directionVectorX)) *
			(fabs(ray.rayEndY - _player.position.y) * fabs(ray.directionVectorX / ray.directionVectorY));
	}
	ray.rayLength = calculateRayLength(ray, _player);
	ray.isHorizontalTraversing = 0;
		
	ray.color.red = 0;
       	ray.color.green = 0.7;
	ray.color.blue = 0;
	
	drawSingleRay(1, 3, ray, _player);
	fprintf(stdout, "vertical: rayLength: %f, rayEndX: %f, rayEndY: %f\n", ray.rayLength, ray.rayEndX, ray.rayEndY);
	return ray;

}

Ray findRayEndPoint(Ray ray, Player _player)
{
	Ray rayVerticalTraversal = findRayEndPointVerticalTraversal(ray, _player);
	Ray rayHorizontalTraversal = findRayEndPointHorizontalTraversal(ray, _player);
	
	return (rayVerticalTraversal.rayLength <= rayHorizontalTraversal.rayLength) ? rayVerticalTraversal : rayHorizontalTraversal;
}	

void drawRays(const Player _player)
{
	struct Ray* rayArrayAddress = rayArray;	

	fprintf(stdout, "rendering3D.h in: playerX: %f, playerY: %f\n", _player.position.x, _player.position.y);
	float rayAngle = _player.playerAngle+(M_PI*0.25);
	
	while (rayAngle>=_player.playerAngle-(M_PI*0.24))
	{
		Ray ray = { .directionVectorX=cos(rayAngle), .directionVectorY=-1*sin(rayAngle), 
			.rayEndPosition=(Vec2){_player.position.x, _player.position.y}, rayAngle };

		ray = findRayEndPoint(ray, _player);
	
		*rayArrayAddress++ = ray;	// Append point to array

		rayAngle-=(M_PI*0.5/RAY_COUNT);
	}
	fprintf(stdout, "rendering3D.h out: playerX: %f, playerY: %f, rayAngle: %f\n", _player.position.x, _player.position.y, rayAngle);
}

void drawSingleColumn(Column column)
{
		glColor3f(column.color.red, column.color.green, column.color.blue);
		glLineWidth(9);
		glBegin(GL_LINES);
		glVertex2f(column.columnX, (HEIGHT/2.0)-column.columnHeight/2);
		glVertex2f(column.columnX, (HEIGHT/2.0)+column.columnHeight/2);
		glEnd();
}

void drawColumns(const Player _player)
{
	int displayWidth = (WIDTH / 2) + 46;
	int columnWidth = (displayWidth / RAY_COUNT);
	for (int i=0; i<RAY_COUNT; i++)
	{
		Column column;
		column.columnWidth = columnWidth;
		column.columnHeight = (1/fabs(rayArray[i].rayLength*cos(rayArray[i].rayAngle-_player.playerAngle))) * 2.5e4;
		column.columnHeight = column.columnHeight > HEIGHT ? HEIGHT : column.columnHeight;
		column.columnX = (WIDTH * 0.5) + (i * column.columnWidth + (column.columnWidth * 0.5));
		column.color = rayArray[i].color;
		drawSingleColumn(column);
	}
}

