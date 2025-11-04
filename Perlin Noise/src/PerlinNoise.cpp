#include "PerlinNoise.h"
#include <cmath>
#include <random>

const float PI = 3.14159;

static const int P_ARRAY_SIZE = 256;
static int p[P_ARRAY_SIZE * 2]; // 512 de tamaño

static int permutation[] = { 151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225,
							140,  36, 103,  30,  69, 142,   8,  99,  37, 240,  21,  10,  23, 190,   6, 148,
							247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
							57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
							74, 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
							60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
							65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
							200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
							 52, 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
							207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
							119, 248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9,
							129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
							218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
							 81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
							184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
							222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180 };

PerlinNoise::PerlinNoise() {
	// Copiar y duplicar la tabla, si no se ha hecho ya.
	// Usamos una bandera estática para inicializarla solo una vez.
	static bool initialized = false;
	if (!initialized) {
		for (int i = 0; i < P_ARRAY_SIZE; ++i) {
			p[i] = permutation[i];
			p[i + P_ARRAY_SIZE] = permutation[i];
		}
		initialized = true;
	}
}

static const Vector2 gradients[] = {
	{ 1.0f,  1.0f}, { -1.0f,  1.0f}, { 1.0f, -1.0f}, { -1.0f, -1.0f},
	{ 1.0f,  0.0f}, { -1.0f,  0.0f}, { 0.0f,  1.0f}, { 0.0f, -1.0f}
};

// c ----- d
// |       |
// |       |
// a ----- b
float PerlinNoise::generateNoise(float x, float y) {

	Vector2 point = Vector2(x, y);

	int x0 = (int)floor(x);
	int x1 = x0 + 1;
	int y0 = (int)floor(y);
	int y1 = y0 + 1;

	Vector2 a = Vector2(x0, y0);
	Vector2 b = Vector2(x1, y0);
	Vector2 c = Vector2(x0, y1);
	Vector2 d = Vector2(x1, y1);

	Vector2 gA = randomGradient(hash(x0, y0));
	Vector2 gB = randomGradient(hash(x1, y0));
	Vector2 gC = randomGradient(hash(x0, y1));
	Vector2 gD = randomGradient(hash(x1, y1));

	Vector2 offsetA = offsetVector(a, point);
	Vector2 offsetB = offsetVector(b, point);
	Vector2 offsetC = offsetVector(c, point);
	Vector2 offsetD = offsetVector(d, point);

	float dotA = dotGridGradient(gA, offsetA);
	float dotB = dotGridGradient(gB, offsetB);
	float dotC = dotGridGradient(gC, offsetC);
	float dotD = dotGridGradient(gD, offsetD);

	float u = x - floor(x);
	float v = y - floor(y);
	float U = fade(u);
	float V = fade(v);

	float i0 = lerp(dotC, dotD, U);
	float i1 = lerp(dotA, dotB, U);
	float value = lerp(i1, i0, V);

	return std::clamp(value, -1.0f, 1.0f);
}

float PerlinNoise::fractalNoise(float x, float y, int octaves, float persistence, float lacunarity, float amp, float frec) {
	float total = 0.0f;
	float amplitude = amp;
	float frequency = frec;
	float maxValue = 0.0f; // para normalizar entre -1 y 1

	for (int i = 0; i < octaves; i++) {
		total += generateNoise(x * frequency, y * frequency) * amplitude;

		maxValue += amplitude;
		amplitude *= persistence;  // reduce la amplitud en cada octava
		frequency *= lacunarity;   // aumenta la frecuencia en cada octava
	}

	return total / maxValue; // normalizamos
}

uint8_t PerlinNoise::hash(int x, int y) {
	// La operación & 255 se asegura de que la coordenada (x o y) 
	// caiga en el rango [0, 255] antes de acceder a la tabla.
	// El patrón de hashing: p[p[x] + y]
	return p[p[x & 255] + (y & 255)];
}

Vector2 PerlinNoise::randomGradient(uint8_t index) {
	return gradients[index % 8];
}

Vector2 PerlinNoise::offsetVector(Vector2 pA, Vector2 pB) {
	return { pB.x - pA.x, pB.y - pA.y };
}

float PerlinNoise::dotGridGradient(Vector2 gr, Vector2 ofs) {
	return gr.x * ofs.x + gr.y * ofs.y;
}

float PerlinNoise::lerp(float a, float b, float t) {
	return a + t * (b - a);
}

float PerlinNoise::fade(float t) {
	return t * t * (3 - 2 * t);
}
