#include "editor_sheet.h"
#include <iostream>


EditorSheet::EditorSheet(SDL_Window* window, SDL_Renderer* renderer, SDL_Rect* viewport)
	: editing_viewport_(viewport), window_(window), renderer_(renderer)
{
	editing_texture_ = new IconTexture(window_, renderer_, 48, editing_viewport_->w / 2, editing_viewport_->h / 2);
}

EditorSheet::~EditorSheet()
{
	delete editing_texture_;
	delete editing_viewport_;
}

void EditorSheet::update(SDL_Event* event)
{
	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	if (event->type == SDL_MOUSEBUTTONDOWN)
	{
		SDL_Keycode k = event->button.button;

		if (k == SDL_BUTTON_LEFT)
			left_mouse_down_ = true;
		if (k == SDL_BUTTON_RIGHT)
			right_mouse_down_ = true;
	}
	else if (event->type == SDL_MOUSEBUTTONUP)
	{
		SDL_Keycode k = event->button.button;

		if (k == SDL_BUTTON_LEFT)
			left_mouse_down_ = false;
		if (k == SDL_BUTTON_RIGHT)
			right_mouse_down_ = false;
	}
	if (event->type == SDL_KEYDOWN)
	{
		SDL_Keycode k = event->key.keysym.sym;
		if (k == SDLK_SPACE)
			std::cout << editing_texture_->get_scale() << std::endl;
		if (k == SDLK_DOWN)
			editing_texture_->set_scale(6);
		if (k == SDLK_UP)
			editing_texture_->set_scale(12);
	}
	if (event->wheel.type == SDL_MOUSEWHEEL)
	{
		if (event->wheel.y < 0)
		{
			editing_texture_->set_scale(editing_texture_->get_scale() - 1);
			event->wheel.y = 0;
		}
		else if (event->wheel.y > 0)
		{
			editing_texture_->set_scale(editing_texture_->get_scale() + 1);
			event->wheel.y = 0;
		}
	}

	if (left_mouse_down_)
	{
		int scale = editing_texture_->get_scale();
		int msofx = mouseX - editing_viewport_->x - (editing_viewport_->w) / 2 + editing_texture_->get_width() / 2 * scale;
		int msofy = mouseY - editing_viewport_->y - (editing_viewport_->h) / 2 + editing_texture_->get_width() / 2 * scale;
		int offsetX = ((msofx - msofx % scale) / scale);
		int offsetY = ((msofy - msofy % scale) / scale);
		Color c;
		c.r = 255;
		c.g = 200;
		c.b = 100;
		c.a = 255;
		editing_texture_->set_pixel_at(offsetX, offsetY, c);

		//MouseEventArgs args = MouseEventArgs{ MouseButton::Left, offsetX, offsetY };
		//current_tool_->on_click(this, args);
	}
	if (right_mouse_down_)
	{
		int scale = editing_texture_->get_scale();
		int msofx = mouseX - editing_viewport_->x - (editing_viewport_->w) / 2 + editing_texture_->get_width() / 2 * scale;
		int msofy = mouseY - editing_viewport_->y - (editing_viewport_->h) / 2 + editing_texture_->get_width() / 2 * scale;
		int offsetX = ((msofx - msofx % scale) / scale);
		int offsetY = ((msofy - msofy % scale) / scale);

		//MouseEventArgs args = MouseEventArgs{ MouseButton::Right, offsetX, offsetY };
		//current_tool_->on_click(this, args);

		auto f = editing_texture_->get_pixel_at(offsetX, offsetY);
		if (f != nullptr)
		{
			//std::cout << (int)f->r << "," << (int)f->g << "," << (int)f->b << std::endl;
		}
	}
}

void EditorSheet::render()
{
	SDL_RenderSetViewport(renderer_, editing_viewport_);
	editing_texture_->render();
	int w = editing_texture_->get_width();
	int s = editing_texture_->get_scale();
	SDL_Rect rect = {editing_viewport_->w / 2 - w / 2 * s, editing_viewport_->h / 2 - w / 2 * s, w * s, w * s};
	SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
	SDL_RenderDrawRect(renderer_, &rect);
	draw_minimap();
}

SDL_Rect* EditorSheet::get_viewport() const
{
	return editing_viewport_;
}

void EditorSheet::set_viewport(SDL_Rect* viewport)
{
	editing_viewport_ = viewport;
}

IconTexture* EditorSheet::get_editing_texture() const
{
	return editing_texture_;
}

void EditorSheet::draw_minimap()
{
	
}
