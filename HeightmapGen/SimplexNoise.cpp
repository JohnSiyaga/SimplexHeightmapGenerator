#include "stdafx.h"
#include "SimplexNoise.h"
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <iostream>

// Permutation set of values [0-255]
static const uint8_t permutation[] = {
	151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36,
	103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0,
	26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56,
	87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
	77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55,
	46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132,
	187, 208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109,
	198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126,
	255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183,
	170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43,
	172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112,
	104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162,
	241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106,
	157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205,
	93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

// 8-bit hash function
static inline uint8_t hash(int32_t i, uint32_t seed) {
	return permutation[static_cast<uint8_t>(i)] ^ permutation[static_cast<uint8_t>(i >> 8)] ^ static_cast<uint8_t>(seed);
}

// Bitwise gradient dot product function
static float grad(int32_t hash, float x, float y) {
	const int32_t h = hash & 0x3F; // Convert 3 bit hash to 8 gradient directions
	const float u = h & 4 ? x : y; 
	const float v = h & 4 ? y : x; 
	return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v); // Compute Dot Product
}

// C++ Implementation of Stefan Gustavson's fastfloor method
static inline int32_t fastfloor(float x) {
	int32_t i = static_cast<int32_t>(x);
	return (x < i) ? (i - 1) : (i); // Find largest integer less than x
}

// C++ Implementation of Stefan Gustavson's 2D Simplex Noise algorithm
float SimplexNoise::noise(float x, float y) const {
	// Contributions from 3 corners (2D)
	float n0, n1, n2;

	// 2D Skew Factors
	static const float F2 = 0.3660254038f; // = 0.5 * (sqrt(3.0) - 1)
	static const float G2 = 0.2113248654f; // = (3.0 - sqrt(3.0)) / 6

	// Skew Input Space to Simplex plane (i,j)
	float s = (x + y) * F2;
	const int32_t i = fastfloor(x + s); // Skew x
	const int32_t j = fastfloor(y + s); // Skew y

	// Unskew Input Space back to normal (x,y) space
	const float t = static_cast<float>(i + j) * G2;
	const float X0 = i - t; // Unskew x
	const float Y0 = j - t; // Unskew y
	const float x0 = x - X0; // x distance from origin
	const float y0 = y - Y0; // y distance from origin

	// Determine which Simplex shape (x,y) occupies.
	// This is the 2D case, so it will be an equilateral triangle
	int32_t i1, j1; // Offsets for middle corner of simplex in the Simplex plane (i,j)
	if (x0 > y0) {
		// Lower Triangle --> XY Order: (0,0)->(1,0)->(1,1)
		i1 = 1;
		j1 = 0;
	}
	else {
		// Upper Triangle --> YX Order: (0,0)->(0,1)->(1,1)
		i1 = 0;
		j1 = 1;
	}

	// A step of (0,1) in the Simplex plane (i,j) = A step of (1-G2, -G2) in the (x,y) plane  
	// A step of (1,0) in the Simplex plane (i,j) = A step of (-G2, 1-G2) in the (x,y) plane  
	const float x1 = x0 - i1 + G2; // Unskewed x offset for middle corner
	const float y1 = y0 - j1 + G2; // Unskewed y offset for middle corner
	const float x2 = x0 - 1.0f + 2.0f * G2; // Unskewed x offset for last corner
	const float y2 = y0 - 1.0f + 2.0f * G2; // Unskewed y offset for last corner

	// Find hashed gradient indicies of 3 corners
	
	const int gi0 = hash(i + hash(j, seed), seed >> 8);
	const int gi1 = hash(i + i1 + hash(j + j1, seed), seed >> 8);
	const int gi2 = hash(i + 1 + hash(j + 1, seed), seed >> 8);

	// Corner contributions //
	
	// Corner 1
	float t0 = 0.5f - x0 * x0 - y0 * y0;
	if (t0 < 0.0f) {
		n0 = 0.0f;
	} 
	else {
		t0 *= t0;
		n0 = t0 * t0 * grad(gi0, x0, y0); 
	}

	// Corner 2
	float t1 = 0.5f - x1 * x1 - y1 * y1;
	if (t1 < 0.0f) {
		n1 = 0.0f;
	}
	else {
		t1 *= t1;
		n1 = t1 * t1 * grad(gi1, x1, y1); 
	}

	// Corner 3
	float t2 = 0.5f - x2 * x2 - y2 * y2;
	if (t2 < 0.0f) {
		n2 = 0.0f;
	}
	else {
		t2 *= t2;
		n2 = t2 * t2 * grad(gi2, x2, y2); 
	}

	// Final Noise Value Calculation scaled to return values in [-1, 1]
	float scaled = 45.23065f * (n0 + n1 + n2);

	// Linear interpolation from [-1, 1] --> [0, 255]
	float brightness = fastfloor(255*((scaled + 1) / 2));

	return brightness; 
}

// Fractional Brownian Motion summation
float SimplexNoise::fbm(size_t octaves, float x, float y) const {
	float output = 0.0f;
	float denominator = 0.0f;
	float frequency = freq;
	float amplitude = ampl;

	for (size_t i = 0; i < octaves; i++) {
		output += amplitude * noise(x * frequency, y * frequency);
		denominator += amplitude;

		frequency *= lacu;
		amplitude *= pers;
	}

	return (output / denominator);
}
