#pragma once

#include <math.h>

int int_clamp(int value, int min, int max)
{
	return (value < min) ? min : ((value > max) ? max : value);
}

float float_clamp(float value, float min, float max)
{
	return (value < min) ? min : ((value > max) ? max : value);
}

typedef struct
{
	float x;
	float y;
} Vec2;

Vec2 vec2_mul_scalar(Vec2 *self, float scalar)
{
	return (Vec2) {
		.x = self->x * scalar,
		.y = self->y * scalar,
	};
}


float vec2_magnitude(Vec2 *self)
{
	return sqrtf((self->x * self->x) + (self->y * self->y));
}

Vec2 vec2_apply_component_wise(Vec2 *self, float fn(float))
{
	return (Vec2) {.x = fn(self->x), .y = fn(self->y)};
}

Vec2 vec2_x_component(Vec2 *self)
{
	return (Vec2) {.x = self->x, .y = 0.0f};
}

Vec2 vec2_y_component(Vec2 *self)
{
	return (Vec2) {.x = 0.0f, .y = self->y};
}

Vec2 x_axis(float magnitude)
{
	return (Vec2) {.x = magnitude, .y = 0.0f};
}

Vec2 y_axis(float magnitude)
{
	return (Vec2) {.y = magnitude, .x = 0.0f};
}

Vec2 vec2_to_unit(Vec2 *left)
{
	float magnitude = vec2_magnitude(left);
	return (Vec2) {left->x / magnitude, left->y / magnitude};
}

Vec2 vec2_angle_to_unit(float angle)
{
	Vec2 init = {.x = cos(angle), .y = sin(angle)};

	float magnitude = vec2_magnitude(&init);
	if (fabs(magnitude) < __FLT_EPSILON__)
		return init;
	init = vec2_mul_scalar(&init, (1.0 / magnitude));
	return init;
}

Vec2 vec2_zero() { return (Vec2){0.0, 0.0}; }

Vec2 vec2_one() { return (Vec2){1.0, 1.0}; }

Vec2 vec2_direction(Vec2 *self)
{
	float mag = vec2_magnitude(self);
	return (Vec2) {.x = self->x / mag, .y = self->y / mag};
}

float vec2_angle(Vec2 *self)
{
	return atan2(self->y, self->x);
}

Vec2 vec2_rotate(Vec2 *self, float angle)
{
	float currMag = vec2_magnitude(self);

	float cosTheta = self->x / currMag;
	float sinTheta = self->y / currMag;

	float cosAlpha = cos(angle);
	float sinAlpha = sin(angle);

	return (Vec2) {.x = ((cosTheta * cosAlpha) - (sinTheta * sinAlpha)) * currMag, .y = ((sinTheta * cosAlpha) + (cosTheta * sinAlpha)) * currMag};
}

/**
 * @brief Applies a transformation to make this vector align with the top-down coordinate system the view uses.
 *
 * @return Vec2
 */
Vec2 vec2_correct_to_view(Vec2 *self)
{
	return (Vec2) {.x = self->x, .y = -self->y};
}

Vec2 vec2_unary_plus(Vec2 *self)
{
	return (Vec2) {.x = self->x, .y = self->y};
}

Vec2 vec2_unary_minus(Vec2 *self)
{
	return (Vec2) {.x = -self->x, .y = -self->y};
}

Vec2 vec2_plus_scalar(Vec2 *self, float scalar)
{
	return (Vec2) {
		.x = self->x + scalar,
		.y = self->y + scalar,
	};
}

Vec2 vec2_plus_vec2(Vec2 *self, Vec2 *b)
{
	return (Vec2) {
		.x = self->x + b->x,
		.y = self->y + b->y,
	};
}

Vec2 vec2_minus_scalar(Vec2 *self, float scalar)
{
	return (Vec2) {
		.x = self->x - scalar,
		.y = self->y - scalar,
	};
}

Vec2 vec2_minus_vec2(Vec2 *self, Vec2 *b)
{
	return (Vec2) {
		.x = self->x - b->x,
		.y = self->y - b->y,
	};
}



Vec2 vec2_componentwise_mul_vector(Vec2 *self, Vec2 *b)
{
	return (Vec2){
		.x = self->x * b->x,
		.y = self->y * b->y,
	};
}

Vec2 vec_clamp(Vec2 *self, float low, float hi)
{
	return (Vec2) {
		.x = float_clamp(self->x, low, hi),
		.y = float_clamp(self->y, low, hi),
	};
}

Vec2 vec2_invert(Vec2 *self)
{
	return (Vec2) {.x = self->y, .y = self->x};
}

Vec2 vec2_perpendicular_left(Vec2 *self)
{
	Vec2 thing = vec2_invert(self);
	return (Vec2) {.x = thing.x, .y = -thing.y};
}

Vec2 vec2_perpendicular_right(Vec2 *self)
{
	Vec2 thing = vec2_invert(self);
	return (Vec2) {.x = -thing.x, .y = thing.y};
}

float dot(Vec2 *left, Vec2 *b) { return left->x * b->x + left->y * b->y; }