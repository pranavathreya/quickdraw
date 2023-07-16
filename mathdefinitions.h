#pragma once

#include <cmath>
#include <functional>

int clamp(int value, int min, int max)
{
	return (value < min) ? min : ((value > max) ? max : value);
}

float clamp(float value, float min, float max)
{
	return (value < min) ? min : ((value > max) ? max : value);
}

struct Vec2
{
	float x;
	float y;

	Vec2 operator+(float f);
	Vec2 operator+(Vec2 b);

	Vec2 operator-(float f);
	Vec2 operator-(Vec2 b);

	Vec2 operator*(float f);
	Vec2 operator*(Vec2 b);

	Vec2 operator+();
	Vec2 operator-();

	Vec2 vec_clamp(float low, float hi);

	Vec2 invert();
	Vec2 perpendicularLeft();
	Vec2 perpendicularRight();

	float magnitude();

	Vec2 correctToView();

	Vec2 componentWiseFn(std::function<float(float)> fn)
	{
		return Vec2{.x = fn(x), .y = fn(y)};
	}

	static Vec2 unit(Vec2 left)
	{
		float magnitude = left.magnitude();
		return (Vec2){left.x / magnitude, left.y / magnitude};
	}

	static Vec2 angleToUnit(float angle)
	{
		Vec2 init = {.x = std::cos(angle), .y = std::sin(angle)};

		float magnitude = init.magnitude();
		if (std::fabs(magnitude) < FLT_EPSILON)
			return init;
		init = init * (1.0 / magnitude);
		return init;
	}

	static Vec2 zero() { return (Vec2){0.0, 0.0}; }

	static Vec2 one() { return (Vec2){1.0, 1.0}; }
};

Vec2 Vec2::correctToView()
{
	return Vec2{.x = this->x, .y = -this->y};
}

Vec2 Vec2::operator+()
{
	return Vec2{.x = this->x, .y = this->y};
}

Vec2 Vec2::operator-()
{
	return Vec2{.x = -this->x, .y = -this->y};
}

Vec2 Vec2::operator+(float scalar)
{
	return Vec2{
		.x = this->x + scalar,
		.y = this->y + scalar,
	};
}

Vec2 Vec2::operator+(Vec2 b)
{
	return Vec2{
		.x = this->x + b.x,
		.y = this->y + b.y,
	};
}

Vec2 Vec2::operator-(float scalar)
{
	return Vec2{
		.x = this->x - scalar,
		.y = this->y - scalar,
	};
}

Vec2 Vec2::operator-(Vec2 b)
{
	return Vec2{
		.x = this->x - b.x,
		.y = this->y - b.y,
	};
}

Vec2 Vec2::operator*(float scalar)
{
	return Vec2{
		.x = this->x * scalar,
		.y = this->y * scalar,
	};
}

Vec2 Vec2::operator*(Vec2 b)
{
	return Vec2{
		.x = this->x * b.x,
		.y = this->y * b.y,
	};
}

Vec2 Vec2::vec_clamp(float low, float hi)
{
	return {
		.x = clamp(this->x, low, hi),
		.y = clamp(this->y, low, hi),
	};
}

Vec2 Vec2::invert()
{
	return Vec2{.x = this->y, .y = this->x};
}

Vec2 Vec2::perpendicularLeft()
{
	auto thing = invert();
	return Vec2{.x = thing.x, .y = -thing.y};
}

Vec2 Vec2::perpendicularRight()
{
	auto thing = invert();
	return Vec2{.x = -thing.x, .y = thing.y};
}

float dot(Vec2 left, Vec2 b) { return left.x * b.x + left.y * b.y; }

float Vec2::magnitude()
{
	return sqrtf((this->x * this->x) + (this->y * this->y));
}