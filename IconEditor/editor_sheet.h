#pragma once
#include "icon_texture.h"
#include "paint_tool.h"
#include "tool_pen.h"

enum class MouseButton
{
	Left,
	Right
};

struct MouseEventArgs
{
	MouseButton button;
	int x;
	int y;
};

class EditorSheet
{
public:
	EditorSheet(SDL_Window* window, SDL_Renderer* renderer, SDL_Rect* viewport);
	virtual ~EditorSheet();

	void update(SDL_Event* event);
	void render();

	SDL_Rect* get_viewport() const;
	void set_viewport(SDL_Rect* viewport);
	IconTexture* get_editing_texture() const;

private:
	void draw_minimap();

	IconTexture* editing_texture_;
	SDL_Rect* editing_viewport_;
	SDL_Window* window_;
	SDL_Renderer* renderer_;

	bool left_mouse_down_ = false;
	bool right_mouse_down_ = false;
};
