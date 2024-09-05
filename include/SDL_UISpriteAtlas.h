//
// Created by David Naussed on 2024-01-01.
//

#ifndef SDL_UISPRITEATLAS_IMPLEMENTATION_H
#define SDL_UISPRITEATLAS_IMPLEMENTATION_H

#include <SDL_pixels.h>
#include <SDL_rect.h>

struct SDL_Texture;
struct SDL_Renderer;

struct SDL_UISpriteAtlas
{
	SDL_Texture* texture;
	SDL_FRect* rects;
	Uint32 count;
	Uint32 capacity;
};

typedef size_t SDL_UISpriteIndex;

struct SDL_UISprite
{
	SDL_UISpriteAtlas* atlas;
	SDL_UISpriteIndex index;
};

void SDL_UISpriteAtlasAlloc(SDL_UISpriteAtlas* atlas, SDL_Texture* texture, Uint32 capacity = 0);

SDL_UISprite SDL_UISpriteAtlasCreate(SDL_UISpriteAtlas* atlas, SDL_FRect rect);

void SDL_UISpriteAtlasSetColor(SDL_UISpriteAtlas const* atlas, SDL_Color color);

void SDL_UISpriteAtlasRender(SDL_Renderer* renderer, SDL_UISprite const* sprite, SDL_FRect dst);

void SDL_UISpriteAtlasFree(SDL_UISpriteAtlas* atlas);

#endif //SDL_UISPRITEATLAS_IMPLEMENTATION_H
