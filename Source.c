#include <stdio.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.1415926535
#define WIDTH 1024 
#define HEIGHT 512
#define PLAYER_WAND_LENGTH 18.0
#define RAYCOUNT 53

typedef struct Player
{
	float playerX;
	float playerY;
	float playerAngle;
	float playerWandVectorX;
	float playerWandVectorY;
	float mouseRelativeX;
	float mouseRelativeY;
} Player;

Player player;

struct RayEndPoint
{
	float rayX;
	float rayY;
	float rayAngle;
};

struct RayEndPoint rayEndPointArray[RAYCOUNT];

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

void drawPlayerBody2D(Player __player)
{
	glColor3f(1,1,0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2f(__player.playerX,__player.playerY);
	glEnd();
}

void drawPlayerWand2D(Player _player)
{
	glColor3f(1,1,0);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(player.playerX,player.playerY);
	glVertex2f(player.playerX+player.playerWandVectorX,player.playerY-player.playerWandVectorY);
	glEnd();
}

void drawPlayer2D(Player _player)
{
	drawPlayerBody2D(_player);
	drawPlayerWand2D(_player);
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


void drawSingleBlock2D(int blockColumn, int blockRow, int blockPadding)
{
	glBegin(GL_QUADS);
	glVertex2i(mapBlockSize*blockColumn,                  mapBlockSize*blockRow);
	glVertex2i(mapBlockSize*blockColumn,                  mapBlockSize*(blockRow+1)-blockPadding);
	glVertex2i(mapBlockSize*(blockColumn+1)-blockPadding, mapBlockSize*(blockRow+1)-blockPadding);
	glVertex2i(mapBlockSize*(blockColumn+1)-blockPadding, mapBlockSize*blockRow);
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
	int mapBlockIndex = (mapColumns*(((int)y)/mapBlockSize)) + (((int)x)/mapBlockSize);
	return (map[mapBlockIndex]==1);
}

struct RayEndPoint findRayEndPoint( float _playerX, float _playerY, float _rayAngle)
{
	struct RayEndPoint rayEndPoint = { _playerX, _playerY, _rayAngle };
	float i = 1;
	while(!pointIsInWall(rayEndPoint.rayX, rayEndPoint.rayY))
	{
		rayEndPoint.rayX = player.playerX + cos(rayEndPoint.rayAngle)*PLAYER_WAND_LENGTH*i;
		rayEndPoint.rayY = player.playerY - sin(rayEndPoint.rayAngle)*PLAYER_WAND_LENGTH*i;
		i += 1.0e-3;
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
	for (float rayAngle=_player.playerAngle-(PI*0.25); rayAngle<_player.playerAngle+(PI*0.25); rayAngle+=0.03)
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
		glVertex2f((WIDTH/2.0)+((RAYCOUNT-1)-position)*9, (HEIGHT/4.0)-_columnHeight/2);
		glVertex2f((WIDTH/2.0)+((RAYCOUNT-1)-position)*9, (HEIGHT/4.0)+_columnHeight/2);
		glEnd();
}

void drawColumns(Player _player)
{
	for (int i=0; i<(RAYCOUNT-1); i++)
	{
		float columnHeight = fabs( cos(rayEndPointArray[i].rayAngle) / (rayEndPointArray[i].rayX - _player.playerX) ) * 2.5e4;
		drawSingleColumn(columnHeight, i);
	}
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




Player keyboardModifyPlayerOrientation(unsigned char key, Player __player)
{
	float angleUnitOfChange = 0.15;
	if (key == 'a')
		__player.playerAngle += angleUnitOfChange;	
	if (key == 'd')
		__player.playerAngle -= angleUnitOfChange;

	__player.playerWandVectorX = cos(__player.playerAngle) * PLAYER_WAND_LENGTH;
	__player.playerWandVectorY = sin(__player.playerAngle) * PLAYER_WAND_LENGTH;	

	return __player;
}

int clamp(int value, int min, int max) {
	return (value < min) ? min : ((value > max) ? max : value);
	
	if (value < min) {
		return min;
	} else if (value > max) {
		return max;
	} else {
		return value;
	}
}

Player keyboardModifyPlayerPosition(unsigned char key, Player __player)
{
	float _displacementFactor = 0.5;
	if (key == 'w')
	{
		__player.playerX += _displacementFactor * __player.playerWandVectorX;
		__player.playerY -= _displacementFactor * __player.playerWandVectorY;
	}
	if (key == 's')
	{
		__player.playerX -= _displacementFactor * __player.playerWandVectorX;
		__player.playerY += _displacementFactor * __player.playerWandVectorY;
	}

	return __player;
}

Player keyboardMovementManager(unsigned char key, Player _player)
{
	
	_player = keyboardModifyPlayerOrientation(key, _player);
	Player modifiedPlayer = keyboardModifyPlayerPosition(key, _player);

	if (pointIsInWall(modifiedPlayer.playerX, modifiedPlayer.playerY))
	{
		modifiedPlayer = _player;
	}
	else
	{
		_player = modifiedPlayer;
	}
		
	return _player;
}

void buttons(unsigned char key, int x, int y)
{
	player = keyboardMovementManager(key, player);
	glutPostRedisplay();
}

float vectorMagnitude(float x, float y) {
	return sqrtf((x * x) + (y * y));
}

Player mouseModifyPlayerOrientation(int mouseRelativeX, int mouseRelativeY, Player __player) 
{
       float vecX = (float) (mouseRelativeX - __player.playerX);
       float vecY = (float) -(mouseRelativeY - __player.playerY);

       float magnitude = vectorMagnitude(vecX, vecY);

       float unitX = vecX / magnitude;
       float unitY = vecY / magnitude;

       float angle = atan2f(unitY, unitX);

       __player.playerWandVectorX = unitX * PLAYER_WAND_LENGTH;
       __player.playerWandVectorY = unitY * PLAYER_WAND_LENGTH;
       __player.playerAngle = angle;

       return __player;
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
