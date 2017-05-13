#include "ico_reader.h"
#include <fstream>
#include <iostream>
#include <iterator>

std::vector<Color*> IcoReader::load_from_file(std::string filename, int& width)
{
	std::ifstream file(filename, std::ios::binary);

	if (!file) throw std::runtime_error("Cannot open file");

	std::vector<Color*> img_data;
	file.unsetf(std::ios::skipws);

	file.seekg(0, std::ios::end);
	std::streampos length = file.tellg();
	//std::cout << "Length: " << length << std::endl;
	file.seekg(0, std::ios::beg);

	std::vector<unsigned char> buffer;
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

	width = info_header->biWidth;

	//height_ = info_header->biHeight / 2;
	//std::cout << width_ << ", " << height_ << ": "<< std::endl;

	//texture_ = SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width_, height_);


	//std::cout << "BytesInRes: " << icondirentry->dwBytesInRes << " | ImageOffset: " << icondirentry->dwImageOffset << std::endl;
	//std::cout << "Size: " << info_header->biSize << " | SizeImage: " << info_header->biSizeImage << std::endl;

	auto start_it = buffer.begin() + sizeof(ICONDIR) + sizeof(ICONDIRENTRY) + sizeof(BITMAPINFOHEADER);
	auto end_it = buffer.end() - info_header->biHeight / 2 * 4;

	int c = 0;
	Color* rgb = new Color;

	for (auto i = start_it; i != end_it; ++i)
	{
		c++;
		switch (c)
		{
		case 1:
			rgb = new Color();
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
			img_data.push_back(rgb);
		default:
			c = 0;
			break;
		}
	}

	file.close();

	return img_data;
}

void IcoReader::save_to_file(std::string filename, const std::vector<Color*>& imgData, int width)
{
	std::ofstream file(filename, std::ios::out | std::ios::binary);

	LPICONDIR icondir = new ICONDIR;// = reinterpret_cast<LPICONDIR>(&buffer_[0]);
	LPICONDIRENTRY icondirentry = new ICONDIRENTRY;// = reinterpret_cast<LPICONDIRENTRY>(&buffer_[0] + sizeof(ICONDIR));
	PBITMAPINFOHEADER info_header = new BITMAPINFOHEADER;// = reinterpret_cast<PBITMAPINFOHEADER>(&buffer_[0] + sizeof(ICONDIR) + sizeof(ICONDIRENTRY));

	unsigned int bytesInRes = 0;
	unsigned int sizeImage = 0;

	switch (width * 2)
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

	icondirentry->bWidth = width;
	icondirentry->bHeight = width;
	icondirentry->bColorCount = 0;
	icondirentry->bReserved = 0;
	icondirentry->wPlanes = 1;
	icondirentry->wBitCount = 32;
	icondirentry->dwBytesInRes = bytesInRes;
	icondirentry->dwImageOffset = 22;

	info_header->biSize = 40;
	info_header->biWidth = width;
	info_header->biHeight = width * 2;
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

	for (auto &c : imgData)
	{
		file.write((char*)c, sizeof(Color));
	}
	for (int i = 0; i < width * 4; i++)
	{
		Color* rgb = new Color{ 0,0,0,0 };
		file.write((char*)rgb, sizeof(Color));
		delete rgb;
	}

	file.close();
}
