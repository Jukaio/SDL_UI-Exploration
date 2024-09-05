//
// Created by David Naussed on 2023-12-23.
//

#ifndef SDL_UI_STACKDEFINITIONS_IMPLEMENTATION_H
#define SDL_UI_STACKDEFINITIONS_IMPLEMENTATION_H

#include "SDL_UI_TypeDefinitions.h"
#include "GENERIC_STACK.h"

#include <stdlib.h>

GENERIC_STACK_TYPE(UICutMode, SDL_UICutMode)

GENERIC_STACK_TYPE(UICutStyle, SDL_UICutStyle)

GENERIC_STACK_TYPE(UIMeasureMode, SDL_UIMeasureMode)

GENERIC_STACK_TYPE(UIPadding, SDL_UIPadding)

GENERIC_STACK_TYPE(UIVerticalAlignment, SDL_UIVerticalAlignmentType)

GENERIC_STACK_TYPE(UIHorizontalAlignment, SDL_UIHorizontalAlignmentType)

GENERIC_STACK_TYPE(UIImage, SDL_UIImage)

GENERIC_STACK_TYPE(UITextureMode, SDL_UITextureMode)


// Make these types? 
GENERIC_STACK_TYPE(UIColor, SDL_Color)

GENERIC_STACK_TYPE(UIRect, SDL_FRect)

GENERIC_STACK_TYPE(UIFont, SDL_UIFont*)

#endif //SDL_UI_STACKDEFINITIONS_IMPLEMENTATION_H
