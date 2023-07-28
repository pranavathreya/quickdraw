#pragma once

#include <iostream>

#include "variables.h"

float calculateNewOrientation(InputState istate, float oldAngle, float deltaTime)
{

	float newAngle = oldAngle;
	if (istate.mouseDX == 0 && !istate.left && !istate.right) {
		; // do nothing.
	} else if (istate.mouseDX != 0) {
		newAngle -= ((float)istate.mouseDX * 0.005);
	} else if (istate.left && !istate.right) {
		newAngle += (deltaTime * 0.04);
	} else if (istate.right && !istate.left) {
		newAngle -= (deltaTime * 0.04);
	}

	newAngle = fmodf32(newAngle, (float)(2 * M_PI));

	return newAngle;
}

Vec2 accelerateBasedOnInput(InputState istate, float angle)
{
	float accelerationFactor = 0.1f;
	float decelerationFactor = 2.0f;

	Vec2 accel = Vec2::zero();
	Vec2 direction = Vec2::angleToUnit(angle);

	bool anythingPressed = istate.forward || istate.left || istate.back || istate.right;

	if (istate.forward)
	{
		// Speed up towards the direction of the player.
		accel = accel + (direction * accelerationFactor);
	}

	if (istate.back)
	{
		// Speed up away from the direction of the player.
		accel = accel + (direction * -accelerationFactor);
	}

	if (istate.strafeLeft)
	{
		// Speed up perpendicular to the direction of the player, to the left.
		// Formula fro vector with components (x, y) is (y, -x).
		accel = accel + (direction * accelerationFactor).perpendicularLeft();
	}

	if (istate.strafeRight)
	{
		// Formula fro vector with components (x, y) is (-y, x)
		// for the perpendicular in the opposite direction.
		accel = accel + (direction * accelerationFactor).perpendicularRight();
	}

	return accel;
}

Player inputManager(Player player, InputState istate, double milliDeltaTime)
{
	float deltaTime = milliDeltaTime * 0.1;
	float newAngle = calculateNewOrientation(istate, player.playerAngle, deltaTime);

	Vec2 acceleration =
		accelerateBasedOnInput(istate, newAngle);

	Vec2 newSpeed =
		(player.speed + acceleration);
	if (newSpeed.magnitude() > 2.0) {
		newSpeed = newSpeed * (2.0 / newSpeed.magnitude());
	}

	Vec2 newPosition = player.position + (player.speed * deltaTime);

	if (pointIsInWall(newPosition))
	{
		newSpeed = Vec2::zero();
		newPosition = player.position;
	}

	if (istate.noButtons()) {
		newSpeed = newSpeed - newSpeed * 0.05 * deltaTime;
	}
	if (newSpeed.magnitude() < FLT_EPSILON) newSpeed = Vec2::zero();

	std::cout << "new data:\n"
			  << "speed: x - " << newSpeed.x << " y - " << newSpeed.y 					<< std::endl
			  << "position: x - " << newPosition.x << " y - " << newPosition.y			<< std::endl
			  << "angle - " << newAngle													<< std::endl;

	return Player {
		.speed = newSpeed,
		.position = newPosition,
		.playerAngle = newAngle,
	};
}
