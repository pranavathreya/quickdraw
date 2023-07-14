#define PI 3.1415926535
#define WIDTH 1024 
#define HEIGHT 512
#define PLAYER_WAND_LENGTH 18.0
#define RAY_COUNT 93

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

