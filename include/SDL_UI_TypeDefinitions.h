//
// Created by David Naussed on 2023-12-23.
//

#ifndef SDL_UI_TYPEDEFINITIONS_IMPLEMENTATION_H
#define SDL_UI_TYPEDEFINITIONS_IMPLEMENTATION_H

#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_pixels.h>

typedef void *SDL_UIFont;
typedef void *SDL_UIImage;

enum SDL_UICommandType
{
	SDL_UI_COMMAND_TYPE_HEADER,
	SDL_UI_COMMAND_TYPE_OUTLINE,
	SDL_UI_COMMAND_TYPE_RECT,
	SDL_UI_COMMAND_TYPE_LABEL,
	SDL_UI_COMMAND_TYPE_IMAGE,
};

enum SDL_UIVerticalAlignmentType
{
	SDL_UI_VERTICAL_ALIGNMENT_CENTER,
	SDL_UI_VERTICAL_ALIGNMENT_TOP,
	SDL_UI_VERTICAL_ALIGNMENT_BOTTOM,
};

enum SDL_UIHorizontalAlignmentType
{
	SDL_UI_HORIZONTAL_ALIGNMENT_CENTER,
	SDL_UI_HORIZONTAL_ALIGNMENT_LEFT,
	SDL_UI_HORIZONTAL_ALIGNMENT_RIGHT,
};

enum SDL_UITextureMode
{
	SDL_UI_TEXTURE_MODE_STRETCH,
	SDL_UI_TEXTURE_MODE_PRESERVE_ASPECT,
};

struct SDL_UICommandHeader
{
	SDL_UICommandType type;

	SDL_FRect rect;
	//SDL_UIBaseCommand* next;
};

struct SDL_UICommandOutline
{
	SDL_UICommandType type;

	SDL_Color color;
};

struct SDL_UICommandRect
{
	SDL_UICommandType type;

	SDL_Color color;
};


struct SDL_UICommandLabel
{
	SDL_UICommandType type;

	SDL_Color color;
	const char *text;
	SDL_UIFont *font;

	SDL_FRect rect;
};

struct SDL_UICommandImage
{
	SDL_UICommandType type;

	SDL_Color color;
	SDL_FRect rect;
	SDL_UIImage image;
};

enum SDL_UICutMode
{
	SDL_UI_CUT_MODE_LEFT,
	SDL_UI_CUT_MODE_RIGHT,
	SDL_UI_CUT_MODE_TOP,
	SDL_UI_CUT_MODE_BOTTOM,
};

// TODO: I need to rethink this... all of it
enum SDL_UICutStyle
{
	SDL_UI_CUT_STYLE_LAYOUT,
	SDL_UI_CUT_STYLE_OVERLAY
};


enum SDL_UIMeasureMode
{
	SDL_UI_MEASURE_MODE_ABSOLUTE,
	SDL_UI_MEASURE_MODE_RELATIVE_TO_WINDOW,
	SDL_UI_MEASURE_MODE_RELATIVE_TO_GROUP,
};


struct SDL_UIPadding
{
	float left;
	float right;
	float top;
	float bottom;
};

struct SDL_UIMouseState
{
	Uint32 buttons;

	float x;
	float y;
};

struct SDL_UIMouseInput
{
	SDL_UIMouseState previous;
	SDL_UIMouseState current;
};

#endif //SDL_UI_TYPEDEFINITIONS_IMPLEMENTATION_H
