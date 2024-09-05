//
// Created by David Naussed on 2023-12-23.
//

#ifndef SDL_UI_FRECTUTILITY_IMPLEMENTATION_H
#define SDL_UI_FRECTUTILITY_IMPLEMENTATION_H

#include "SDL_UI_TypeDefinitions.h"

SDL_FRect SDL_FRectApplyPadding(SDL_FRect rect, SDL_UIPadding padding);

float SDL_FRectCalculateCut(SDL_FRect rect, SDL_UIMeasureMode measureMode, SDL_UICutMode cutMode, float cut);
SDL_FRect SDL_FRectCut(SDL_FRect *target, SDL_UICutMode cutMode, float cut);

/*SDL_FRect SDL_UICutRect(SDL_FRect *rect, SDL_FRect uiRect, SDL_UIMeasureMode measureMode, SDL_UICutMode cutMode,
						float cut);*/

#endif //SDL_UI_FRECTUTILITY_IMPLEMENTATION_H
