//
// Created by David Naussed on 2023-12-23.
//

#include "SDL_UI.h"

SDL_FRect SDL_FRectCreate(float x, float y, float w, float h)
{
	SDL_FRect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	return rect;
}

SDL_FRect SDl_FRectCutRight(SDL_FRect *target, float cut)
{
	target->w = SDL_max(target->w - cut, 0);
	return SDL_FRectCreate(target->x + target->w, target->y, cut, target->h);
}

SDL_FRect SDl_FRectCutBottom(SDL_FRect *target, float cut)
{
	target->h = SDL_max(target->h - cut, 0);
	return SDL_FRectCreate(target->x, target->y + target->h, target->w, cut);
}

SDL_FRect SDl_FRectCutLeft(SDL_FRect *target, float cut)
{
	const float width = target->w;
	target->w = SDL_max(target->w - cut, 0);
	float x = target->x;
	target->x = target->x + (width - target->w);
	return SDL_FRectCreate(x, target->y, cut, target->h);
}

SDL_FRect SDl_FRectCutTop(SDL_FRect *target, float cut)
{
	const float height = target->h;
	target->h = SDL_max(target->h - cut, 0);
	float y = target->y;
	target->y = target->y + (height - target->h);
	return SDL_FRectCreate(target->x, y, target->w, cut);
}

SDL_FRect SDL_FRectCut(SDL_FRect *target, SDL_UICutMode cutMode, float cut)
{
	switch (cutMode)
	{
	case SDL_UI_CUT_MODE_LEFT:
		return SDl_FRectCutLeft(target, cut);
	case SDL_UI_CUT_MODE_RIGHT:
		return SDl_FRectCutRight(target, cut);
	case SDL_UI_CUT_MODE_TOP:
		return SDl_FRectCutTop(target, cut);
	case SDL_UI_CUT_MODE_BOTTOM:
		return SDl_FRectCutBottom(target, cut);
	}
	SDL_assert(false && "Should never end up here - Invalid Rect cut mode");
	return *target;
}

SDL_FRect SDL_FRectApplyPadding(SDL_FRect rect, SDL_UIPadding padding)
{
	rect.x += padding.left;
	rect.y += padding.top;

	rect.w -= (padding.left + padding.right);
	rect.h -= (padding.top + padding.bottom);
	return rect;
}

float SDL_FRectCalculateCut(SDL_FRect rect, SDL_UIMeasureMode measureMode, SDL_UICutMode cutMode, float cut)
{
	switch (measureMode)
	{
	case SDL_UI_MEASURE_MODE_ABSOLUTE:
		return cut;
	case SDL_UI_MEASURE_MODE_RELATIVE_TO_WINDOW:
		float size = 0;
		switch (cutMode)
		{
		case SDL_UI_CUT_MODE_LEFT:
		case SDL_UI_CUT_MODE_RIGHT:
			size = rect.w;
			break;
		case SDL_UI_CUT_MODE_TOP:
		case SDL_UI_CUT_MODE_BOTTOM:
			size = rect.h;
			break;
		}
		cut = size * cut;
		return cut;
	}
	SDL_assert(false && "Invalid measure mode or cut mode");
	return 0.0f;
}

/*SDL_FRect SDL_UICutRect(SDL_FRect *rect, SDL_FRect uiRect, SDL_UIMeasureMode measureMode, SDL_UICutMode cutMode,
						float cut)
{
	switch (measureMode)
	{
	case SDL_UI_MEASURE_MODE_ABSOLUTE:
		break;
	case SDL_UI_MEASURE_MODE_RELATIVE_TO_WINDOW:
		float size = 0;
		switch (cutMode)
		{
		case SDL_UI_CUT_MODE_LEFT:
		case SDL_UI_CUT_MODE_RIGHT:
			size = uiRect.w;
			break;
		case SDL_UI_CUT_MODE_TOP:
		case SDL_UI_CUT_MODE_BOTTOM:
			size = uiRect.h;
			break;
		}
		cut = size * cut;
		break;
	}

	return SDL_FRectCut(rect, cutMode, cut);
}*/


