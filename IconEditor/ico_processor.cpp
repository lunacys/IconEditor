#include "ico_processor.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <iterator>

IcoProcessor::IcoProcessor(SDL_Window* window, SDL_Renderer* renderer, int width, int height, int x, int y)
	: width_(width), height_(height), x_(x), y_(y)
{
	window_ = window;
	renderer_ = renderer;
	texture_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
}

IcoProcessor::IcoProcessor(SDL_Window* window, SDL_Renderer* renderer, const std::string& filename, int x, int y)
	: x_(x), y_(y)
{
	window_ = window;
	renderer_ = renderer;
	load_from_file(filename);
}

IcoProcessor::~IcoProcessor()
{
	SDL_DestroyTexture(texture_);
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
		//std::cout << "Length: " << length << std::endl;
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
		//std::cout << width_ << ", " << height_ << ": "<< std::endl;

		texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width_, height_);
		

		//std::cout << "BytesInRes: " << icondirentry->dwBytesInRes << " | ImageOffset: " << icondirentry->dwImageOffset << std::endl;
		//std::cout << "Size: " << info_header->biSize << " | SizeImage: " << info_header->biSizeImage << std::endl;

		auto start_it = buffer.begin() + sizeof(ICONDIR) + sizeof(ICONDIRENTRY) + sizeof(BITMAPINFOHEADER);
		auto end_it = buffer.end() - info_header->biHeight / 2 * 4; 

		int c = 0;
		BGRA* rgb = new BGRA;


		for (auto i = start_it; i != end_it; ++i)
		{
			c++;
			switch (c)
			{
			case 1: 
				rgb = new BGRA(); 
				rgb->b = *i; 
			break;
			case 2: 
				rgb->g = *i; 
			break;
			case 3: 
				rgb->r = *i; 
			break;
			case 4:
				rgb->a = *i;
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
		file.write((char*)c, sizeof(BGRA));
	}
	for (int i = 0; i < height_ * 4; i++)
	{
		BGRA* rgb = new BGRA{ 0,0,0,0 };
		file.write((char*)rgb, sizeof(BGRA));
		delete rgb;
	}

	file.close();
}

int IcoProcessor::get_width() const
{
	return width_;
}

int IcoProcessor::get_height() const
{
	return height_;
}

BGRA* IcoProcessor::get_pixel_at(int x, int y)
{
	if (x < 0 || y < 0 || x > width_ || y > height_)
		throw std::runtime_error("Out of image bounds");
	return pixel_data_[x * width_ + y];
}

void IcoProcessor::set_pixel_at(int x, int y, const BGRA& quad)
{
	if (x < 0 || y < 0 || x > width_ - 1 || y > height_ - 1)
		return;//throw std::runtime_error("Out of image bounds");
	int p = y * width_ + x;

	pixel_data_[p]->r = quad.r;
	pixel_data_[p]->g = quad.g;
	pixel_data_[p]->b = quad.b;
	pixel_data_[p]->a = quad.a;
	update_point(SDL_Point{x, y});
}

SDL_Texture* IcoProcessor::get_texture() const
{
	return texture_;
}

void IcoProcessor::update()
{
	void* pixels; int pitch;

	//SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_BLEND);

	SDL_LockTexture(texture_, nullptr, &pixels, &pitch);

	Uint32* p = static_cast<Uint32*>(pixels);

	for (int i = 0; i < width_ * height_; i++)
	{
		p[i] = 
			SDL_MapRGBA(SDL_GetWindowSurface(window_)->format,
			pixel_data_[i]->r, pixel_data_[i]->g, pixel_data_[i]->b, pixel_data_[i]->a);
	}

	SDL_UnlockTexture(texture_);
}

void IcoProcessor::update_point(const SDL_Point& point)
{
	void* pixels; int pitch;
	SDL_LockTexture(texture_, nullptr, &pixels, &pitch);
	Uint32* p = static_cast<Uint32*>(pixels);
	int pos = point.y * width_ + point.x;

	p[pos] =
		SDL_MapRGBA(SDL_GetWindowSurface(window_)->format,
			pixel_data_[pos]->r, pixel_data_[pos]->g, pixel_data_[pos]->b, pixel_data_[pos]->a);

	SDL_UnlockTexture(texture_);
}

void IcoProcessor::render() const
{
	//SDL_Point center = { width_ / 2, height_ / 2 };
	SDL_Rect renderQuad = { x_ - width_ / 2 * scale_, y_ - height_ / 2 * scale_, width_ * scale_, height_ * scale_ }; 
	SDL_RenderCopy(renderer_, texture_, nullptr, &renderQuad);
}


void IcoProcessor::flip_horizontally()
{
	
}

SDL_Point IcoProcessor::get_position() const
{
	return SDL_Point{ x_, y_ };
}

void IcoProcessor::set_position(int x, int y)
{
	x_ = x;
	y_ = y;
}

void IcoProcessor::set_position(SDL_Point point)
{
	set_position(point.x, point.y);
}

void IcoProcessor::set_scale(int val)
{
	scale_ = val;
}

int IcoProcessor::get_scale() const
{
	return scale_;
}

