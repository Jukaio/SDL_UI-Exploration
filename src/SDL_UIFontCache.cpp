//
// Created by David Naussed on 2024-01-01.
//

#include "SDL_UIFontCache.h"
#include <stdlib.h>
#include <string.h>

void FontCacheAlloc(FontCache *cache, Uint32 capacity)
{
	SDL_assert(capacity > 0 && "Cannot have capacity smaller or equal than zero for cache");
	cache->data		= (FontCacheEntry *)calloc(capacity, sizeof(FontCacheEntry));
	cache->count	= 0;
	cache->capacity = capacity;
}

TTF_Font *FontCacheGet(FontCache *cache, FontCacheEntryConfig const *fontCacheEntryConfig)
{
	FontCacheEntry *data = cache->data;
	int count			 = cache->count;

	const char *path = fontCacheEntryConfig->path;
	int size		 = fontCacheEntryConfig->size;

	for (int i = 0; i < count; i++)
	{
		FontCacheEntry *entry = &data[i];
		if (entry->size == size && !strcmp(entry->key, path))
		{
			return entry->font;
		}
	}

	if (cache->count >= cache->capacity)
	{
		// TODO: Make it realloc
		const int newCapacity = cache->capacity * 2;
		FontCache newCache;
		FontCacheAlloc(&newCache, newCapacity);
		memcpy(newCache.data, cache->data, cache->count);
		newCache.capacity = newCapacity;
		newCache.count	  = cache->count;

		free(cache->data);

		*cache = newCache;

		SDL_assert(cache && cache->data && "Cache is valid after reallocation");
	}

	TTF_Font *font = TTF_OpenFont(path, size);
	FontCacheEntry entry;
	entry.font = font;
	entry.key  = path;
	entry.size = size;

	int index		   = cache->count;
	cache->data[index] = entry;

	cache->count += 1;
	return font;
}

void FontCacheFree(FontCache *cache)
{
	FontCacheEntry *data = cache->data;
	int count			 = cache->count;

	for (int i = 0; i < count; i++)
	{
		FontCacheEntry *entry = &data[i];

		TTF_CloseFont(entry->font);

		entry->key	= NULL;
		entry->font = NULL;
	}

	SDL_zero(*cache);

	free(data);
}
