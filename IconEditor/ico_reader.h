#pragma once
#include <vector>

struct Color
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
};

class IcoReader
{
public:
	static std::vector<Color*> load_from_file(std::string filename, int& width);
	static void save_to_file(std::string filename, const std::vector<Color*>& imgData, int width);

private:
	typedef struct
	{
		unsigned char        bWidth;          // Width, in pixels, of the image
		unsigned char        bHeight;         // Height, in pixels, of the image
		unsigned char        bColorCount;     // Number of colors in image (0 if >=8bpp)
		unsigned char        bReserved;       // Reserved ( must be 0)
		unsigned short        wPlanes;         // Color Planes
		unsigned short        wBitCount;       // Bits per pixel
		unsigned int       dwBytesInRes;    // How many bytes in this resource?
		unsigned int       dwImageOffset;   // Where in the file is this image?
	} ICONDIRENTRY, *LPICONDIRENTRY;

	typedef struct
	{
		unsigned short           idReserved;   // Reserved (must be 0)
		unsigned short           idType;       // Resource Type (1 for icons)
		unsigned short           idCount;      // How many images?
	} ICONDIR, *LPICONDIR;

	typedef struct tagBITMAPINFOHEADER {
		unsigned int biSize;
		int  biWidth;
		int  biHeight;
		unsigned short  biPlanes;
		unsigned short  biBitCount;
		unsigned int biCompression;
		unsigned int biSizeImage;
		int  biXPelsPerMeter;
		int  biYPelsPerMeter;
		unsigned int biClrUsed;
		unsigned int biClrImportant;
	} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
};
