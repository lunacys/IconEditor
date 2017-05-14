#pragma once
#include "editor_sheet.h"

class EditorSheet;
struct MouseEventArgs;

class PaintTool
{
public:
	PaintTool(SDL_Texture* texture, int x, int y, int width, int height)
		: width(width), height(height), x(x), y(y), texture(texture)
	{ }
	virtual ~PaintTool()
	{
		SDL_DestroyTexture(texture);
	}

	virtual void on_click(/*EditorSheet* editorSheet, MouseEventArgs args*/) = 0;

	SDL_Point get_position() const
	{
		return SDL_Point{ x, y };
	}
	SDL_Rect get_rect() const 
	{
		return SDL_Rect{ x, y, width, height };
	}

protected:
	int width = 32;
	int height = 32;
	int x = 0, y = 0;

	SDL_Texture* texture;
};
