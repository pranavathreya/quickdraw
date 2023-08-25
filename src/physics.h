#pragma once

#include <math.h>
#include "variables.h"

int mapColumns = 8, mapRows = 8, mapBlockSideLength = 64;

int map[]=
{
	1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,1,
	1,0,1,0,1,0,1,1,
	1,0,0,0,0,0,0,1,
	1,0,1,0,0,0,1,1,
	1,0,0,0,0,0,0,1,
	1,0,1,0,1,0,1,1,
	1,1,1,1,1,1,1,1,
};

uint8_t pointIsInWall(Vec2 position)
{
	int mapBlockIndex =
		(mapColumns * (((int)position.y) / mapBlockSideLength)) + (((int)position.x) / mapBlockSideLength);
	if (mapBlockIndex > 63 || mapBlockIndex < 0)
		return 1;
	return (map[mapBlockIndex] == 1);
}

float calculateNewOrientation(InputState* istate, float oldAngle, float deltaTime)
{

	float newAngle = oldAngle;
	if (istate->mouseDX == 0 && !istate->left && !istate->right) {
		; // do nothing.
	} else if (istate->mouseDX != 0) {
		newAngle -= ((float)istate->mouseDX * 0.005);
	} else if (istate->left && !istate->right) {
		newAngle += (deltaTime * 0.04);
	} else if (istate->right && !istate->left) {
		newAngle -= (deltaTime * 0.04);
	}

	return fmod(newAngle, 2*M_PI);
}

Vec2 accelerateBasedOnInput(InputState* istate, float angle)
{
	float accelerationFactor = 0.1f;
	float decelerationFactor = 2.0f;

	Vec2 accel =  vec2_zero();
	Vec2 intermediate1 = vec2_angle_to_unit(angle);
	Vec2 direction = vec2_correct_to_view(&intermediate1);

	uint8_t anythingPressed = istate->forward || istate->left || istate->back || istate->right;

	if (istate->forward)
	{
		Vec2 thing = vec2_mul_scalar(&direction, accelerationFactor);
		// Speed up towards the direction of the player.
		accel = vec2_plus_vec2(&accel, &thing);
	}

	if (istate->back)
	{
		Vec2 thing = vec2_mul_scalar(&direction, -accelerationFactor);
		// Speed up away from the direction of the player.
		accel = vec2_plus_vec2(&accel, &thing);
	}

	if (istate->strafeLeft)
	{
		Vec2 thing1 = vec2_mul_scalar(&direction, accelerationFactor);
		Vec2 thing2 = vec2_perpendicular_left(&thing1);
		// Speed up perpendicular to the direction of the player, to the left.
		// Formula fro vector with components (x, y) is (y, -x).
		accel = vec2_plus_vec2(&accel, &thing2);
	}

	if (istate->strafeRight)
	{
		Vec2 thing1 = vec2_mul_scalar(&direction, accelerationFactor);
		Vec2 thing2 = vec2_perpendicular_right(&thing1);
		// Formula fro vector with components (x, y) is (-y, x)
		// for the perpendicular in the opposite direction.
		accel = vec2_plus_vec2(&accel, &thing2);
	}

	return accel;
}

void updatePhysics(Player* player, InputState* istate, double milliDeltaTime)
{
	float deltaTime = milliDeltaTime * 0.1;
	float newAngle = calculateNewOrientation(istate, player->playerAngle, deltaTime);

	Vec2 acceleration =
		accelerateBasedOnInput(istate, newAngle);

	Vec2 newSpeed =
		vec2_plus_vec2(&player->speed, &acceleration);
	if (vec2_magnitude(&newSpeed) > 2.0) {
		newSpeed = vec2_mul_scalar(&newSpeed, (2.0 / vec2_magnitude(&newSpeed)));
	}

	Vec2 intermediate1 = vec2_mul_scalar(&player->speed, deltaTime);
	Vec2 newPosition = vec2_plus_vec2(&player->position, &intermediate1);

	if (pointIsInWall(newPosition))
	{
		Vec2 wallNormal = 
		newSpeed = vec2_zero();
		newPosition = player->position;
	}

	if (input_state_no_buttons(istate)) {
		Vec2 intermediate2 = vec2_mul_scalar(&newSpeed, 0.05 * deltaTime);
		newSpeed = vec2_minus_vec2(&newSpeed, &intermediate2);
	}
	if (vec2_magnitude(&newSpeed) < FLT_EPSILON) newSpeed = vec2_zero();

	printf("new data:\n");
	printf("speed: x - %4.4f y - %4.4f\n", newSpeed.x, newSpeed.y);
	printf("position: x - %4.4f y - %4.4f\n", newPosition.x, newPosition.y);
	printf("angle - %f\n", newAngle);

	player->speed = newSpeed;
	player->position = newPosition;
	player->playerAngle = newAngle;
}
