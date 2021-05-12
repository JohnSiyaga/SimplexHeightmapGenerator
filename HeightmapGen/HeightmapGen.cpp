#include "stdafx.h"
#include "SimplexNoise.h"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <cstdint>

using namespace cv;
using namespace std;

void invert(Mat_<uchar> heightmap, int size) {
	for (int j = 0; j < size; j++)
		for (int k = 0; k < size; k++)
			heightmap(j, k) = 255 - heightmap(j, k);
}

void clamp(int brightness, Mat_<uchar> heightmap, int hmSize) {
	for (int j = 0; j < hmSize; j++)
		for (int k = 0; k < hmSize; k++)
			heightmap(j, k) *= (brightness / 255.0);
}

void raise(int intensity, Mat_<uchar> heightmap, int size) {
	for (int j = 0; j < size; j++)
		for (int k = 0; k < size; k++)
			if (heightmap(j, k) * (1 + intensity / 255) <= 255) {
				heightmap(j, k) *= 1 + intensity / 255;
			}
			else {
				heightmap(j, k) = 255;
			}
}

void lower(int intensity, Mat_<uchar> heightmap, int size) {
	for (int j = 0; j < size; j++)
		for (int k = 0; k < size; k++)
			if (heightmap(j, k) * (1 - intensity / 255) >= 0) {
				heightmap(j, k) *= 1 - intensity / 255;
			}
			else {
				heightmap(j, k) = 0;
			}
}

void filter(int intensity, float mask[][3], Mat_<uchar> image, int size) {
	for (int i = 0; i < intensity; i++) {
		float value_out = 0.0;
		int m, n;
		float sum;

		for (int j = 0; j < size; j++) {
			for (int k = 0; k < size; k++) {
				if (j == 0 || k == 0 || j == size - 1 || k == size - 1)
					image(j, k) = image(j, k);
				else
				{
					sum = 0.0;
					for (m = 0; m < 3; m++)
						for (n = 0; n < 3; n++)
						{
							sum += (float) image(j + m - 1, k + n - 1)*mask[m][n];
						}
					value_out = sum;
					value_out = (value_out > 255 ? 255 : value_out);
					value_out = (value_out < 0 ? 0 : value_out);
					image(j, k) = (int) value_out;
				}
			}
		}
	}
}

void smooth(int intensity, Mat_<uchar> image, int size) {
	// Smoothing filter
	float mask[3][3] = {
		{ 0.11, 0.11, 0.11 },
		{ 0.11, 0.11, 0.11 },
		{ 0.11, 0.11, 0.11 }
	};

	filter(intensity, mask, image, size);
}

void sharpen(int intensity, Mat_<uchar> image, int size) {
	// Sharpening filter
	float mask[3][3] = {
		{ -0.11, -0.11, -0.11 },
		{ -0.11, 1.0, -0.11 },
		{ -0.11, -0.11, -0.11 }
	};

	filter(intensity, mask, image, size);
}

void equalize(Mat_<uchar> image, int size) {
	float   hist[256];  /* counter for histogram */
	float   eq_map[256]; /* equalization mapping function */
	float   one_pixel = 1.0 / ((float) size * size);

	for (int j = 0; j<256; j++)  hist[j] = 0.0;    /* initialization */
	for (int j = 0; j<256; j++)  eq_map[j] = 0.0;  /* initialization */

	for (int j = 0; j < size; j++)
		for (int k = 0; k < size; k++)
		{
			hist[image(j,k)] += one_pixel;
		}

	for (int j = 0; j<256; j++)
	{
		for (int k = 0; k<j + 1; k++)
			eq_map[j] += hist[k];
		eq_map[j] = floor(eq_map[j] * 255.0);
	}

	for (int j = 0; j < size; j++)
		for (int k = 0; k < size; k++)
		{
			image(j,k) = (int) eq_map[image(j,k)];
		}
}

Mat_<uchar> mask(Mat_<uchar> image, bool above, int height, int size) {
	Mat_<uchar> mask(size, size);
	for (int j = 0; j < size; j++)
		for (int k = 0; k < size; k++)
			if (above) {
				if (image(j, k) > height) {
					mask(j, k) = image(j, k);
				}
				else {
					mask(j, k) = 0;
				}
			}
			else {
				if (image(j, k) < height) {
					mask(j, k) = image(j, k);
				}
				else {
					mask(j, k) = 0;
				}
			}
	return mask;
}

Mat_<uchar> angleMask(Mat_<uchar> image, bool above, float angle, int size) {
	Mat_<uchar> mask(size, size);
	Mat_<uchar> angleImg = image.clone();

	double maxVal;

	minMaxLoc(image, &maxVal);

	angle = (int) angle * 2.83333f; // (angle / 90) * 255
	angle /= 2 - maxVal / 255; // Adjust for clamping

	// Edge Detector filter
	float edge[3][3] = {
		{ -0.11, -0.11, -0.11 },
		{ -0.11, 0.88, -0.11 },
		{ -0.11, -0.11, -0.11 }
	};
	filter(1, edge, angleImg, size);

	for (int j = 0; j < size; j++)
		for (int k = 0; k < size; k++)
			if (above) {
				if (angleImg(j, k) > angle) {
					mask(j, k) = image(j, k);
				}
				else {
					mask(j, k) = 0;
				}
			}
			else {
				if (angleImg(j, k) < angle) {
					mask(j, k) = image(j, k);
				}
				else {
					mask(j, k) = 0;
				}
			}
			return mask; 
}

Mat_<uchar> applyOnMask(string type1, string type2, Mat_<uchar> apply1, Mat_<uchar> apply2, int intensity1, int intensity2, int size) {
	// Different operations for mask portion 1
	if (type1 == "smooth") {
		smooth(intensity1, apply1, size);
	}
	else if (type1 == "sharpen") {
		sharpen(intensity1, apply1, size);
	}
	else if (type1 == "clamp") {
		clamp(intensity1, apply1, size);
	}
	else if (type1 == "raise") {
		raise(intensity1, apply1, size);
	}
	else if (type1 == "lower") {
		lower(intensity1, apply1, size);
	}
	
	// Different operations for mask portion 2
	if (type2 == "smooth") {
		smooth(intensity2, apply2, size);
	}
	else if (type2 == "sharpen") {
		sharpen(intensity2, apply2, size);
	}
	else if (type2 == "clamp") {
		clamp(intensity2, apply2, size);
	}
	else if (type2 == "raise") {
		raise(intensity2, apply2, size);
	}
	else if (type2 == "lower") {
		lower(intensity2, apply2, size);
	}

	// Combine both mask modifications
	return apply1 + apply2;
	
}

int main(int argc, char *argv[])
{
	// Simplex Noise Initial Parameters
	int size = 1028; 
	uint32_t seed = 0; 
	float scale = 200.0f; 
	float amplitude = 1.0f; 
	float lacunarity = 2.0f; 
	float persistence = 0.5f; // Typically 1/lacunarity
	size_t octaves = 7; 

	SimplexNoise sn = SimplexNoise(seed, 1/scale, amplitude, lacunarity, persistence);

	Mat_<uchar> heightmap(size, size); // Initialize with user specified size

	/********************************************************************/
	/* Image Processing begins                                          */
	/********************************************************************/

	// Generate Simplex Noisemap with given parameters
	for (int j = 0; j < size; j++)
		for (int k = 0; k < size; k++) {
			//heightmap(j, k) = sn.noise(j, k);
			heightmap(j, k) = sn.fbm(octaves, j, k);
		}
	
	// Modifications (All the filters you want to apply to your image)
	

	/********************************************************************/
	/* Image Processing ends                                          */
	/********************************************************************/

	String windowName1 = "Heightmap";
	namedWindow(windowName1);
	imshow(windowName1, heightmap);

	imwrite("heightmap.jpg", heightmap);

	waitKey(0);
	destroyWindow(windowName1);

	return 0;
}
