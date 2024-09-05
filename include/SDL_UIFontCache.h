//
// Created by David Naussed on 2024-01-01.
//

#include <SDL3_ttf/SDL_ttf.h>

#ifndef SDL_UIFONTCACHE_IMPLEMENTATION_H
#define SDL_UIFONTCACHE_IMPLEMENTATION_H

struct FontCacheEntryConfig
{
	const char *path;
	int size;
};

// Ref-counted or make the caches arena styled, baby
struct FontCacheEntry
{
	const char *key;
	TTF_Font *font;
	int size;
};

// No need to GC so far - maybe we load one or two - Maybe 5. No biggie
struct FontCache
{
	FontCacheEntry *data;
	Uint32 count;
	Uint32 capacity;
};

void FontCacheAlloc(FontCache *cache, Uint32 capacity);
TTF_Font *FontCacheGet(FontCache *cache, FontCacheEntryConfig const *fontCacheEntryConfig);
void FontCacheFree(FontCache *cache);

#endif // SDL_UIFONTCACHE_IMPLEMENTATION_H
