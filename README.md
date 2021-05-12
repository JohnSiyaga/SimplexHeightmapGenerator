# SimplexHeightmapGenerator
Heightmap Generator written in C++ with OpenCV Libraries. Simplex Noise algorithm based off of Stefan Gustavson's work. 

Input Parameters:
Octaves - Controls the layering of the Simplex Noise
Scale - Controls the 'zoom' level of the noise image (1 / frequency)
Amplitude - Controls the contribution of pixels in the noise image
Lacunarity - Controls the gaps in frequency between octaves
Persistence - Controls the speed at which higher octaves fade in magnitude (Typically 1 / Lacunarity)

Functions:
void invert(Mat_<uchar> heightmap, int size) - 
void clamp(int brightness, Mat_<uchar> heightmap, int hmSize) - 
void raise(int intensity, Mat_<uchar> heightmap, int size) - 
void lower(int intensity, Mat_<uchar> heightmap, int size) -
void filter(int intensity, float mask[][3], Mat_<uchar> image, int size) -
void smooth(int intensity, Mat_<uchar> image, int size) -
void sharpen(int intensity, Mat_<uchar> image, int size) -
void equalize(Mat_<uchar> image, int size) -
Mat_<uchar> mask(Mat_<uchar> image, bool above, int height, int size) -
Mat_<uchar> angleMask(Mat_<uchar> image, bool above, float angle, int size) -
Mat_<uchar> applyOnMask(string type1, string type2, Mat_<uchar> apply1, Mat_<uchar> apply2, int intensity1, int intensity2, int size) -
