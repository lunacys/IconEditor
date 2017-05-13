#pragma once
#include "SDL2/SDL.h"
#include <string>
#include "ico_reader.h"

class IconTexture
{
public:
	IconTexture(SDL_Window* window, SDL_Renderer* renderer, int width, int x, int y);
	IconTexture(SDL_Window* window, SDL_Renderer* renderer, const std::string& filename, int x, int y);
	virtual ~IconTexture();

	int get_width() const;

	Color* get_pixel_at(int x, int y);
	void set_pixel_at(int x, int y, const Color& color);

	SDL_Texture* get_texture() const;

	void update();
	void update_point(const SDL_Point& point);
	void render() const;

	//void flip_horizontally();
	//void flip_vertically();

	SDL_Point get_position() const;
	void set_position(int x, int y);
	void set_position(SDL_Point point);

	void set_scale(float val);
	float get_scale() const;

	const std::vector<Color*>& get_image_data() const;

private:
	SDL_Renderer* renderer_;
	SDL_Window* window_;
	SDL_Texture* texture_;

	std::vector<Color*> image_data_;

	int width_;
	int height_;

	int x_, y_;
	float scale_ = 1.0f;
};
