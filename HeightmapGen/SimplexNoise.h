#pragma once

#include <cstddef>
#include <cstdint>

class SimplexNoise {
public:
	// 2D Simplex Noise function
	float noise(float x, float y) const;
	
	// 2D Fractional Brownian Motion
	float fbm(size_t octaves, float x, float y) const;

	explicit SimplexNoise(
		// Parameters for Fractional Brownian Motion summation
		uint32_t seedInit = 100,
		float frequency = 0.1f,
		float amplitude = 1.0f,
		float lacunarity = 2.0f,
		float persistence = 0.5f) :
		seed(seedInit),
		freq(frequency),
		ampl(amplitude),
		lacu(lacunarity),
		pers(persistence) {
	}
		
private:
	uint32_t seed;
	float freq; // Frequency
	float ampl; // Amplitude
	float lacu; // Lacunarity
	float pers; // Persistence
};

