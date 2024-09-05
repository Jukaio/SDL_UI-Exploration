//
// Created by David Naussed on 2024-01-01.
//

#ifndef SDL_GLYPHATLASANSCII_IMPLEMENTATION_H
#define SDL_GLYPHATLASANSCII_IMPLEMENTATION_H

#include <SDL_pixels.h>
#include <SDL_rect.h>

struct SDL_Texture;
struct SDL_Renderer;

typedef struct _TTF_Font TTF_Font;

struct SDL_GlyphAtlasANSCII
{
	SDL_Texture *glyphTexture;
	// Whoever thinks having a texture bigger than 1 << 16 in height, truly is a twisted asshole
	Uint16 fontSize;
	Uint16 textureWidth;
	// Let's pray this is the only meta data we need
	Uint16 strides[127];
};

bool SDL_GlyphAtlasANSCIIMeasureText(SDL_GlyphAtlasANSCII *atlas, const char *str, int *w, int *h);

void SDL_GlyphAtlasANSCIICreate(SDL_Renderer *renderer, SDL_GlyphAtlasANSCII *atlas, _TTF_Font *font, SDL_Color color);

void SDL_GlyphAtlasANSCIIRenderText(SDL_Renderer *renderer, SDL_GlyphAtlasANSCII const *atlas, SDL_FRect destination,
									SDL_Color color, const char *str);

void SDL_GlyphAtlasANSCIIFree(SDL_GlyphAtlasANSCII *atlas);

#endif //SDL_GLYPHATLASANSCII_IMPLEMENTATION_H
