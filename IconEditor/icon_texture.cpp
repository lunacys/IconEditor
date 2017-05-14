#include "icon_texture.h"

IconTexture::IconTexture(SDL_Window* window, SDL_Renderer* renderer, int width, int x, int y)
	: width_(width), height_(width), x_(x), y_(y)
{
	//if (width != 16 || width != 32 || width != 48 || width != 64) throw std::runtime_error("Invalid size");

	window_ = window;
	renderer_ = renderer;

	for (int i = 0; i < width * width; i++)
	{
		Color* white = new Color;
		white->r = white->g = white->b = white->a = 255;
		image_data_.push_back(white);
	}

	texture_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, width);
	update();
}

IconTexture::IconTexture(SDL_Window* window, SDL_Renderer* renderer, const std::string& filename, int x, int y)
	: x_(x), y_(y)
{
	window_ = window;
	renderer_ = renderer;
	int width;
	image_data_ = IcoReader::load_from_file(filename, width);
	width_ = height_ = width;

	texture_ = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, width);
	update();
}

IconTexture::~IconTexture()
{
	SDL_DestroyTexture(texture_);
	for (auto c : image_data_)
		delete c;
}

int IconTexture::get_width() const
{
	return width_;
}

Color* IconTexture::get_pixel_at(int x, int y)
{
	if (x < 0 || y < 0 || x > width_ - 1 || y > height_ - 1)
		return nullptr;//throw std::runtime_error("Out of image bounds");
	return image_data_[(height_ - 1 - y) * width_ + x];
}

void IconTexture::set_pixel_at(int x, int y, const Color& color)
{
	if (x < 0 || y < 0 || x > width_ - 1 || y > height_ - 1)
		return;//throw std::runtime_error("Out of image bounds");
	int p = (height_ - y - 1) * width_ + x;

	image_data_[p]->r = color.r;
	image_data_[p]->g = color.g;
	image_data_[p]->b = color.b;
	image_data_[p]->a = color.a;
	update();//_point(SDL_Point{x, y});
}

SDL_Texture* IconTexture::get_texture() const
{
	return texture_;
}

void IconTexture::update()
{
	void* pixels; int pitch;

	//SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_BLEND);

	SDL_LockTexture(texture_, nullptr, &pixels, &pitch);

	Uint32* p = static_cast<Uint32*>(pixels);

	for (int i = 0; i < width_ * height_; i++)
	{
		p[i] =
			SDL_MapRGBA(SDL_GetWindowSurface(window_)->format,
				image_data_[i]->r, image_data_[i]->g, image_data_[i]->b, image_data_[i]->a);
	}

	SDL_UnlockTexture(texture_);
}

void IconTexture::update_point(const SDL_Point& point)
{
	void* pixels; int pitch;
	SDL_LockTexture(texture_, nullptr, &pixels, &pitch);
	Uint32* p = static_cast<Uint32*>(pixels);
	int pos = point.y * width_ + point.x;

	p[pos] =
		SDL_MapRGBA(SDL_GetWindowSurface(window_)->format,
			image_data_[pos]->r, image_data_[pos]->g, image_data_[pos]->b, image_data_[pos]->a);

	SDL_UnlockTexture(texture_);
}

void IconTexture::render() const
{
	SDL_Point center = { width_ / 2, height_ / 2 };
	SDL_Rect renderQuad = { x_ - width_ / 2 * scale_, y_ - height_ / 2 * scale_, width_ * scale_, height_ * scale_ };
	SDL_RenderCopyEx(renderer_, texture_, nullptr, &renderQuad, 0.0, &center, SDL_FLIP_VERTICAL);
}

SDL_Point IconTexture::get_position() const
{
	return SDL_Point{ x_, y_ };
}

void IconTexture::set_position(int x, int y)
{
	x_ = x;
	y_ = y;
}

void IconTexture::set_position(SDL_Point point)
{
	set_position(point.x, point.y);
}

void IconTexture::set_scale(int val)
{
	scale_ = val;
	if (scale_ < 1) scale_ = 1;
	if (scale_ > 16) scale_ = 16;
}

int IconTexture::get_scale() const
{
	return scale_;
}

const std::vector<Color*>& IconTexture::get_image_data() const
{
	return image_data_;
}
