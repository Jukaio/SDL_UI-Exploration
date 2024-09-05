//
// Created by David Naussed on 2023-12-23.
//
#ifndef SDL_UI_IMPLEMENTATION_H
#define SDL_UI_IMPLEMENTATION_H


#include "SDL_UI_StackTypeDefinitions.h"

#include "SDL_UI_MemoryList.h"

// Needs testing -> Return hint type??
typedef bool (*SDL_UIMeasureTextFunction)(SDL_UIFont font, const char *str, int *w, int *h);
typedef bool (*SDL_UIMeasureImageFunction)(SDL_UIImage image, int *w, int *h);

struct SDL_UIContext
{
	SDL_UIMemoryList commandMemory;

	SDL_UIColorStack rectColourStack;
	SDL_UIColorStack outlineColourStack;
	SDL_UIColorStack textColorStack;

	SDL_UIColorStack hoverColourStack;
	SDL_UIColorStack hoverOutlineColourStack;
	SDL_UIColorStack hoverTextColorStack;

	SDL_UIColorStack imageColorStack;
	SDL_UIColorStack hoverImageColorStack;

	SDL_UIImageStack imageStack;
	SDL_UIImageStack hoverImageStack;

	SDL_UICutModeStack cutModeStack;
	SDL_UICutStyleStack cutStyleStack;

	SDL_UIMeasureModeStack measureModeStack;
	SDL_UIMeasureModeStack paddingMeasureModeStack;

	SDL_UIPaddingStack mainPaddingStack;
	SDL_UIPaddingStack textPaddingStack;

	SDL_UIVerticalAlignmentStack textVerticalAlignmentStack;
	SDL_UIHorizontalAlignmentStack textHorizontalAlignmentStack;

	SDL_UIFontStack fontStack;

	SDL_UIRectStack uiGroupRectStack;
	SDL_UIRectStack activeRectStack;

	SDL_UITextureModeStack textureModeStack;

	SDL_UIMeasureTextFunction measureText;
	SDL_UIMeasureImageFunction measureImage;

	SDL_UIMouseInput mouseInput;
};

struct SDL_UIContextIterator
{
	Uint32 current;
	Uint32 count;
};

// Utility
SDL_FRect SDL_FRectCreate(float x, float y, float w, float h);
SDL_UIContextIterator SDL_UIContextGetIterator(SDL_UIContext *ctx);
bool SDL_UIContextIterate(SDL_UIContext *ctx, SDL_UIContextIterator *it, SDL_UICommandType **command);

SDL_FRect SDL_UIWindowRect(SDL_UIContext *ctx);
SDL_FRect SDL_UIGoupRect(SDL_UIContext *ctx);
SDL_FRect SDL_UIActiveRect(SDL_UIContext *ctx);

// while(SDL_UIRemainingWidth(ctx) > 0) { /* do something */ }
float SDL_UIRemainingWidth(SDL_UIContext *ctx);
float SDL_UIRemainingHeight(SDL_UIContext *ctx);

// Resource
void SDL_UIContextAlloc(SDL_UIContext *ctx, SDL_UIMeasureTextFunction measureText,
						SDL_UIMeasureImageFunction measureImage);
void SDL_UIContextFree(SDL_UIContext *ctx);

// Style
void SDL_UICutModeBegin(SDL_UIContext *ctx, SDL_UICutMode cutMode);
void SDL_UICutModeEnd(SDL_UIContext *ctx);

void SDL_UICutStyleBegin(SDL_UIContext *ctx, SDL_UICutStyle cutStyle);
void SDL_UICutStyleEnd(SDL_UIContext *ctx);

void SDL_UIMeasureModeBegin(SDL_UIContext *ctx, SDL_UIMeasureMode measureMode);
void SDL_UIMeasureModeEnd(SDL_UIContext *ctx);

void SDL_UIPaddingMeasureModeBegin(SDL_UIContext *ctx, SDL_UIMeasureMode measureMode);
void SDL_UIPaddingMeasureModeEnd(SDL_UIContext *ctx);

void SDL_UIMainColorBegin(SDL_UIContext *ctx, SDL_Color color);
void SDL_UIMainColorEnd(SDL_UIContext *ctx);
void SDL_UIOutlineColorBegin(SDL_UIContext *ctx, SDL_Color color);
void SDL_UIOutlineColorEnd(SDL_UIContext *ctx);
void SDL_UIFontColorBegin(SDL_UIContext *ctx, SDL_Color color);
void SDL_UIFontColorEnd(SDL_UIContext *ctx);

void SDL_UIPaddingBegin(SDL_UIContext *ctx, SDL_UIPadding padding);
void SDL_UIPaddingEnd(SDL_UIContext *ctx);

void SDL_UITextPaddingBegin(SDL_UIContext *ctx, SDL_UIPadding padding);
void SDL_UITextPaddingEnd(SDL_UIContext *ctx);

void SDL_UIHoverColorBegin(SDL_UIContext *ctx, SDL_Color color);
void SDL_UIHoverColorEnd(SDL_UIContext *ctx);
void SDL_UIHoverOutlineColorBegin(SDL_UIContext *ctx, SDL_Color color);
void SDL_UIHoverOutlineColorEnd(SDL_UIContext *ctx);
void SDL_UIHoverTextColorBegin(SDL_UIContext *ctx, SDL_Color color);
void SDL_UIHoverTextColorEnd(SDL_UIContext *ctx);

void SDL_UIFontBegin(SDL_UIContext *ctx, SDL_UIFont *font);
void SDL_UIFontEnd(SDL_UIContext *ctx);

void SDL_UITextVerticalAlignmentBegin(SDL_UIContext *ctx, SDL_UIVerticalAlignmentType verticalAlignment);
void SDL_UITextVerticalAlignmentEnd(SDL_UIContext *ctx);

void SDL_UITextHorizontalAlignmentBegin(SDL_UIContext *ctx, SDL_UIHorizontalAlignmentType verticalAlignment);
void SDL_UITextHorizontalAlignmentEnd(SDL_UIContext *ctx);

void SDL_UIImageBegin(SDL_UIContext *ctx, SDL_UIImage image);
void SDL_UIImageEnd(SDL_UIContext *ctx);

void SDL_UIImageColorBegin(SDL_UIContext *ctx, SDL_Color color);
void SDL_UIImageColorEnd(SDL_UIContext *ctx);

void SDL_UIIHoverImageBegin(SDL_UIContext *ctx, SDL_UIImage image);
void SDL_UIIHoverImageEnd(SDL_UIContext *ctx);

void SDL_UIHoverImageColorBegin(SDL_UIContext *ctx, SDL_Color color);
void SDL_UIHoverImageColorEnd(SDL_UIContext *ctx);

void SDL_UITextureModeBegin(SDL_UIContext *ctx, SDL_UITextureMode textureMode);
void SDL_UITextureModeEnd(SDL_UIContext *ctx);

// Elements
void SDL_UIBegin(SDL_UIContext *ctx, SDL_FRect rect);
void SDL_UIEnd(SDL_UIContext *ctx);

void SDL_UIGroupBegin(SDL_UIContext *ctx, float cut);
void SDL_UIGroupEnd(SDL_UIContext *ctx);

void SDL_UIActiveOutline(SDL_UIContext *ctx);
void SDL_UIOutline(SDL_UIContext *ctx, float cut);
void SDL_UIRect(SDL_UIContext *ctx, float cut);
void SDL_UIEmpty(SDL_UIContext *ctx, float cut);

void SDL_UILabel(SDL_UIContext *ctx, const char *text, float cut);
void SDL_UIMeasuredLabel(SDL_UIContext *ctx, const char *text);

bool SDL_UIButton(SDL_UIContext *ctx, float cut);
bool SDL_UILabelButton(SDL_UIContext *ctx, const char *text, float cut);

//void SDL_UITexture(SDL_UIContext * ctx, SDL_Tex)

// TODO: LOL
bool SDL_UIToggle(SDL_UIContext *ctx, bool *state, float cut);

void SDL_UITexture(SDL_UIContext *ctx, float cut);
bool SDL_UITextureButton(SDL_UIContext *ctx, float cut);


/*
#define SDL_UI_TOGGLE_START(ctx, state, cut) SDL_UIToggleStart(ctx, &state, cut); \
	switch(state) {
	
#define SDL_UI_TOGGLE_ON break; case true: \

#define SDL_UI_TOGGLE_OFF break; case false: \

#define SDL_UI_TOGGLE_COMPLETE(ctx) }\
SDL_UIToggleComplete(ctx);
*/

//
void SDL_UIInputUpdate(SDL_UIContext *ctx);

#endif //SDL_UI_IMPLEMENTATION_H
