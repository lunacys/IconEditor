#ifndef ICO_PROCESSOR_H
#define ICO_PROCESSOR_H
#pragma pack(2)
#include <vector>
#include "SDL2/SDL.h"

struct BGRA
{
	unsigned char b;
	unsigned char g;
	unsigned char r;
	unsigned char a;
};

class IcoProcessor
{
public:
	IcoProcessor(SDL_Window* window, SDL_Renderer* renderer, int width, int height, int x, int y);
	IcoProcessor(SDL_Window* window, SDL_Renderer* renderer, const std::string& filename, int x, int y);
	virtual ~IcoProcessor();

	void load_from_file(std::string filename);
	void save_to_file(std::string filename);

	int get_width() const;
	int get_height() const;

	BGRA* get_pixel_at(int x, int y);
	void set_pixel_at(int x, int y, const BGRA& quad);

	SDL_Texture* get_texture() const;

	void update();
	void update_point(const SDL_Point& point);
	void render() const;

	void flip_horizontally();

	SDL_Point get_position() const;
	void set_position(int x, int y);
	void set_position(SDL_Point point);

	void set_scale(int val);
	int get_scale() const;

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

	std::vector<BGRA*> pixel_data_;

	SDL_Renderer* renderer_;
	SDL_Window* window_;
	SDL_Texture* texture_;

	int width_;
	int height_;

	int x_, y_;
	int scale_ = 1;
};

#endif // ICO_PROCESSOR_H
