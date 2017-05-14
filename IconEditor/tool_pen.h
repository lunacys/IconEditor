#pragma once
#include "paint_tool.h"

class EditorSheet;
struct MouseEventArgs;

class ToolPen : public PaintTool
{
public:
	ToolPen(SDL_Texture* texture, int x, int y, int width, int height)
		: PaintTool(texture, x, y, width, height)
	{
	}

	void on_click(/*EditorSheet* editorSheet, MouseEventArgs args*/) override;
};
