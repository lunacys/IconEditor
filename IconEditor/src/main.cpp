#pragma pack(2)
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "SDL2/SDL.h"
#include <vector>
#include "../ico_processor.h"


int main(int argc, char *argv[])
{
	SDL_Event event;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 3;
	}

	SDL_Window *window = SDL_CreateWindow("SDL_CreateTexture",
	                                      SDL_WINDOWPOS_UNDEFINED,
	                                      SDL_WINDOWPOS_UNDEFINED,
	                                      1024, 768,
	                                      SDL_WINDOW_RESIZABLE);

	SDL_Rect rect = { 128, 128, 16, 16 };
	SDL_Rect rect2 = { 128 + 16, 128, 32, 32 };
	SDL_Rect rect3 = { 128 + 16 + 32, 128, 48, 48 };
	SDL_Rect rect4 = { 128 + 16 + 32 + 48, 128, 64, 64 };

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

	IcoProcessor* i16x16 = new IcoProcessor(window, renderer, "16x16.ico");
	IcoProcessor* i32x32 = new IcoProcessor(window, renderer, "32x32.ico"); 
	IcoProcessor* i48x48 = new IcoProcessor(window, renderer, "48x48.ico"); 
	IcoProcessor* i64x64 = new IcoProcessor(window, renderer, "64x64.ico");

	auto red = Rgb{ 255, 255, 0 };
	i64x64->set_pixel_at(0, 0, &red);
	i48x48->flip_horizontally();
	i48x48->update();

	i16x16->save_to_file("sdsfd.ico");
	i64x64->save_to_file("newxd.ico");

	//proc->save_to_file("hello.t");
	SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);
	// TODO: DEAL WITH ALPHA CHANNELS
	SDL_Point ttt{ 24, 24 };

	while (true) {
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			break;

		SDL_RenderClear(renderer);
		SDL_RenderCopyEx(renderer, i16x16->to_texture(), nullptr, &rect, 0.0, &ttt, SDL_FLIP_VERTICAL);
		SDL_RenderCopyEx(renderer, i32x32->to_texture(), nullptr, &rect2, 0.0, &ttt, SDL_FLIP_VERTICAL);
		SDL_RenderCopyEx(renderer, i48x48->to_texture(), nullptr, &rect3, 0.0, &ttt, SDL_FLIP_VERTICAL);
		SDL_RenderCopyEx(renderer, i64x64->to_texture(), nullptr, &rect4, 0.0, &ttt, SDL_FLIP_VERTICAL);
		SDL_RenderPresent(renderer);
	}

	//delete i16x16;
	//SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
