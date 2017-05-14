#include "icon_editor.h"
#include <stdexcept>

IconEditor::IconEditor()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		throw std::runtime_error("Error whilte initializing SDL");
	}

	window_width_ = 1024;
	window_height_ = 768;

	window_ = SDL_CreateWindow("Icon Editor",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		window_width_, window_height_,
		SDL_WINDOW_SHOWN);

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

	SDL_SetRenderDrawColor(renderer_, 127, 127, 127, 255);

	viewport_ = new SDL_Rect{ 0, 0, window_width_, window_height_ };

	SDL_Rect* vp = new SDL_Rect{ 165, 42, 1016 - 165, 750 - 42 };
	editor_sheet_ = new EditorSheet{ window_, renderer_, vp};

	interface_texture_ = SDL_CreateTextureFromSurface(renderer_, SDL_LoadBMP("Interface.bmp"));
}

IconEditor::~IconEditor()
{
	SDL_DestroyRenderer(renderer_);
	SDL_Quit();
}

void IconEditor::run()
{
	main_loop();
}

void IconEditor::exit()
{
	is_running_ = false;
}

void IconEditor::update()
{
	SDL_PollEvent(&event_);
	editor_sheet_->update(&event_);

	if (event_.type == SDL_QUIT)
		is_running_ = false;
	if (event_.type == SDL_KEYDOWN)
	{
		SDL_Keycode k = event_.key.keysym.sym;
		if (k == SDLK_ESCAPE)
			is_running_ = false;
	}
}

void IconEditor::render()
{
	SDL_RenderClear(renderer_);
	SDL_RenderSetViewport(renderer_, viewport_);
	SDL_Rect r = { 0,0,window_width_, window_height_ };
	SDL_RenderCopy(renderer_, interface_texture_, nullptr, &r);

	auto t = editor_sheet_->get_editing_texture();
	SDL_Rect r2 = SDL_Rect{ 32, 600, t->get_width(), t->get_width() };
	SDL_RenderCopyEx(renderer_, t->get_texture(), nullptr, &r2, 0.0, nullptr, SDL_FLIP_VERTICAL);

	editor_sheet_->render();
	SDL_RenderPresent(renderer_);
}

void IconEditor::main_loop()
{
	while(is_running_)
	{
		update();
		render();
	}
}
