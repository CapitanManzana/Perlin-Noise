#pragma once
#include <cstdint>
struct Vector2
{
	float x;
	float y;

	Vector2(float x, float y) {
		Vector2::x = x;
		Vector2::y = y;
	}
};

class PerlinNoise {
private:
	uint8_t hash(int x, int y);
	Vector2 randomGradient(uint8_t index);
	Vector2 offsetVector(Vector2 pA, Vector2 pB);
	float dotGridGradient(Vector2 gradient, Vector2 offset);
	float lerp(float a, float b, float t);
	float fade(float t);
public:
	PerlinNoise();
	float generateNoise(float x, float y);
	float fractalNoise(float x, float y, int octaves, float persistence, float lacunarity, float amp, float frec);
};

