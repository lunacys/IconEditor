#include "ico_processor.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <iterator>

IcoProcessor::IcoProcessor(SDL_Window* window, SDL_Renderer* renderer, int width, int height)
	: width_(width), height_(height)
{
	window_ = window;
	renderer_ = renderer;
	texture_ = nullptr;
	//texture_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width, height);
	//for (int i = 0; i < width * height; i++)
		//pixel_data_.push_back(new Rgb{ (Uint8)0xFF, 0x00, 0x00 });
	//update();
}

IcoProcessor::IcoProcessor(SDL_Window* window, SDL_Renderer* renderer, const std::string& filename)
{
	window_ = window;
	renderer_ = renderer;
	//texture_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, 0, 0);
	load_from_file(filename);
	// TODO: Remake SDL_CreateTexture
}

IcoProcessor::~IcoProcessor()
{
	for (auto c : pixel_data_)
		delete c;
}

void IcoProcessor::load_from_file(std::string filename)
{
	std::ifstream file(filename, std::ios::binary);

	if (file) { 
		file.unsetf(std::ios::skipws);

		file.seekg(0, std::ios::end);
		std::streampos length = file.tellg();
		std::cout << "Length: " << length << std::endl;
		file.seekg(0, std::ios::beg);

		std::vector<Uint8> buffer;
		buffer.reserve(length);
	
		buffer.insert(buffer.begin(),
			std::istream_iterator<unsigned char>(file),
			std::istream_iterator<unsigned char>());

		LPICONDIR icondir = reinterpret_cast<LPICONDIR>(&buffer[0]);
		LPICONDIRENTRY icondirentry = reinterpret_cast<LPICONDIRENTRY>(&buffer[0] + sizeof(ICONDIR));
		

		if (icondir->idType != 1 || icondir->idReserved != 0 || icondir->idCount > 1
			|| icondirentry->wBitCount != 32)
		{
			throw std::runtime_error("Unsupported format");
		}

		PBITMAPINFOHEADER info_header = reinterpret_cast<PBITMAPINFOHEADER>(&buffer[0] + sizeof(ICONDIR) + sizeof(ICONDIRENTRY));

		if ((info_header->biWidth != info_header->biHeight / 2) ||
			(info_header->biWidth != 16 && info_header->biWidth != 32 && info_header->biWidth != 48 && info_header->biWidth != 64))
		{
			throw std::runtime_error("Unsupported resolution");
		}
		
		width_ = info_header->biWidth;
		height_ = info_header->biHeight / 2;
		std::cout << width_ << ", " << height_ << ": "<< std::endl;

		texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, width_, height_);

		std::cout << "BytesInRes: " << icondirentry->dwBytesInRes << " | ImageOffset: " << icondirentry->dwImageOffset << std::endl;
		std::cout << "Size: " << info_header->biSize << " | SizeImage: " << info_header->biSizeImage << std::endl;

		auto start_it = buffer.begin() + sizeof(ICONDIR) + sizeof(ICONDIRENTRY) + sizeof(BITMAPINFOHEADER);
		auto end_it = buffer.end(); 

		int c = 0;
		Rgb* rgb = new Rgb;


		for (auto i = start_it; i != end_it; ++i)
		{
			c++;
			switch (c)
			{
			case 1: 
				rgb = new Rgb(); 
				rgb->b = *i; 
			break;
			case 2: 
				rgb->g = *i; 
			break;
			case 3: 
				rgb->r = *i; 
			break;
			case 4:
				rgb->s = 255;
				pixel_data_.push_back(rgb);
			default:
				c = 0; 
			break;
			}
		}
		update();

		file.close();
	}
}

void IcoProcessor::save_to_file(std::string filename)
{
	std::ofstream file(filename, std::ios::out | std::ios::binary);

	LPICONDIR icondir = new ICONDIR;// = reinterpret_cast<LPICONDIR>(&buffer_[0]);
	LPICONDIRENTRY icondirentry = new ICONDIRENTRY;// = reinterpret_cast<LPICONDIRENTRY>(&buffer_[0] + sizeof(ICONDIR));
	PBITMAPINFOHEADER info_header = new BITMAPINFOHEADER;// = reinterpret_cast<PBITMAPINFOHEADER>(&buffer_[0] + sizeof(ICONDIR) + sizeof(ICONDIRENTRY));

	unsigned int bytesInRes = 0;
	unsigned int sizeImage = 0;

	switch (width_ + height_)
	{
	case 16 + 16: bytesInRes = 1128; sizeImage = 2048; break;
	case 32 + 32: bytesInRes = 4264; sizeImage = 8192; break;
	case 48 + 48: bytesInRes = 9640; sizeImage = 18432; break;
	case 64 + 64: bytesInRes = 16936; sizeImage = 32768; break;
	default: std::cout << "lul" << std::endl; break;
	}

	icondir->idReserved = 0;
	icondir->idCount = 1;
	icondir->idType = 1;

	icondirentry->bWidth = width_;
	icondirentry->bHeight = height_;
	icondirentry->bColorCount = 0;
	icondirentry->bReserved = 0;
	icondirentry->wPlanes = 1;
	icondirentry->wBitCount = 32;
	icondirentry->dwBytesInRes = bytesInRes;
	icondirentry->dwImageOffset = 22;

	info_header->biSize = 40;
	info_header->biWidth = width_;
	info_header->biHeight = height_ * 2;
	info_header->biPlanes = 1;
	info_header->biBitCount = 32;
	info_header->biCompression = 0;
	info_header->biSizeImage = sizeImage;
	info_header->biXPelsPerMeter = 0;
	info_header->biYPelsPerMeter = 0;
	info_header->biClrUsed = 0;
	info_header->biClrImportant = 0;

	file.seekp(0);
	file.write((char*)icondir, sizeof(ICONDIR));
	file.write((char*)icondirentry, sizeof(ICONDIRENTRY));
	file.write((char*)info_header, sizeof(BITMAPINFOHEADER));

	for (auto &c : pixel_data_)
	{
		file.write((char*)c, sizeof(Rgb));
	}

	file.close();
}

int IcoProcessor::get_image_width() const
{
	return width_;
}

int IcoProcessor::get_image_height() const
{
	return height_;
}

Rgb* IcoProcessor::get_pixel_at(int x, int y)
{
	if (x < 0 || y < 0 || x > width_ || y > height_)
		throw std::runtime_error("Out of image bounds");
	return pixel_data_[x * width_ + y];
}

void IcoProcessor::set_pixel_at(int x, int y, Rgb* quad)
{
	if (x < 0 || y < 0 || x > width_ || y > height_)
		throw std::runtime_error("Out of image bounds");
	pixel_data_[x * width_ + y] = quad;
	update();
}

SDL_Texture* IcoProcessor::to_texture() const
{
	return texture_;
}

void IcoProcessor::update()
{
	void* pixels; int pitch;
	SDL_LockTexture(texture_, nullptr, &pixels, &pitch);

	Uint32* p = static_cast<Uint32*>(pixels);
	std::cout << "size: " << pixel_data_.size() << std::endl;

	for (int i = 0; i < width_ * height_; i++)
	{
		p[i] = 
			SDL_MapRGB(SDL_GetWindowSurface(window_)->format,
			pixel_data_[i]->r, pixel_data_[i]->g, pixel_data_[i]->b);

		//Uint8 r, g, b;
		//SDL_GetRGB(p[i], SDL_GetWindowSurface(window_)->format, &r, &g, &b);
	}

	SDL_UnlockTexture(texture_);
}

void IcoProcessor::flip_horizontally()
{
	
}

