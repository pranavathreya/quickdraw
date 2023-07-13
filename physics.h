#include <GL/glut.h> 

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




