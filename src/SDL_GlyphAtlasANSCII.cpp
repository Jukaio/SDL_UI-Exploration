//
// Created by David Naussed on 2024-01-01.
//

#include "SDL_GlyphAtlasANSCII.h"

#include <SDL_assert.h>
#include <SDL_render.h>
#include <SDL_stdinc.h>
#include <SDL3_ttf/SDL_ttf.h>


bool SDL_GlyphAtlasANSCIIMeasureText(SDL_GlyphAtlasANSCII *atlas, const char *str, int *w, int *h)
{
	//SDL_GlyphAtlasANSCII *atlas = (SDL_GlyphAtlasANSCII *)font;
	*w = 0;
	for (const char *c = str; *c != '\0'; c++)
	{
		const char character = *c;
		const Uint16 stride = atlas->strides[character - 1];
		const Uint16 nextStride = atlas->strides[character];
		const Uint16 width = (nextStride - stride) * atlas->fontSize;

		*w += width;
	}
	*h = atlas->fontSize;
	return true;
}

void SDL_GlyphAtlasANSCIICreate(SDL_Renderer *renderer, SDL_GlyphAtlasANSCII *atlas, TTF_Font *font, SDL_Color color)
{
	SDL_assert(atlas != NULL && "Required a glyph atlas to emplace data in!");
	SDL_assert(renderer != NULL && "SDL_Renderer is required to create texture - provide one");
	SDL_assert(font != NULL && "Cannot create glyph atlas from NULL font");

	const char minChar = 0x01;
	const char maxChar = 0x7F;
	SDL_Surface *glyphs[maxChar];
	int glyphStride[maxChar];
	Uint16 stride = 0;

	for (char c = minChar; c < maxChar; c++)
	{
		SDL_Surface *surface = TTF_RenderGlyph_Blended(font, c, color);
		SDL_assert(surface != NULL && "Error in rendering glyph");

		int width = surface->w;
		glyphs[c] = surface;
		glyphStride[c] = stride;
		stride += width;
	}

	// Corresponds to ptsize in TTF_OpenFont 
	const int fontHeight = TTF_FontHeight(font);
	SDL_Surface *glyphAtlasSurface = SDL_CreateSurface(stride, fontHeight, SDL_PIXELFORMAT_RGBA32);
	for (char c = minChar; c < maxChar; c++)
	{
		int s = glyphStride[c];
		SDL_Surface *surface = glyphs[c];
		SDL_Rect srcRect = {0, 0, surface->w, surface->h};
		SDL_Rect dstRect = {s, 0, surface->w, surface->h};
		const int result = SDL_BlitSurfaceUnchecked(surface, &srcRect, glyphAtlasSurface, &dstRect);
		SDL_assert(result == 0 && "Error in blitting");
		atlas->strides[c - 1] = s / fontHeight;
		SDL_DestroySurface(surface);
	}

	// Good measure cause it doesn't exist
	atlas->strides[127] = stride / fontHeight;
	atlas->glyphTexture = SDL_CreateTextureFromSurface(renderer, glyphAtlasSurface);
	atlas->fontSize = fontHeight;
	atlas->textureWidth = stride;
	SDL_DestroySurface(glyphAtlasSurface);
}

void SDL_GlyphAtlasANSCIIRenderText(SDL_Renderer *renderer, SDL_GlyphAtlasANSCII const *atlas, SDL_FRect destination,
									SDL_Color color, const char *str)
{
	const bool result = SDL_SetTextureColorMod(atlas->glyphTexture, color.r, color.g, color.b);
	SDL_assert(result != -1 && "Cannot set color mod, implement fallback!");

	float offset = 0;
	for (const char *c = str; *c != '\0'; c++)
	{
		const char character = *c;
		const Uint16 stride = atlas->strides[character - 1];
		const Uint16 nextStride = atlas->strides[character];
		const Uint16 width = (nextStride - stride) * atlas->fontSize;

		SDL_FRect src = {(float)stride * (float)atlas->fontSize, 0, (float)width, (float)atlas->fontSize};
		SDL_FRect dst = {destination.x + offset, destination.y, (float)width, (float)atlas->fontSize};

		SDL_RenderTexture(renderer, atlas->glyphTexture, &src, &dst);
		offset += (float)width;
	}
}

void SDL_GlyphAtlasANSCIIFree(SDL_GlyphAtlasANSCII *atlas)
{
	SDL_DestroyTexture(atlas->glyphTexture);
	atlas->fontSize = 0;
	atlas->textureWidth = 0;
	SDL_zero(atlas->strides);
}
