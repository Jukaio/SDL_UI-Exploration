//
// Created by David Naussed on 2024-01-01.
//

#include "SDL_UISpriteAtlas.h"

#include <SDL3/SDL_render.h>

#include <HighestBitDeBruijn.h>
#include <SDL_assert.h>
#include <stdlib.h>


void SDL_UISpriteAtlasAlloc(SDL_UISpriteAtlas *atlas, SDL_Texture *texture, Uint32 capacity)
{
	atlas->texture = texture;
	// The texture itself is always index 0 :)
	capacity += 1;
	atlas->rects = (SDL_FRect *)calloc(capacity, sizeof(SDL_FRect));
	atlas->count = 1;
	atlas->capacity = capacity;
}

void SDL_UISpriteAtlasRealloc(SDL_UISpriteAtlas *atlas)
{
	const Uint32 newCapacity = 1 << (HighestSetBitDeBruijn32(atlas->capacity) + 1);
	SDL_assert(newCapacity > 0 && "New Capacity is not allowed to be 0");
	SDL_assert(newCapacity > atlas->capacity && "New capacity must be bigger than old capacity");

	atlas->rects = (SDL_FRect *)realloc(atlas->rects, newCapacity * sizeof(SDL_FRect));
	SDL_assert(atlas->rects != NULL && "Reallocation of stack failed!");
	atlas->capacity = newCapacity;
}

SDL_UISprite SDL_UISpriteAtlasCreate(SDL_UISpriteAtlas *atlas, SDL_FRect rect)
{
	if (atlas->count >= atlas->capacity)
	{
		SDL_UISpriteAtlasRealloc(atlas);
	}

	const SDL_UISpriteIndex sprite = atlas->count;
	atlas->rects[atlas->count] = rect;
	atlas->count = atlas->count + 1;
	return (SDL_UISprite){atlas, sprite};
}

void SDL_UISpriteAtlasSetColor(SDL_UISpriteAtlas const *atlas, SDL_Color color)
{
	SDL_assert(atlas->texture != NULL && "No texture in sprite atlas");

	SDL_SetTextureColorMod(atlas->texture, color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(atlas->texture, color.a);
}

void SDL_UISpriteAtlasRender(SDL_Renderer *renderer, SDL_UISprite const *sprite, SDL_FRect dst)
{
	SDL_assert(sprite->atlas->texture != NULL && "No texture in sprite atlas");
	SDL_assert(sprite->index < sprite->atlas->count && "Sprite is out of bounds?");
	const SDL_FRect src = sprite->atlas->rects[sprite->index];

	const int result = SDL_RenderTexture(renderer, sprite->atlas->texture, &src, &dst);
	SDL_assert(result == 0 && "Cannot render texture");
}

void SDL_UISpriteAtlasFree(SDL_UISpriteAtlas *atlas)
{
	atlas->texture = NULL;
	SDL_assert(atlas->rects != NULL && "Cannot free non-allocated Sprite Atlas");
	free(atlas->rects);
	atlas->rects = NULL;
	atlas->count = 0;
	atlas->capacity = 0;
}
