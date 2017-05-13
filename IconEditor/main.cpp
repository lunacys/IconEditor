#pragma pack(2)
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "SDL2/SDL.h"
#include <vector>
#include "icon_texture.h"

const int WindowWidth = 1024, WindowHeight = 768;

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
	                                      WindowWidth, WindowHeight,
	                                      SDL_WINDOW_SHOWN);

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_Surface* interfaceSurface = SDL_LoadBMP("Interface.bmp");
	SDL_Texture* interfaceTexture = SDL_CreateTextureFromSurface(renderer, interfaceSurface);

	SDL_SetRenderDrawColor(renderer, 127, 127, 127, 255);

	SDL_Rect mainViewport, sheetViewport;
	mainViewport = { 0, 0, WindowWidth, WindowHeight };
	sheetViewport = { 190, 11, WindowWidth - 15 - 190, WindowHeight - 11 - 11};

	//IcoProcessor* first = new IcoProcessor(window, renderer, "48x48.ico", sheetViewport.w / 2, sheetViewport.h/2);
	IconTexture* first = new IconTexture(window, renderer, "48x48.ico", sheetViewport.w / 2, sheetViewport.h / 2);

	int mouseX, mouseY;
	Color asd = { 100, 200, 255, 255 };
	
	first->set_scale(8);

	while (true)
	{
		SDL_PollEvent(&event);
		SDL_GetMouseState(&mouseX, &mouseY);

		if (event.type == SDL_QUIT)
			break;
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			SDL_Keycode k = event.button.button;

			int scale = first->get_scale();
			int msofx = mouseX - sheetViewport.x - (sheetViewport.w ) / 2 + first->get_width() / 2 * scale;
			int msofy = mouseY - sheetViewport.y - (sheetViewport.h ) / 2 + first->get_width() / 2 * scale;
			int offsetX = ((msofx - msofx % scale) / scale);
			int offsetY = ((msofy - msofy % scale) / scale);
			//std::cout << offsetX << " | " << offsetY << std::endl;

			if (k == SDL_BUTTON_LEFT)
				first->set_pixel_at(offsetX, offsetY, asd);
			if (k == SDL_BUTTON_RIGHT)
			{
				auto f = first->get_pixel_at(offsetX, offsetY);
				if (f != nullptr)
				{
					//std::cout << (int)f->r << "," << (int)f->g << "," << (int)f->b << std::endl;
				}
			}
		}
		if (event.type == SDL_KEYDOWN)
		{
			SDL_Keycode k = event.key.keysym.sym;
			if (k == SDLK_ESCAPE)
				break;
			if (k == SDLK_DOWN)
				first->set_scale(first->get_scale() - 1);
			if (k == SDLK_UP)
				first->set_scale(first->get_scale() + 1);
		}

		SDL_RenderClear(renderer);
		SDL_RenderSetViewport(renderer, &mainViewport);
		SDL_RenderCopy(renderer, interfaceTexture, nullptr, &mainViewport);
		
		SDL_RenderSetViewport(renderer, &sheetViewport);
		first->render();
		SDL_RenderPresent(renderer);
	}

	//first->save_to_file("lul.ico");
	IcoReader::save_to_file("asdasd.ico", first->get_image_data(), first->get_width());

	delete first;
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}
