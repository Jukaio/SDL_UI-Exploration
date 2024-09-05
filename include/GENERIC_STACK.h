//
// Created by David Naussed on 2023-12-23.
//

#ifndef GENERIC_STACK_H
#define GENERIC_STACK_H

#include "HighestBitDeBruijn.h"
#include <SDL3/SDL_assert.h>

#define GENERIC_STACK_TYPE(NAME, TYPE) \
struct SDL_##NAME##Stack \
{ \
	TYPE* data; \
	Uint32 count; \
	Uint32 capacity; \
};
#define GENERIC_STACK_FUNCTIONS(NAME, TYPE) \
inline void SDL_##NAME##StackAlloc(SDL_##NAME##Stack* stack, Uint32 capacity) \
{ \
	stack->data = (TYPE*)calloc(capacity, sizeof(TYPE)); \
	stack->count = 0; \
	stack->capacity = capacity; \
} \
\
inline void SDL_##NAME##StackRealloc(SDL_##NAME##Stack* stack) \
{ \
	const Uint32 newCapacity = 1 << (HighestSetBitDeBruijn32(stack->capacity) + 1); \
	SDL_assert(newCapacity > 0 && "New Capacity is not allowed to be 0"); \
	SDL_assert(newCapacity > stack->capacity && "New capacity must be bigger than old capacity"); \
	\
	stack->data = (TYPE*)realloc(stack->data, newCapacity * sizeof(TYPE)); \
	SDL_assert(stack->data != NULL && "Reallocation of stack failed!"); \
	stack->capacity = newCapacity; \
} \
\
inline void SDL_##NAME##StackPush(SDL_##NAME##Stack* stack, TYPE state) \
{ \
	if(stack->count >= stack->capacity) \
	{ \
		SDL_##NAME##StackRealloc(stack); \
	} \
\
	stack->data[stack->count] = state; \
	stack->count++; \
} \
\
inline bool SDL_##NAME##StackTryPeek(SDL_##NAME##Stack const* stack, TYPE* state) \
{ \
	if(stack->count == 0) \
	{ \
		return false; \
	} \
	Uint32 last = stack->count - 1; \
	*state = stack->data[last]; \
	return true; \
} \
\
inline bool SDL_##NAME##StackTryPeekMemory(SDL_##NAME##Stack const* stack, TYPE** state) \
{ \
	if(stack->count == 0) \
	{ \
		return false; \
	} \
	Uint32 last = stack->count - 1; \
	*state = &stack->data[last]; \
	return true; \
} \
\
inline bool SDL_##NAME##StackPop(SDL_##NAME##Stack* stack) \
{ \
	if(stack->count == 0) \
	{ \
		return false; \
	} \
	stack->count--; \
	return true; \
} \
\
inline void SDL_##NAME##StackClear(SDL_##NAME##Stack* stack) \
{ \
	stack->count = 0; \
} \
\
inline void SDL_##NAME##StackFree(SDL_##NAME##Stack* stack) \
{ \
	free(stack->data); \
	stack->count = 0; \
	stack->capacity = 0; \
}
#endif //GENERIC_STACK_H
