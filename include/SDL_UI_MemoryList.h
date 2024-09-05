//
// Created by David Naussed on 2023-12-24.
//

#ifndef SDL_UI_MEMORYLIST_IMPLEMENTATION_H
#define SDL_UI_MEMORYLIST_IMPLEMENTATION_H

#include <SDL3/SDL_stdinc.h>

struct SDL_UIMemoryList
{
	Uint8 *memory;
	Uint32 count;
	Uint32 capacity;
};

void SDL_UIMemoryListAlloc(SDL_UIMemoryList *memoryList, Uint32 capacity);
void SDL_UIMemoryListRealloc(SDL_UIMemoryList *memoryList);
void SDL_UIMemoryListFree(SDL_UIMemoryList *memoryList);

void SDL_UIMemoryListAppend(SDL_UIMemoryList *memoryList, const void *memory, Uint32 size);
void SDL_UIMemoryListErase(SDL_UIMemoryList *memoryList, Uint32 size);
bool SDL_UIMemoryListTryGet(SDL_UIMemoryList *memoryList, Uint32 index, void **data);
void SDL_UIMemoryListClear(SDL_UIMemoryList *memoryList);


#endif //SDL_UI_MEMORYLIST_IMPLEMENTATION_H
