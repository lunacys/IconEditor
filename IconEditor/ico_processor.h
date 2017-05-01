#ifndef ICO_PROCESSOR_H
#define ICO_PROCESSOR_H
#pragma pack(2)
#include <vector>
#include "SDL2/SDL.h"

struct Rgb
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char s;
};

class IcoProcessor
{
public:
	IcoProcessor(SDL_Window* window, SDL_Renderer* renderer, int width, int height);
	IcoProcessor(SDL_Window* window, SDL_Renderer* renderer, const std::string& filename);
	virtual ~IcoProcessor();

	void load_from_file(std::string filename);
	void save_to_file(std::string filename);
	int get_image_width() const;
	int get_image_height() const;
	Rgb* get_pixel_at(int x, int y);
	void set_pixel_at(int x, int y, Rgb* quad);
	SDL_Texture* to_texture() const;
	void update();
	void flip_horizontally();

private:
	typedef int LONG;
	typedef unsigned short WORD;
	typedef unsigned int DWORD;

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
		//ICONDIRENTRY   idEntries[1]; // An entry for each image (idCount of 'em)
	} ICONDIR, *LPICONDIR;

	typedef struct tagBITMAPINFOHEADER {
		DWORD biSize;
		LONG  biWidth;
		LONG  biHeight;
		WORD  biPlanes;
		WORD  biBitCount;
		DWORD biCompression;
		DWORD biSizeImage;
		LONG  biXPelsPerMeter;
		LONG  biYPelsPerMeter;
		DWORD biClrUsed;
		DWORD biClrImportant;
	} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

	//std::vector<char> buffer_;

	std::vector<Rgb*> pixel_data_;

	SDL_Renderer* renderer_;
	SDL_Window* window_;
	SDL_Texture* texture_;

	int width_;
	int height_;

	
};

#endif // ICO_PROCESSOR_H
