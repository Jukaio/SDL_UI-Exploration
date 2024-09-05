//
// Created by David Naussed on 2023-12-24.
//

#include "SDL_UI_MemoryList.h"

#include <HighestBitDeBruijn.h>
#include <SDL_assert.h>
#include <stdlib.h>
#include <string.h>

void SDL_UIMemoryListAlloc(SDL_UIMemoryList *memoryList, Uint32 capacity)
{
	memoryList->memory = (Uint8 *)malloc(capacity);
	memoryList->capacity = capacity;
	memoryList->count = 0;
}

void SDL_UIMemoryListRealloc(SDL_UIMemoryList *memoryList)
{
	Uint32 newCapacity = 1 << (HighestSetBitDeBruijn32(memoryList->capacity) + 1);
	SDL_assert(newCapacity > 0 && "New Capacity is not allowed to be 0");
	SDL_assert(newCapacity > memoryList->capacity && "New capacity must be bigger than old capacity");

	memoryList->memory = (Uint8 *)realloc(memoryList->memory, newCapacity);
	SDL_assert(memoryList->memory != NULL && "Reallocation of stack failed!");
	memoryList->capacity = newCapacity;
}

void SDL_UIMemoryListFree(SDL_UIMemoryList *memoryList)
{
	free(memoryList->memory);
	memoryList->capacity = 0;
	memoryList->count = 0;
}

void SDL_UIMemoryListAppend(SDL_UIMemoryList *memoryList, const void *data, Uint32 size)
{
	const Uint32 nextCount = memoryList->count + size;
	// Just in case - While this bad boy
	while (nextCount >= memoryList->capacity)
	{
		SDL_UIMemoryListRealloc(memoryList);
	}

	memcpy(&memoryList->memory[memoryList->count], data, size);
	memoryList->count = nextCount;
}

void SDL_UIMemoryListErase(SDL_UIMemoryList *memoryList, Uint32 size)
{
	SDL_assert(size >= memoryList->count);
	const Uint32 nextCount = memoryList->count - size;
	memoryList->count = nextCount;
}

bool SDL_UIMemoryListTryGet(SDL_UIMemoryList *memoryList, Uint32 index, void **data)
{
	if (index >= memoryList->count)
	{
		return false;
	}

	*data = (void *)&memoryList->memory[index];
	return true;
}

void SDL_UIMemoryListClear(SDL_UIMemoryList *memoryList)
{
	memoryList->count = 0;
}
