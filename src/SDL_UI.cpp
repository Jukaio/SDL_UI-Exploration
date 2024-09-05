//
// Created by David Naussed on 2023-12-23.
//

#include "SDL_UI.h"

#include "SDL_UI_FRectUtility.h"
#include<SDL3/SDL_mouse.h>

GENERIC_STACK_FUNCTIONS(UICutMode, SDL_UICutMode)
GENERIC_STACK_FUNCTIONS(UIMeasureMode, SDL_UIMeasureMode)
GENERIC_STACK_FUNCTIONS(UIPadding, SDL_UIPadding)
GENERIC_STACK_FUNCTIONS(UIVerticalAlignment, SDL_UIVerticalAlignmentType)
GENERIC_STACK_FUNCTIONS(UIHorizontalAlignment, SDL_UIHorizontalAlignmentType)
GENERIC_STACK_FUNCTIONS(UIImage, SDL_UIImage);
GENERIC_STACK_FUNCTIONS(UITextureMode, SDL_UITextureMode)
GENERIC_STACK_FUNCTIONS(UICutStyle, SDL_UICutStyle)

// Make these types? 
GENERIC_STACK_FUNCTIONS(UIColor, SDL_Color)
GENERIC_STACK_FUNCTIONS(UIRect, SDL_FRect)
GENERIC_STACK_FUNCTIONS(UIFont, SDL_UIFont*)

bool SDL_UICOmmandTypeToSize(SDL_UICommandType type, size_t *size)
{
	switch (type)
	{
	case SDL_UI_COMMAND_TYPE_HEADER:
		*size = sizeof(SDL_UICommandHeader);
		break;
	case SDL_UI_COMMAND_TYPE_OUTLINE:
		*size = sizeof(SDL_UICommandOutline);
		break;
	case SDL_UI_COMMAND_TYPE_RECT:
		*size = sizeof(SDL_UICommandRect);
		break;
	case SDL_UI_COMMAND_TYPE_LABEL:
		*size = sizeof(SDL_UICommandLabel);
		break;
	case SDL_UI_COMMAND_TYPE_IMAGE:
		*size = sizeof(SDL_UICommandImage);
		break;
	default:
		return false;
	}
	return true;
}

static SDL_UIPadding SDL_UIAdjustPadding(SDL_UIContext const *ctx, SDL_UIPadding padding)
{
	SDL_UIMeasureMode measureMode;
	const bool result = SDL_UIMeasureModeStackTryPeek(&ctx->paddingMeasureModeStack, &measureMode);
	SDL_assert(result && "No padding measure mode on stack");

	SDL_FRect uiRect;
	switch (measureMode)
	{
	case SDL_UI_MEASURE_MODE_ABSOLUTE:
		break;
	case SDL_UI_MEASURE_MODE_RELATIVE_TO_WINDOW:
		SDL_assert(ctx->uiGroupRectStack.count > 0 && "No ui group rect on stack");
		uiRect = *ctx->uiGroupRectStack.data;

		break;
	case SDL_UI_MEASURE_MODE_RELATIVE_TO_GROUP:
		const bool result = SDL_UIRectStackTryPeek(&ctx->uiGroupRectStack, &uiRect);
		SDL_assert(result && "No ui group rect on stack");
		break;
	}

	switch (measureMode)
	{
	case SDL_UI_MEASURE_MODE_ABSOLUTE:
		break;
	case SDL_UI_MEASURE_MODE_RELATIVE_TO_WINDOW:
	case SDL_UI_MEASURE_MODE_RELATIVE_TO_GROUP:
	{
		padding.left *= uiRect.w;
		padding.right *= uiRect.w;
		padding.top *= uiRect.h;
		padding.bottom *= uiRect.h;
	}
	break;
	}
	return padding;
}

static SDL_FRect SDL_UICutActiveRect(SDL_UIContext const *ctx, SDL_UIMeasureMode measureMode, SDL_UICutMode cutMode,
									 float cut)
{
	SDL_FRect *activeRect;
	bool result = SDL_UIRectStackTryPeekMemory(&ctx->activeRectStack, &activeRect);
	SDL_assert(result && "No active rect on stack");

	SDL_UIPadding padding;
	result = SDL_UIPaddingStackTryPeek(&ctx->mainPaddingStack, &padding);
	SDL_assert(result && "No padding on stack");

	SDL_FRect uiRect;
	switch (measureMode)
	{
	case SDL_UI_MEASURE_MODE_ABSOLUTE:
		break;
	case SDL_UI_MEASURE_MODE_RELATIVE_TO_WINDOW:
		uiRect = *ctx->uiGroupRectStack.data;
		SDL_assert(ctx->uiGroupRectStack.count > 0 && "No ui group rect on stack");

		break;
	case SDL_UI_MEASURE_MODE_RELATIVE_TO_GROUP:
		result = SDL_UIRectStackTryPeek(&ctx->uiGroupRectStack, &uiRect);
		SDL_assert(result && "No ui group rect on stack");
		break;
	}

	switch (measureMode)
	{
	case SDL_UI_MEASURE_MODE_ABSOLUTE:
		break;
	case SDL_UI_MEASURE_MODE_RELATIVE_TO_WINDOW:
	case SDL_UI_MEASURE_MODE_RELATIVE_TO_GROUP:
	{
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
	}
	break;
	}

	padding = SDL_UIAdjustPadding(ctx, padding);

	SDL_UICutStyle cutStyle;
	result = SDL_UICutStyleStackTryPeek(&ctx->cutStyleStack, &cutStyle);
	SDL_assert(result && "No cut style on stack");

	switch (cutStyle)
	{
	// Let's just always fallback to the default implementation "Layout" 
	default:
	case SDL_UI_CUT_STYLE_LAYOUT:
	{
		const SDL_FRect rect = SDL_FRectCut(activeRect, cutMode, cut);
		return SDL_FRectApplyPadding(rect, padding);
	}
	case SDL_UI_CUT_STYLE_OVERLAY:
	{
		SDL_FRect copy = *activeRect;
		const SDL_FRect rect = SDL_FRectCut(&copy, cutMode, cut);
		return SDL_FRectApplyPadding(rect, padding);
	}
	}
}


SDL_UIContextIterator SDL_UIContextGetIterator(SDL_UIContext *ctx)
{
	SDL_UIContextIterator iterator;
	iterator.current = 0;
	iterator.count = ctx->commandMemory.count;
	return iterator;
}

bool SDL_UIContextIterate(SDL_UIContext *ctx, SDL_UIContextIterator *it, SDL_UICommandType **command)
{
	void *memory;
	if (SDL_UIMemoryListTryGet(&ctx->commandMemory, it->current, &memory))
	{
		SDL_UICommandType *type = (SDL_UICommandType *)memory;
		*command = type;
		size_t size;
		const bool result = SDL_UICOmmandTypeToSize(*type, &size);
		SDL_assert(result && "SIZE OF COMMAND TYPE NOT IMPLEMENTED COMPONENT!!");

		it->current += size;
		return true;
	}
	return false;
}

SDL_FRect SDL_UIWindowRect(SDL_UIContext *ctx)
{
	SDL_assert(ctx->uiGroupRectStack.data != NULL && ctx->uiGroupRectStack.count > 0 && "No ui group rect on stack");
	return *ctx->uiGroupRectStack.data;
}

SDL_FRect SDL_UIGoupRect(SDL_UIContext *ctx)
{
	SDL_FRect rect;
	const bool result = SDL_UIRectStackTryPeek(&ctx->uiGroupRectStack, &rect);
	SDL_assert(result && "No ui group rect on stack");
	return rect;
}

SDL_FRect SDL_UIActiveRect(SDL_UIContext *ctx)
{
	SDL_FRect rect;
	const bool result = SDL_UIRectStackTryPeek(&ctx->activeRectStack, &rect);
	SDL_assert(result && "No active rect on stack");
	return rect;
}

float SDL_UIRemainingWidth(SDL_UIContext *ctx)
{
	return SDL_UIActiveRect(ctx).w;
}

float SDL_UIRemainingHeight(SDL_UIContext *ctx)
{
	return SDL_UIActiveRect(ctx).h;
}

void SDL_UIContextAlloc(SDL_UIContext *ctx, SDL_UIMeasureTextFunction measureText,
						SDL_UIMeasureImageFunction measureImage)
{
	SDL_assert(measureText && "Measure text function cannot be NULL!");
	SDL_assert(measureImage && "Measure image function cannot be NULL!");

	// Set callback
	ctx->measureText = measureText;
	ctx->measureImage = measureImage;

	SDL_UIMemoryListAlloc(&ctx->commandMemory, 512);

	SDL_UIFontStackAlloc(&ctx->fontStack, 8);

	SDL_UIRectStackAlloc(&ctx->activeRectStack, 8);
	SDL_UIRectStackAlloc(&ctx->uiGroupRectStack, 8);

	SDL_UICutModeStackAlloc(&ctx->cutModeStack, 8);
	SDL_UICutStyleStackAlloc(&ctx->cutStyleStack, 8);

	SDL_UIPaddingStackAlloc(&ctx->mainPaddingStack, 8);
	SDL_UIPaddingStackAlloc(&ctx->textPaddingStack, 8);

	SDL_UIImageStackAlloc(&ctx->hoverImageStack, 8);
	SDL_UIImageStackAlloc(&ctx->imageStack, 8);

	SDL_UIColorStackAlloc(&ctx->hoverImageColorStack, 8);
	SDL_UIColorStackAlloc(&ctx->imageColorStack, 8);

	SDL_UIColorStackAlloc(&ctx->rectColourStack, 8);
	SDL_UIColorStackAlloc(&ctx->outlineColourStack, 8);
	SDL_UIColorStackAlloc(&ctx->textColorStack, 8);

	SDL_UIColorStackAlloc(&ctx->hoverColourStack, 8);
	SDL_UIColorStackAlloc(&ctx->hoverOutlineColourStack, 8);
	SDL_UIColorStackAlloc(&ctx->hoverTextColorStack, 8);

	SDL_UIMeasureModeStackAlloc(&ctx->measureModeStack, 8);
	SDL_UIMeasureModeStackAlloc(&ctx->paddingMeasureModeStack, 8);

	SDL_UIVerticalAlignmentStackAlloc(&ctx->textVerticalAlignmentStack, 8);
	SDL_UIHorizontalAlignmentStackAlloc(&ctx->textHorizontalAlignmentStack, 8);

	SDL_UITextureModeStackAlloc(&ctx->textureModeStack, 8);
}

void SDL_UIContextFree(SDL_UIContext *ctx)
{
	SDL_UITextureModeStackFree(&ctx->textureModeStack);

	SDL_UIVerticalAlignmentStackFree(&ctx->textVerticalAlignmentStack);
	SDL_UIHorizontalAlignmentStackFree(&ctx->textHorizontalAlignmentStack);

	SDL_UIMeasureModeStackFree(&ctx->paddingMeasureModeStack);
	SDL_UIMeasureModeStackFree(&ctx->measureModeStack);

	SDL_UIColorStackFree(&ctx->hoverTextColorStack);
	SDL_UIColorStackFree(&ctx->hoverOutlineColourStack);
	SDL_UIColorStackFree(&ctx->hoverColourStack);
	SDL_UIColorStackFree(&ctx->textColorStack);
	SDL_UIColorStackFree(&ctx->outlineColourStack);
	SDL_UIColorStackFree(&ctx->rectColourStack);

	SDL_UIColorStackFree(&ctx->imageColorStack);
	SDL_UIColorStackFree(&ctx->hoverImageColorStack);

	SDL_UIImageStackFree(&ctx->imageStack);
	SDL_UIImageStackFree(&ctx->hoverImageStack);

	SDL_UIPaddingStackFree(&ctx->textPaddingStack);
	SDL_UIPaddingStackFree(&ctx->mainPaddingStack);

	SDL_UICutStyleStackFree(&ctx->cutStyleStack);
	SDL_UICutModeStackFree(&ctx->cutModeStack);

	SDL_UIRectStackFree(&ctx->uiGroupRectStack);
	SDL_UIRectStackFree(&ctx->activeRectStack);

	SDL_UIFontStackFree(&ctx->fontStack);

	SDL_UIMemoryListFree(&ctx->commandMemory);

	ctx->measureText = NULL; // Invalidate - just in case! 
}

void SDL_UIGroupBegin(SDL_UIContext *ctx, float cut)
{
	SDL_UIMeasureMode measureMode;
	bool result = SDL_UIMeasureModeStackTryPeek(&ctx->measureModeStack, &measureMode);
	SDL_assert(result && "No UI measure mode on stack.");

	SDL_UICutMode cutMode;
	result = SDL_UICutModeStackTryPeek(&ctx->cutModeStack, &cutMode);
	SDL_assert(result && "No UI cutMode on stack.");

	SDL_UIPadding padding;
	result = SDL_UIPaddingStackTryPeek(&ctx->mainPaddingStack, &padding);
	SDL_assert(result && "No padding on stack");

	SDL_FRect rect = SDL_UICutActiveRect(ctx, measureMode, cutMode, cut);
	rect = SDL_FRectApplyPadding(rect, padding);
	SDL_UIRectStackPush(&ctx->activeRectStack, rect);
	SDL_UIRectStackPush(&ctx->uiGroupRectStack, rect);
}

void SDL_UIGroupEnd(SDL_UIContext *ctx)
{
	bool result = SDL_UIRectStackPop(&ctx->activeRectStack);
	result &= SDL_UIRectStackPop(&ctx->uiGroupRectStack);
	SDL_assert(result && "Inconsistent Begin/End - End is getting called too often?");
}

void SDL_UICutModeBegin(SDL_UIContext *ctx, SDL_UICutMode cutMode)
{
	SDL_UICutModeStackPush(&ctx->cutModeStack, cutMode);
}

void SDL_UICutModeEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UICutModeStackPop(&ctx->cutModeStack);
	SDL_assert(result && "Inconsistent Begin/End - End is getting called too often?");
}

void SDL_UICutStyleBegin(SDL_UIContext *ctx, SDL_UICutStyle cutStyle)
{
	SDL_UICutStyleStackPush(&ctx->cutStyleStack, cutStyle);
}

void SDL_UICutStyleEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UICutStyleStackPop(&ctx->cutStyleStack);
	SDL_assert(result && "Inconsistent Begin/End - End is getting called too often?");
}

void SDL_UIMainColorBegin(SDL_UIContext *ctx, SDL_Color color)
{
	SDL_UIColorStackPush(&ctx->rectColourStack, color);
}

void SDL_UIMainColorEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIColorStackPop(&ctx->rectColourStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIOutlineColorBegin(SDL_UIContext *ctx, SDL_Color color)
{
	SDL_UIColorStackPush(&ctx->outlineColourStack, color);
}

void SDL_UIOutlineColorEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIColorStackPop(&ctx->outlineColourStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIFontColorBegin(SDL_UIContext *ctx, SDL_Color color)
{
	SDL_UIColorStackPush(&ctx->textColorStack, color);
}

void SDL_UIFontColorEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIColorStackPop(&ctx->textColorStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIPaddingBegin(SDL_UIContext *ctx, SDL_UIPadding padding)
{
	SDL_UIPaddingStackPush(&ctx->mainPaddingStack, padding);
}

void SDL_UIPaddingEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIPaddingStackPop(&ctx->mainPaddingStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UITextPaddingBegin(SDL_UIContext *ctx, SDL_UIPadding padding)
{
	SDL_UIPaddingStackPush(&ctx->textPaddingStack, padding);
}

void SDL_UITextPaddingEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIPaddingStackPop(&ctx->textPaddingStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIHoverColorBegin(SDL_UIContext *ctx, SDL_Color color)
{
	SDL_UIColorStackPush(&ctx->hoverColourStack, color);
}

void SDL_UIHoverColorEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIColorStackPop(&ctx->hoverColourStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIHoverOutlineColorBegin(SDL_UIContext *ctx, SDL_Color color)
{
	SDL_UIColorStackPush(&ctx->hoverOutlineColourStack, color);
}

void SDL_UIHoverOutlineColorEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIColorStackPop(&ctx->hoverOutlineColourStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIHoverTextColorBegin(SDL_UIContext *ctx, SDL_Color color)
{
	SDL_UIColorStackPush(&ctx->hoverTextColorStack, color);
}

void SDL_UIHoverTextColorEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIColorStackPop(&ctx->hoverTextColorStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIFontBegin(SDL_UIContext *ctx, SDL_UIFont *font)
{
	SDL_UIFontStackPush(&ctx->fontStack, font);
}

void SDL_UIFontEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIFontStackPop(&ctx->fontStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UITextVerticalAlignmentBegin(SDL_UIContext *ctx, SDL_UIVerticalAlignmentType verticalAlignment)
{
	SDL_UIVerticalAlignmentStackPush(&ctx->textVerticalAlignmentStack, verticalAlignment);
}

void SDL_UITextVerticalAlignmentEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIVerticalAlignmentStackPop(&ctx->textVerticalAlignmentStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UITextHorizontalAlignmentBegin(SDL_UIContext *ctx, SDL_UIHorizontalAlignmentType horizontalAlignment)
{
	SDL_UIHorizontalAlignmentStackPush(&ctx->textHorizontalAlignmentStack, horizontalAlignment);
}

void SDL_UITextHorizontalAlignmentEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIHorizontalAlignmentStackPop(&ctx->textHorizontalAlignmentStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIImageBegin(SDL_UIContext *ctx, SDL_UIImage image)
{
	SDL_UIImageStackPush(&ctx->imageStack, image);
}

void SDL_UIImageEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIImageStackPop(&ctx->imageStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIImageColorBegin(SDL_UIContext *ctx, SDL_Color color)
{
	SDL_UIColorStackPush(&ctx->imageColorStack, color);
}

void SDL_UIImageColorEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIColorStackPop(&ctx->imageColorStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIIHoverImageBegin(SDL_UIContext *ctx, SDL_UIImage image)
{
	SDL_UIImageStackPush(&ctx->hoverImageStack, image);
}

void SDL_UIIHoverImageEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIImageStackPop(&ctx->hoverImageStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIHoverImageColorBegin(SDL_UIContext *ctx, SDL_Color color)
{
	SDL_UIColorStackPush(&ctx->hoverImageColorStack, color);
}

void SDL_UIHoverImageColorEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIColorStackPop(&ctx->hoverImageColorStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UITextureModeBegin(SDL_UIContext *ctx, SDL_UITextureMode textureMode)
{
	SDL_UITextureModeStackPush(&ctx->textureModeStack, textureMode);
}

void SDL_UITextureModeEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UITextureModeStackPop(&ctx->textureModeStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIMeasureModeBegin(SDL_UIContext *ctx, SDL_UIMeasureMode measureMode)
{
	SDL_UIMeasureModeStackPush(&ctx->measureModeStack, measureMode);
}

void SDL_UIMeasureModeEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIMeasureModeStackPop(&ctx->measureModeStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIPaddingMeasureModeBegin(SDL_UIContext *ctx, SDL_UIMeasureMode measureMode)
{
	SDL_UIMeasureModeStackPush(&ctx->paddingMeasureModeStack, measureMode);
}

void SDL_UIPaddingMeasureModeEnd(SDL_UIContext *ctx)
{
	const bool result = SDL_UIMeasureModeStackPop(&ctx->paddingMeasureModeStack);
	SDL_assert(result && "Inconsistent Push/Pop call detected");
}

void SDL_UIBegin(SDL_UIContext *ctx, SDL_FRect rect)
{
	SDL_UIMemoryListClear(&ctx->commandMemory);

	SDL_UIColorStackClear(&ctx->rectColourStack);
	SDL_UIColorStackClear(&ctx->outlineColourStack);
	SDL_UIColorStackClear(&ctx->textColorStack);
	SDL_UIColorStackClear(&ctx->hoverColourStack);
	SDL_UIColorStackClear(&ctx->hoverOutlineColourStack);
	SDL_UIColorStackClear(&ctx->hoverTextColorStack);

	SDL_UIColorStackClear(&ctx->imageColorStack);
	SDL_UIColorStackClear(&ctx->hoverImageColorStack);
	SDL_UIImageStackClear(&ctx->imageStack);
	SDL_UIImageStackClear(&ctx->hoverImageStack);

	SDL_UICutModeStackClear(&ctx->cutModeStack);
	SDL_UICutStyleStackClear(&ctx->cutStyleStack);

	SDL_UIMeasureModeStackClear(&ctx->measureModeStack);
	SDL_UIMeasureModeStackClear(&ctx->paddingMeasureModeStack);

	SDL_UIPaddingStackClear(&ctx->mainPaddingStack);
	SDL_UIPaddingStackClear(&ctx->textPaddingStack);
	SDL_UIVerticalAlignmentStackClear(&ctx->textVerticalAlignmentStack);
	SDL_UIHorizontalAlignmentStackClear(&ctx->textHorizontalAlignmentStack);
	SDL_UIFontStackClear(&ctx->fontStack);

	SDL_UIRectStackClear(&ctx->activeRectStack);
	SDL_UIRectStackClear(&ctx->uiGroupRectStack);

	SDL_UITextureModeStackClear(&ctx->textureModeStack);

	SDL_UICutModeBegin(ctx, SDL_UI_CUT_MODE_TOP);
	SDL_UICutStyleBegin(ctx, SDL_UI_CUT_STYLE_LAYOUT);

	SDL_UIMeasureModeBegin(ctx, SDL_UI_MEASURE_MODE_ABSOLUTE);
	SDL_UIPaddingMeasureModeBegin(ctx, SDL_UI_MEASURE_MODE_ABSOLUTE);

	const SDL_UIPadding padding = {0.0f, 0.0f, 0.0f, 0.0f};
	SDL_UIPaddingBegin(ctx, padding);
	SDL_UITextPaddingBegin(ctx, padding);

	SDL_UIRectStackPush(&ctx->activeRectStack, rect);
	SDL_UIRectStackPush(&ctx->uiGroupRectStack, rect);
}

void SDL_UIEnd(SDL_UIContext *ctx)
{
	SDL_UITextPaddingEnd(ctx);
	SDL_UIPaddingEnd(ctx);

	SDL_UIPaddingMeasureModeEnd(ctx);
	SDL_UIMeasureModeEnd(ctx);
	SDL_UICutModeEnd(ctx);
	SDL_UICutStyleEnd(ctx);

	bool result = SDL_UIRectStackPop(&ctx->activeRectStack);
	result &= SDL_UIRectStackPop(&ctx->uiGroupRectStack);
	SDL_assert(result && "Failed to pop last rect");
	SDL_assert(ctx->activeRectStack.count == 0 && "Active rect stack is not empty");
	// left-overs in horizontal and vertical grouping are not muy bien :( 
}

SDL_UICommandHeader SDL_UICommandHeaderConstruct(SDL_UIContext *ctx, float cut)
{
	SDL_UIMeasureMode measureMode;
	bool result = SDL_UIMeasureModeStackTryPeek(&ctx->measureModeStack, &measureMode);
	SDL_assert(result && "No UI measure mode on stack.");

	SDL_UICutMode cutMode;
	result = SDL_UICutModeStackTryPeek(&ctx->cutModeStack, &cutMode);
	SDL_assert(result && "No UI cutMode on stack.");

	const SDL_FRect rect = SDL_UICutActiveRect(ctx, measureMode, cutMode, cut);

	SDL_UICommandHeader header;
	header.type = SDL_UI_COMMAND_TYPE_HEADER;
	header.rect = rect;

	return header;
}

SDL_UICommandOutline SDL_UICommandOutlineConstruct(SDL_UIContext *ctx)
{
	SDL_Color outlineColour;
	const bool result = SDL_UIColorStackTryPeek(&ctx->outlineColourStack, &outlineColour);
	SDL_assert(result && "No UI outline color has been pushed - consider pushing it.");

	SDL_UICommandOutline outlineCommand;
	outlineCommand.type = SDL_UI_COMMAND_TYPE_OUTLINE;
	outlineCommand.color = outlineColour;

	return outlineCommand;
}

SDL_UICommandOutline SDL_UICommandHoveredOutlineConstruct(SDL_UIContext *ctx)
{
	SDL_Color outlineColour;
	const bool result = SDL_UIColorStackTryPeek(&ctx->hoverOutlineColourStack, &outlineColour);
	SDL_assert(result && "No UI hovered outline color has been pushed - consider pushing it.");

	SDL_UICommandOutline outlineCommand;
	outlineCommand.type = SDL_UI_COMMAND_TYPE_OUTLINE;
	outlineCommand.color = outlineColour;

	return outlineCommand;
}

SDL_UICommandRect SDL_UICommandRectConstruct(SDL_UIContext *ctx)
{
	SDL_Color mainColor;
	const bool result = SDL_UIColorStackTryPeek(&ctx->rectColourStack, &mainColor);
	SDL_assert(result && "No UI main color has been pushed - consider pushing it.");

	SDL_UICommandRect rectCommand;;
	rectCommand.type = SDL_UI_COMMAND_TYPE_RECT;
	rectCommand.color = mainColor;

	return rectCommand;
}

SDL_UICommandRect SDL_UICommandHoveredRectConstruct(SDL_UIContext *ctx)
{
	SDL_Color mainColor;
	const bool result = SDL_UIColorStackTryPeek(&ctx->hoverColourStack, &mainColor);
	SDL_assert(result && "No UI hovered main color has been pushed - consider pushing it.");

	SDL_UICommandRect rectCommand;;
	rectCommand.type = SDL_UI_COMMAND_TYPE_RECT;
	rectCommand.color = mainColor;

	return rectCommand;
}

SDL_UICommandLabel SDL_UICOmmandLabelBaseConstruct(SDL_UIContext *ctx, SDL_FRect rect, const char *text)
{
	SDL_UIFont *font;
	bool result = SDL_UIFontStackTryPeek(&ctx->fontStack, &font);
	SDL_assert(result && "No UI font has been pushed - consider pushing it.");

	int width;
	int height;
	result = ctx->measureText(font, text, &width, &height);
	SDL_assert(result && "Measurement failed - Maybe implement a comebacK?");

	SDL_UIHorizontalAlignmentType horizontalAlignment;
	result = SDL_UIHorizontalAlignmentStackTryPeek(&ctx->textHorizontalAlignmentStack, &horizontalAlignment);
	SDL_assert(result && "No horizontal alignment on stack");

	SDL_UIVerticalAlignmentType verticalAlignment;
	result = SDL_UIVerticalAlignmentStackTryPeek(&ctx->textVerticalAlignmentStack, &verticalAlignment);
	SDL_assert(result && "No vertical alignment on stack");

	SDL_UIPadding textPadding;
	result = SDL_UIPaddingStackTryPeek(&ctx->textPaddingStack, &textPadding);
	SDL_assert(result && "No text padding on stack");

	textPadding = SDL_UIAdjustPadding(ctx, textPadding);

	SDL_UICommandLabel label;
	label.type = SDL_UI_COMMAND_TYPE_LABEL;
	label.text = text;
	label.font = font;

	SDL_FRect dst = {rect.x, rect.y, (float)width, (float)height};
	switch (horizontalAlignment)
	{
	case SDL_UI_HORIZONTAL_ALIGNMENT_CENTER:
	{
		float offsetX = rect.w * 0.5f;
		offsetX -= (float)width * 0.5f;
		dst.x += offsetX;
	}
	break;
	case SDL_UI_HORIZONTAL_ALIGNMENT_RIGHT:
	{
		float offsetX = rect.w * 1.0f;
		offsetX -= (float)width * 1.0;
		dst.x += offsetX;
	}
	break;
	case SDL_UI_HORIZONTAL_ALIGNMENT_LEFT:
		break;
	}

	switch (verticalAlignment)
	{
	case SDL_UI_VERTICAL_ALIGNMENT_CENTER:
	{
		float offsetY = rect.h * 0.5f;
		offsetY -= (float)height * 0.5f;
		dst.y += offsetY;
	}
	break;
	case SDL_UI_VERTICAL_ALIGNMENT_BOTTOM:
	{
		float offsetY = rect.h * 1.0f;
		offsetY -= (float)height * 1.0f;
		dst.y += offsetY;
	}
	break;
	case SDL_UI_VERTICAL_ALIGNMENT_TOP:
		break;
	}
	label.rect = dst;


	return label;
}

SDL_UICommandLabel SDL_UICommandLabelConstruct(SDL_UIContext *ctx, SDL_FRect rect, const char *text)
{
	SDL_UICommandLabel label = SDL_UICOmmandLabelBaseConstruct(ctx, rect, text);

	SDL_Color textColor;
	const bool result = SDL_UIColorStackTryPeek(&ctx->textColorStack, &textColor);
	SDL_assert(result && "No UI text color has been pushed - consider pushing it.");

	label.color = textColor;

	return label;
}

SDL_UICommandLabel SDL_UICommandHoveredLabelConstruct(SDL_UIContext *ctx, SDL_FRect rect, const char *text)
{
	SDL_UICommandLabel label = SDL_UICOmmandLabelBaseConstruct(ctx, rect, text);

	SDL_Color textColor;
	bool result = SDL_UIColorStackTryPeek(&ctx->hoverTextColorStack, &textColor);
	SDL_assert(result && "No UI text color has been pushed - consider pushing it.");

	label.color = textColor;

	return label;
}

static SDL_FRect SDL_UITextureModeToRect(SDL_UIContext *ctx, SDL_FRect rect, SDL_UIImage image)
{
	SDL_UITextureMode textureMode;
	bool result = SDL_UITextureModeStackTryPeek(&ctx->textureModeStack, &textureMode);
	SDL_assert(result && "No texture mode on stack");

	switch (textureMode)
	{
	case SDL_UI_TEXTURE_MODE_STRETCH:
		return rect;
	case SDL_UI_TEXTURE_MODE_PRESERVE_ASPECT:
		int width;
		int height;
		result = ctx->measureImage(image, &width, &height);
		SDL_assert(result && "Measurement failed - Maybe implement a comebacK?");

		const float ratioWidth = rect.w / (float)width;
		const float ratioHeight = rect.h / (float)height;

		const float targetRatio = ratioWidth < ratioHeight ? ratioWidth : ratioHeight;

		const float newWidth = (float)width * targetRatio;
		const float newHeight = (float)height * targetRatio;

		const float differenceX = rect.w - newWidth;
		const float differenceY = rect.h - newHeight;

		rect.x = rect.x + differenceX * 0.5f;
		rect.y = rect.y + differenceY * 0.5f;
		rect.w = newWidth;
		rect.h = newHeight;
		return rect;
	}

	// TODO: Maybe assert
	return rect;
}

SDL_UICommandImage SDL_UICommandImageConstruct(SDL_UIContext *ctx, SDL_FRect rect)
{
	SDL_UIImage image;
	bool result = SDL_UIImageStackTryPeek(&ctx->imageStack, &image);
	SDL_assert(result && "No image on stack");

	SDL_Color color;
	result = SDL_UIColorStackTryPeek(&ctx->imageColorStack, &color);
	SDL_assert(result && "No image color on stack");

	SDL_UICommandImage imageCommand;
	imageCommand.type = SDL_UI_COMMAND_TYPE_IMAGE;
	imageCommand.color = color;
	imageCommand.image = image;
	imageCommand.rect = SDL_UITextureModeToRect(ctx, rect, image);

	return imageCommand;
}

SDL_UICommandImage SDL_UICommandHoveredImageConstruct(SDL_UIContext *ctx, SDL_FRect rect)
{
	SDL_UIImage image;
	bool result = SDL_UIImageStackTryPeek(&ctx->hoverImageStack, &image);
	SDL_assert(result && "No image on stack");

	SDL_Color color;
	result = SDL_UIColorStackTryPeek(&ctx->hoverImageColorStack, &color);
	SDL_assert(result && "No image color on stack");

	SDL_UICommandImage imageCommand;
	imageCommand.type = SDL_UI_COMMAND_TYPE_IMAGE;
	imageCommand.color = color;
	imageCommand.image = image;
	imageCommand.rect = SDL_UITextureModeToRect(ctx, rect, image);

	return imageCommand;
}

void SDL_UIActiveOutline(SDL_UIContext *ctx)
{
	SDL_FRect *activeRect;
	bool result = SDL_UIRectStackTryPeekMemory(&ctx->activeRectStack, &activeRect);
	SDL_assert(result && "No active rect on stack");

	// Special Header! 
	SDL_UICommandHeader header;
	header.type = SDL_UI_COMMAND_TYPE_HEADER;
	header.rect = *activeRect;
	SDL_UIMemoryListAppend(&ctx->commandMemory, &header, sizeof(header));

	const SDL_UICommandOutline outline = SDL_UICommandOutlineConstruct(ctx);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &outline, sizeof(outline));
}

void SDL_UIOutline(SDL_UIContext *ctx, float cut)
{
	const SDL_UICommandHeader header = SDL_UICommandHeaderConstruct(ctx, cut);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &header, sizeof(header));

	const SDL_UICommandOutline outline = SDL_UICommandOutlineConstruct(ctx);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &outline, sizeof(outline));
}

void SDL_UIRect(SDL_UIContext *ctx, float cut)
{
	const SDL_UICommandHeader header = SDL_UICommandHeaderConstruct(ctx, cut);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &header, sizeof(header));

	const SDL_UICommandRect rect = SDL_UICommandRectConstruct(ctx);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &rect, sizeof(rect));

	const SDL_UICommandOutline outline = SDL_UICommandOutlineConstruct(ctx);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &outline, sizeof(outline));
}

void SDL_UIEmpty(SDL_UIContext *ctx, float cut)
{
	SDL_UICommandHeaderConstruct(ctx, cut);
	// We do the same as outline or rect command, but discard the result :) 
}

static bool SDL_UIMouseInputWasPressedThisFrame(SDL_UIMouseInput const *input, Uint8 button)
{
	const bool wasPressedLastFrame = input->previous.buttons & SDL_BUTTON(button);
	const bool wasPressedCurrentFrame = input->current.buttons & SDL_BUTTON(button);
	return wasPressedCurrentFrame && !wasPressedLastFrame;
}

void SDL_UILabel(SDL_UIContext *ctx, const char *text, float cut)
{
	const SDL_UICommandHeader header = SDL_UICommandHeaderConstruct(ctx, cut);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &header, sizeof(header));

	const SDL_UICommandRect rect = SDL_UICommandRectConstruct(ctx);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &rect, sizeof(rect));

	const SDL_UICommandOutline outline = SDL_UICommandOutlineConstruct(ctx);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &outline, sizeof(outline));

	const SDL_UICommandLabel label = SDL_UICommandLabelConstruct(ctx, header.rect, text);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &label, sizeof(label));
}

void SDL_UIMeasuredLabel(SDL_UIContext *ctx, const char *text)
{
	SDL_UIFont *font;
	bool result = SDL_UIFontStackTryPeek(&ctx->fontStack, &font);
	SDL_assert(result && "No UI font has been pushed - consider pushing it.");

	int width;
	int height;
	result = ctx->measureText(font, text, &width, &height);
	SDL_assert(result && "Measurement failed - Maybe implement a comebacK?");

	SDL_UIPadding textPadding;
	result = SDL_UIPaddingStackTryPeek(&ctx->textPaddingStack, &textPadding);
	SDL_assert(result && "No text padding on stack");

	const float targetWidth = (float)width + textPadding.left + textPadding.right;
	SDL_UILabel(ctx, text, targetWidth);
}

enum SDL_UIButtonState
{
	SDL_UI_BUTTON_STATE_IDLE,
	SDL_UI_BUTTON_STATE_HOVERED,
	SDL_UI_BUTTON_STATE_CLICKED
};

static bool SDL_FRectPointIntersection(SDL_FRect rect, float x, float y)
{
	return x >= rect.x && x <= rect.x + rect.w && y >= rect.y && y <= rect.y + rect.h;
}

static SDL_UIButtonState SDL_UIButtonBase(SDL_UIContext *ctx, SDL_FRect headerRect)
{
	const float mX = ctx->mouseInput.current.x;
	const float mY = ctx->mouseInput.current.y;

	if (SDL_FRectPointIntersection(headerRect, mX, mY))
	{
		const SDL_UICommandRect rect = SDL_UICommandHoveredRectConstruct(ctx);
		SDL_UIMemoryListAppend(&ctx->commandMemory, &rect, sizeof(rect));

		const SDL_UICommandOutline outline = SDL_UICommandHoveredOutlineConstruct(ctx);
		SDL_UIMemoryListAppend(&ctx->commandMemory, &outline, sizeof(outline));
		if (SDL_UIMouseInputWasPressedThisFrame(&ctx->mouseInput, SDL_BUTTON_LEFT))
		{
			return SDL_UI_BUTTON_STATE_CLICKED;
		}
		return SDL_UI_BUTTON_STATE_HOVERED;
	}

	const SDL_UICommandRect rect = SDL_UICommandRectConstruct(ctx);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &rect, sizeof(rect));

	const SDL_UICommandOutline outline = SDL_UICommandOutlineConstruct(ctx);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &outline, sizeof(outline));
	return SDL_UI_BUTTON_STATE_IDLE;
}

bool SDL_UIButton(SDL_UIContext *ctx, float cut)
{
	const SDL_UICommandHeader header = SDL_UICommandHeaderConstruct(ctx, cut);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &header, sizeof(header));

	return SDL_UIButtonBase(ctx, header.rect) == SDL_UI_BUTTON_STATE_CLICKED;
}

bool SDL_UILabelButton(SDL_UIContext *ctx, const char *text, float cut)
{
	const SDL_UICommandHeader header = SDL_UICommandHeaderConstruct(ctx, cut);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &header, sizeof(header));

	SDL_UIButtonState result = SDL_UIButtonBase(ctx, header.rect);
	if (result == SDL_UI_BUTTON_STATE_HOVERED)
	{
		const SDL_UICommandLabel label = SDL_UICommandHoveredLabelConstruct(ctx, header.rect, text);
		SDL_UIMemoryListAppend(&ctx->commandMemory, &label, sizeof(label));
	}
	else
	{
		const SDL_UICommandLabel label = SDL_UICommandLabelConstruct(ctx, header.rect, text);
		SDL_UIMemoryListAppend(&ctx->commandMemory, &label, sizeof(label));
	}
	return result == SDL_UI_BUTTON_STATE_CLICKED;
}

bool SDL_UIToggle(SDL_UIContext *ctx, bool *state, float cut)
{
	enum SDL_UIToggleState
	{
		SDL_UI_TOGGLE_STATE_OFF = false,
		SDL_UI_TOGGLE_STATE_ON = true
	};

	const SDL_UICommandHeader header = SDL_UICommandHeaderConstruct(ctx, cut);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &header, sizeof(header));

	if (SDL_UIButtonBase(ctx, header.rect) == SDL_UI_BUTTON_STATE_CLICKED)
	{
		switch (*state)
		{
		case SDL_UI_TOGGLE_STATE_OFF:
			*state = SDL_UI_TOGGLE_STATE_ON;
			break;
		case SDL_UI_TOGGLE_STATE_ON:
			*state = SDL_UI_TOGGLE_STATE_OFF;
			break;
		}
	}
}

void SDL_UITexture(SDL_UIContext *ctx, float cut)
{
	const SDL_UICommandHeader header = SDL_UICommandHeaderConstruct(ctx, cut);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &header, sizeof(header));

	const SDL_UICommandImage image = SDL_UICommandImageConstruct(ctx, header.rect);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &image, sizeof(image));
}

bool SDL_UITextureButton(SDL_UIContext *ctx, float cut)
{
	const SDL_UICommandHeader header = SDL_UICommandHeaderConstruct(ctx, cut);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &header, sizeof(header));

	const float mX = ctx->mouseInput.current.x;
	const float mY = ctx->mouseInput.current.y;

	if (SDL_FRectPointIntersection(header.rect, mX, mY))
	{
		const SDL_UICommandImage image = SDL_UICommandHoveredImageConstruct(ctx, header.rect);
		SDL_UIMemoryListAppend(&ctx->commandMemory, &image, sizeof(image));
		if (SDL_UIMouseInputWasPressedThisFrame(&ctx->mouseInput, SDL_BUTTON_LEFT))
		{
			return true;
		}
		return false;
	}

	const SDL_UICommandImage image = SDL_UICommandImageConstruct(ctx, header.rect);
	SDL_UIMemoryListAppend(&ctx->commandMemory, &image, sizeof(image));
	return false;
}

void SDL_UIInputUpdate(SDL_UIContext *ctx)
{
	// For C, we can also just do a simple memcpy
	ctx->mouseInput.previous = ctx->mouseInput.current;

	SDL_Window *window = SDL_GetMouseFocus();
	if (window != NULL)
	{
		ctx->mouseInput.current.buttons = SDL_GetMouseState(&ctx->mouseInput.current.x, &ctx->mouseInput.current.y);
	}
	else
	{
		// is this enough?
		ctx->mouseInput.current.x = -1.0f;
		ctx->mouseInput.current.y = -1.0f;
		ctx->mouseInput.current.buttons = 0.0f;
	}

}
