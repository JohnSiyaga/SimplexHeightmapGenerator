# SimplexHeightmapGenerator
Heightmap Generator written in C++ with OpenCV Libraries.  
Simplex Noise algorithm based off of Stefan Gustavson's work.  

# Input Parameters:
Octaves - Controls the layering of the Simplex Noise

Scale - Controls the 'zoom' level of the noise image (1 / frequency)

Amplitude - Controls the contribution of pixels in the noise image

Lacunarity - Controls the gaps in frequency between octaves

Persistence - Controls the speed at which higher octaves fade in magnitude (Typically 1 / Lacunarity)

# Functions:
void invert(Mat_<uchar> heightmap, int size):  
- Inverts the pixels of the image  

void clamp(int brightness, Mat_<uchar> heightmap, int hmSize):   
- Performs linear interpolation to bring brightness values of image from [0-255] to [0-brightness]  
  
void raise(int intensity, Mat_<uchar> heightmap, int size):   
- Raises pixels in image by a factor of 1 + intensity / 255  

void lower(int intensity, Mat_<uchar> heightmap, int size):   
- Lowers pixels in image by a factor of 1 - intensity / 255  
  
void filter(int intensity, float mask[][3], Mat_<uchar> image, int size):  
- Applies a filter on the input image  
  
void smooth(int intensity, Mat_<uchar> image, int size):  
- Applies a smoothing filter on the input image  

void sharpen(int intensity, Mat_<uchar> image, int size):  
- Applies a sharpening filter on the input image  
  
void equalize(Mat_<uchar> image, int size):  
- Equalizes the input image  

Mat_<uchar> mask(Mat_<uchar> image, bool above, int height, int size):  
- Returns all values either above or below a brightness value  
  
Mat_<uchar> angleMask(Mat_<uchar> image, bool above, float angle, int size):  
- Returns all values above a specified slope (in degrees)  
  
Mat_<uchar> applyOnMask(string type1, string type2, Mat_<uchar> apply1, Mat_<uchar> apply2, int intensity1, int intensity2, int size):  
- Returns a matrix that combines two mask images with two filters applied selectively  
