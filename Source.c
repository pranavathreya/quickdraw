#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#define PI 3.1415926535
#define WIDTH 1024 
#define HEIGHT 512
#define PLAYERWANDLENGTH 18.0
#define RAYCOUNT 53

struct Player
{
	float playerX;
	float playerY;
	float playerAngle;
	float playerUnitVectorX;
	float playerUnitVectorY;
};

struct Player player;

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
	player.playerUnitVectorX = cos(player.playerAngle) *  PLAYERWANDLENGTH;
	player.playerUnitVectorY = sin(player.playerAngle) * PLAYERWANDLENGTH;
}

void drawPlayerBody2D(struct Player __player)
{
	glColor3f(1,1,0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2f(__player.playerX,__player.playerY);
	glEnd();
}

void drawPlayerWand(struct Player _player)
{
	glColor3f(1,1,0);
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2f(player.playerX,player.playerY);
	glVertex2f(player.playerX+player.playerUnitVectorX,player.playerY-player.playerUnitVectorY);
	glEnd();
}

void drawPlayer2D(struct Player _player)
{
	drawPlayerBody2D(_player);
	drawPlayerWand(_player);
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


void drawSingleBlock(int blockColumn, int blockRow, int blockPadding)
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

			drawSingleBlock(currentColumn, currentRow, 2);
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
		rayEndPoint.rayX = player.playerX + cos(rayEndPoint.rayAngle)*PLAYERWANDLENGTH*i;
		rayEndPoint.rayY = player.playerY - sin(rayEndPoint.rayAngle)*PLAYERWANDLENGTH*i;
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

void drawRays(struct Player _player)
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

void drawColumns(struct Player _player)
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




struct Player modifyPlayerOrientation(unsigned char key, struct Player __player)
{
	float angleUnitOfChange = 0.15;
	if (key == 'a')
		__player.playerAngle +=  angleUnitOfChange;	
	if (key == 'd')
		__player.playerAngle -= angleUnitOfChange;

	__player.playerUnitVectorX = cos(__player.playerAngle) * PLAYERWANDLENGTH;
	__player.playerUnitVectorY = sin(__player.playerAngle) * PLAYERWANDLENGTH;	

	return __player;
}

struct Player modifyPlayerPosition(unsigned char key, struct Player __player)
{
	float _displacementFactor = 0.5;
	if (key == 'w')
	{
		__player.playerX += _displacementFactor * __player.playerUnitVectorX;
		__player.playerY -= _displacementFactor * __player.playerUnitVectorY;
	}
	if (key == 's')
	{
		__player.playerX -= _displacementFactor * __player.playerUnitVectorX;
		__player.playerY += _displacementFactor * __player.playerUnitVectorY;
	}

	return __player;
}

struct Player modifyPlayerOrOrientation(unsigned char key, struct Player _player)
{
	
	_player = modifyPlayerOrientation(key, _player);
	struct Player modifiedPlayer = modifyPlayerPosition(key, _player);

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
	player = modifyPlayerOrOrientation(key, player);
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
