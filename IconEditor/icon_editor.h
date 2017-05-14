#pragma once
#include "SDL2/SDL.h"
#include "editor_sheet.h"

class IconEditor
{
public:
	IconEditor();
	virtual ~IconEditor();

	void run();
	void exit();

	void update();
	void render();
	void main_loop();

private:
	SDL_Window* window_;
	SDL_Renderer* renderer_;
	SDL_Event event_;
	SDL_Rect* viewport_;

	SDL_Texture* interface_texture_;

	EditorSheet* editor_sheet_;

	int window_width_;
	int window_height_;

	bool is_running_ = true;
};
