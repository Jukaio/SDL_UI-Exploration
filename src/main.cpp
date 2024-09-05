#include "SDL_UI.h"

#include <SDL3_image/SDL_image.h>
#include <SDL3_net/SDL_net.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string.h>
#include <unistd.h>

#include "SDL_GlyphAtlasANSCII.h"
#include "SDL_UIFontCache.h"
#include "SDL_UISpriteAtlas.h"

struct Core_Input
{
	Uint8 keyboardStateCurrent[SDL_NUM_SCANCODES];
	Uint8 keyboardStatePrevious[SDL_NUM_SCANCODES];

	Uint32 mouseStateCurrent;
	Uint32 mouseStatePrevious;

	SDL_FPoint mousePositionPrevious;
	SDL_FPoint mousePositionCurrent;
};

void Core_InputAlloc(Core_Input *input)
{
	SDL_assert(input != NULL && "Input argument cannot be NULL");
	// Idk...
	SDL_zerop(input);
}

void Core_InputUpdate(Core_Input *input)
{
	SDL_memcpy(input->keyboardStatePrevious, input->keyboardStateCurrent, sizeof(input->keyboardStateCurrent));
	int keyNums;
	const Uint8 *currentInternalKeyboardState = SDL_GetKeyboardState(&keyNums);
	SDL_memcpy(input->keyboardStateCurrent, currentInternalKeyboardState, keyNums);

	input->mouseStatePrevious	 = input->mouseStateCurrent;
	input->mousePositionPrevious = input->mousePositionCurrent;
	input->mouseStateCurrent	 = SDL_GetMouseState(&input->mousePositionCurrent.x, &input->mousePositionCurrent.y);
}

bool Core_KeyboardKeyIsPressed(const Core_Input *input, const SDL_Scancode scancode)
{
	return input->keyboardStateCurrent[scancode];
}

bool Core_KeyboardKeyIsReleased(const Core_Input *input, const SDL_Scancode scancode)
{
	return !input->keyboardStateCurrent[scancode];
}

bool Core_KeyboardKeyJustPressed(const Core_Input *input, const SDL_Scancode scancode)
{
	return input->keyboardStateCurrent[scancode] && !input->keyboardStatePrevious[scancode];
}

bool Core_KeyboardKeyJustReleased(const Core_Input *input, const SDL_Scancode scancode)
{
	return !input->keyboardStateCurrent[scancode] && input->keyboardStatePrevious[scancode];
}

bool Core_MouseButtonIsPressed(const Core_Input *input, const int button)
{
	return input->mouseStateCurrent & SDL_BUTTON(button) == SDL_BUTTON(button);
}

bool Core_MouseButtonIsReleased(const Core_Input *input, const int button)
{
	return input->mouseStateCurrent & SDL_BUTTON(button) != SDL_BUTTON(button);
}

bool Core_MouseButtonJustPressed(const Core_Input *input, const int button)
{
	bool isDown	 = input->mouseStateCurrent & SDL_BUTTON(button) == SDL_BUTTON(button);
	bool wasDown = input->mouseStatePrevious & SDL_BUTTON(button) == SDL_BUTTON(button);

	return isDown && !wasDown;
}

bool Core_MouseButtonJustReleased(const Core_Input *input, const int button)
{
	bool isDown	 = input->mouseStateCurrent & SDL_BUTTON(button) == SDL_BUTTON(button);
	bool wasDown = input->mouseStatePrevious & SDL_BUTTON(button) == SDL_BUTTON(button);

	return !isDown && wasDown;
}

SDL_FPoint Core_MouseGetPreviousPosition(const Core_Input *input)
{
	return (SDL_FPoint){input->mousePositionPrevious.x, input->mousePositionPrevious.y};
}

SDL_FPoint Core_MouseGetCurrentPosition(const Core_Input *input)
{
	return (SDL_FPoint){input->mousePositionCurrent.x, input->mousePositionCurrent.y};
}

SDL_FPoint Core_MouseGetDelta(const Core_Input *input)
{
	return (SDL_FPoint){
	input->mousePositionCurrent.x - input->mousePositionPrevious.x, input->mousePositionCurrent.y - input->mousePositionPrevious.y};
}

// Global input state - will fall flat for multiple windows :(
Core_Input input;
#define KEY_IS_PRESSED(scancode) Core_KeyboardKeyIsPressed(&input, scancode)
#define KEY_IS_RELEASED(scancode) Core_KeyboardKeyIsReleased(&input, scancode)
#define KEY_JUST_PRESSED(scancode) Core_KeyboardKeyJustPressed(&input, scancode)
#define KEY_JUST_RELEASED(scancode) Core_KeyboardKeyJustReleased(&input, scancode)

#define MOUSE_IS_PRESSED(button) Core_MouseButtonIsPressed(&input, button)
#define MOUSE_IS_RELEASED(button) Core_MouseButtonIsReleased(&input, button)
#define MOUSE_JUST_PRESSED(button) Core_MouseButtonJustPressed(&input, button)
#define MOUSE_JUST_RELEASED(button) Core_MouseButtonJustReleased(&input, button)

void SDL_UIRender(SDL_Renderer *renderer, SDL_UIContext *uictx)
{
	SDL_FRect currentRect;
	SDL_UICommandType *command;
	SDL_UIContextIterator it = SDL_UIContextGetIterator(uictx);
	while (SDL_UIContextIterate(uictx, &it, &command))
	{
		const SDL_UICommandType type = *command;
		switch (type)
		{
			case SDL_UI_COMMAND_TYPE_HEADER:
				{
					const SDL_UICommandHeader *header = (SDL_UICommandHeader *)command;
					currentRect						  = header->rect;
				}
				break;
			case SDL_UI_COMMAND_TYPE_OUTLINE:
				{
					const SDL_UICommandOutline *rectCommand = (SDL_UICommandOutline *)command;
					const SDL_Color color					= rectCommand->color;
					SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
					SDL_RenderRect(renderer, &currentRect);
				}
				break;
			case SDL_UI_COMMAND_TYPE_RECT:
				{
					const SDL_UICommandRect *rectCommand = (SDL_UICommandRect *)command;
					const SDL_Color color				 = rectCommand->color;
					SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
					SDL_RenderFillRect(renderer, &currentRect);
				}
				break;
			case SDL_UI_COMMAND_TYPE_LABEL:
				{
					const SDL_UICommandLabel *label	  = (SDL_UICommandLabel *)command;
					const SDL_GlyphAtlasANSCII *atlas = (SDL_GlyphAtlasANSCII *)label->font;
					SDL_GlyphAtlasANSCIIRenderText(renderer, atlas, label->rect, label->color, label->text);
				}
				break;
			case SDL_UI_COMMAND_TYPE_IMAGE:
				{
					const SDL_UICommandImage *image = (SDL_UICommandImage *)command;
					const SDL_UISprite *sprite		= (SDL_UISprite *)image->image;
					const SDL_Color color			= image->color;
					SDL_UISpriteAtlasSetColor(sprite->atlas, color);
					SDL_UISpriteAtlasRender(renderer, sprite, image->rect);
				}
				break;
		}
	}
}

bool SDL_UIImageMeasureImpl(SDL_UIImage image, int *w, int *h)
{
	// Maybe introduce getter... idk
	SDL_assert(image != NULL && "Image is null");
	SDL_UISprite *sprite = (SDL_UISprite *)image;
	SDL_assert(sprite->atlas->texture != NULL && "No texture in sprite atlas");

	const SDL_FRect *rect = &sprite->atlas->rects[sprite->index];
	*w					  = (int)rect->w;
	*h					  = (int)rect->h;
	return true;
}

bool SDL_UIMeasureTextImpl(SDL_UIFont font, const char *str, int *w, int *h)
{
	return SDL_GlyphAtlasANSCIIMeasureText((SDL_GlyphAtlasANSCII *)font, str, w, h);
}

SDL_FRect Game_FPointsToFRect(SDL_FRect pos, SDL_FRect size)
{
	return (SDL_FRect){pos.x, pos.y, size.x, size.y};
}

struct Core_Transform
{
	SDL_FPoint position;
	SDL_FPoint size;
};

// Initialise Core Resources
SDL_Window *clientWindow = NULL;
SDL_Renderer *renderer	 = NULL;

SDL_GlyphAtlasANSCII glyphAtlas;
FontCache fontCache;
SDL_UIContext uictx;

SDL_UISpriteAtlas prototypeButtonSpriteAtlas;

SDL_Cursor *cursors[SDL_NUM_SYSTEM_CURSORS];

SDL_Color green	 = {0, 255, 0, 255};
SDL_Color blue	 = {0, 0, 255, 255};
SDL_Color yellow = {255, 255, 0, 255};
SDL_Color red	 = {255, 0, 0, 255};
SDL_Color black	 = {0, 0, 0, 255};
SDL_Color white	 = {255, 255, 255, 255};
SDL_Color grey	 = {127, 127, 127, 255};
SDL_Color clear	 = {0, 0, 0, 0};

bool editorPanelToggle = true;
bool isEditMode		   = true;
bool isPaused		   = true;

SDL_UISprite greyProto;
SDL_UISprite greenSprite;
SDL_UISprite pinkSprite;
SDL_UISprite blueSprite;

bool Core_ShouldExecutePlayMode()
{
	return !isEditMode && !isPaused;
}

void Editor_DrawUI(SDL_Window *clientWindow, SDL_UIContext *uictx, SDL_GlyphAtlasANSCII *glyphAtlas)
{
	int windowWidth;
	int windowHeight;
	SDL_GetWindowSize(clientWindow, &windowWidth, &windowHeight);
	SDL_UIBegin(uictx, SDL_FRectCreate(0.0f, 0.0f, (float)windowWidth, (float)windowHeight));
	{
		SDL_UIFontBegin(uictx, (SDL_UIFont *)glyphAtlas);
		SDL_UIFontColorBegin(uictx, blue);
		SDL_UIHoverTextColorBegin(uictx, green);
		SDL_UITextVerticalAlignmentBegin(uictx, SDL_UI_VERTICAL_ALIGNMENT_CENTER);
		SDL_UITextHorizontalAlignmentBegin(uictx, SDL_UI_HORIZONTAL_ALIGNMENT_CENTER);
		SDL_UITextPaddingBegin(uictx, (SDL_UIPadding){1.0f, 1.0f, 1.0f, 1.0f});
		SDL_UIOutlineColorBegin(uictx, black);
		SDL_UIHoverOutlineColorBegin(uictx, red);
		SDL_UIHoverColorBegin(uictx, yellow);
		SDL_UIMeasureModeBegin(uictx, SDL_UI_MEASURE_MODE_RELATIVE_TO_WINDOW);
		SDL_UIMainColorBegin(uictx, red);

		SDL_UIMeasureModeBegin(uictx, SDL_UI_MEASURE_MODE_ABSOLUTE);
		SDL_UICutModeBegin(uictx, SDL_UI_CUT_MODE_LEFT);

		SDL_UIGroupBegin(uictx, 128.0f);

		SDL_UICutModeBegin(uictx, SDL_UI_CUT_MODE_TOP);
		SDL_UITextureModeBegin(uictx, SDL_UI_TEXTURE_MODE_PRESERVE_ASPECT);

		SDL_UIImageBegin(uictx, !editorPanelToggle ? &pinkSprite : &greenSprite);
		SDL_UIIHoverImageBegin(uictx, &blueSprite);
		SDL_UIImageColorBegin(uictx, white);
		SDL_UIHoverImageColorBegin(uictx, white);

		SDL_UIMainColorBegin(uictx, editorPanelToggle ? green : grey);
		if (SDL_UILabelButton(uictx, editorPanelToggle ? "v" : "^", 18.0f))
		{
			editorPanelToggle = !editorPanelToggle;
		}
		SDL_UIMainColorEnd(uictx);
		if (editorPanelToggle)
		{
			if (SDL_UILabelButton(uictx, "Play", 64.0f))
			{
				isEditMode = false;
			}
			if (SDL_UILabelButton(uictx, "Edit", 64.0f))
			{
				isEditMode = true;
			}
		}

		SDL_UIGroupEnd(uictx);
		SDL_UICutModeEnd(uictx);
		SDL_UICutModeEnd(uictx);
		SDL_UIMeasureModeEnd(uictx);

		SDL_UIMeasureModeBegin(uictx, SDL_UI_MEASURE_MODE_ABSOLUTE);
		SDL_UICutModeBegin(uictx, SDL_UI_CUT_MODE_RIGHT);
		SDL_UIGroupBegin(uictx, 128.0f);

		SDL_UICutModeBegin(uictx, SDL_UI_CUT_MODE_TOP);

		SDL_UILabel(uictx, isEditMode ? "Edit Mode" : "Play Mode", 24.0f);

		if (!isEditMode)
		{
			if (SDL_UILabelButton(uictx, isPaused ? "Play" : "Pause", 24.0f))
			{
				isPaused = !isPaused;
			}
			if (!isEditMode && SDL_UILabelButton(uictx, "Reset", 24.0f))
			{
			}
		}

		SDL_UIGroupEnd(uictx);
		SDL_UICutModeEnd(uictx);
		SDL_UICutModeEnd(uictx);
		SDL_UIMeasureModeEnd(uictx);
	}
	SDL_UIEnd(uictx);
}

float tickAccumulator;
const float TICK_FREQUENCY = 1.0f / 60.0f;

typedef size_t Core_SparseId;
typedef size_t Core_DenseId;

typedef Core_SparseId Core_EntityId;

const Core_EntityId INVALID_ENTITY = ~0;

struct Core_Entity
{
	Core_SparseId id;

	Core_Transform transform;
};

struct Core_DenseIterator
{
	size_t current;
	size_t count;
};

struct Core_EntityContainer
{
	Core_DenseId *sparse;
	Core_Entity *dense;

	Core_SparseId freeList;

	size_t sparseCount;
	size_t denseCount;

	size_t capacity;
};

inline Core_DenseIterator Core_EntityGetIterator(const Core_EntityContainer *container)
{
	return (Core_DenseIterator){0, container->denseCount};
}

inline bool Core_EntityIterate(const Core_EntityContainer *container, Core_DenseIterator *iterator, Core_Entity **outEntity)
{
	SDL_assert(container != NULL && "No data to iterate over");
	SDL_assert(iterator != NULL && "Cannot iterate without iterator");
	SDL_assert(outEntity != NULL && "Out entity parameter must be non-null");

	if (iterator->current >= iterator->count)
	{
		return false;
	}
	*outEntity = &container->dense[iterator->current];
	iterator->current++;
	return true;
}

bool Core_EntityIdIsValid(const Core_DenseId id)
{
	return id != INVALID_ENTITY;
}

void Core_EntityContainerAlloc(Core_EntityContainer *entityContainer, const size_t capacity)
{
	SDL_assert(entityContainer != NULL && "Entity container cannot be not NULL!");
	entityContainer->sparse = (Core_DenseId *)SDL_calloc(capacity, sizeof(Core_DenseId));
	entityContainer->dense	= (Core_Entity *)SDL_calloc(capacity, sizeof(Core_Entity));

	SDL_memset(entityContainer->sparse, ~0, sizeof(Core_DenseId) * capacity);

	entityContainer->capacity	 = capacity;
	entityContainer->sparseCount = 0;
	entityContainer->denseCount	 = 0;
	entityContainer->freeList	 = INVALID_ENTITY;
}

void Core_EntityContainerRealloc(Core_EntityContainer *entityContainer, const size_t newCapacity)
{
	SDL_assert(entityContainer != NULL && "Entity container cannot be not NULL!");

	const size_t oldCapacity  = entityContainer->capacity;
	entityContainer->sparse	  = (Core_DenseId *)SDL_realloc(entityContainer->sparse, newCapacity * sizeof(Core_DenseId));
	entityContainer->dense	  = (Core_Entity *)SDL_realloc(entityContainer->dense, newCapacity * sizeof(Core_Entity));
	entityContainer->capacity = newCapacity;

	const size_t difference = newCapacity - oldCapacity;
	SDL_memset(entityContainer->sparse + oldCapacity, ~0, sizeof(Core_DenseId) * difference);
	SDL_memset(entityContainer->dense + oldCapacity, 0, sizeof(Core_Entity) * difference);
}

Core_Entity *Core_EntityGet(Core_EntityContainer *entityContainer, const Core_EntityId id)
{
	SDL_assert(Core_EntityIdIsValid(id) && "Should not pass in invalid entity to destroy");

	const Core_DenseId denseIndex = entityContainer->sparse[id];

	return &entityContainer->dense[denseIndex];
}

void Core_EntityDestroy(Core_EntityContainer *entityContainer, const Core_EntityId id)
{
	SDL_assert(Core_EntityIdIsValid(id) && "Should not pass in invalid entity to destroy");

	const Core_DenseId denseIndex	  = entityContainer->sparse[id];
	const Core_DenseId lastDenseIndex = entityContainer->denseCount - 1;

	Core_Entity *lastElement = &entityContainer->dense[lastDenseIndex];

	// effectively remove id, by re-using its slow as free list
	entityContainer->sparse[id] = entityContainer->freeList;
	entityContainer->freeList	= id;

	entityContainer->sparse[lastElement->id] = denseIndex;
	entityContainer->dense[denseIndex]		 = entityContainer->dense[lastDenseIndex];

	entityContainer->denseCount--;
}

void Core_EntityCreate(Core_EntityContainer *entityContainer, const Core_Transform transform)
{
	SDL_assert(entityContainer != NULL && "Entity container cannot be not NULL!");

	if (Core_EntityIdIsValid(entityContainer->freeList))
	{
		const Core_DenseId last = entityContainer->denseCount;
		Core_Entity *entity		= &entityContainer->dense[last];
		entity->id				= entityContainer->freeList;
		entity->transform		= transform;

		const Core_SparseId freeSlotIndex	  = entityContainer->freeList;
		Core_SparseId *sparseSlot			  = &entityContainer->sparse[freeSlotIndex];
		const Core_SparseId nextFreeSlotIndex = *sparseSlot;
		*sparseSlot							  = last;

		entityContainer->freeList = nextFreeSlotIndex;

		entityContainer->denseCount += 1;
	}
	else
	{
		if (entityContainer->sparseCount >= entityContainer->capacity)
		{
			Core_EntityContainerRealloc(entityContainer, entityContainer->capacity * 2);
		}

		SDL_assert(
		entityContainer->denseCount == entityContainer->sparseCount &&
		"Trivial entity creation shall only happen if sparse and dense counts are synced");

		const Core_SparseId sparseIndex		 = entityContainer->sparseCount;
		const Core_DenseId denseIndex		 = entityContainer->denseCount;
		entityContainer->sparse[sparseIndex] = denseIndex;

		Core_Entity *entity = &entityContainer->dense[denseIndex];
		entity->id			= sparseIndex;
		entity->transform	= transform;

		entityContainer->sparseCount += 1;
		entityContainer->denseCount += 1;
	}
}

Core_EntityContainer entityContainer;

void Game_FixedUpdates(float deltaTime)
{
	tickAccumulator += deltaTime;
	while (tickAccumulator >= TICK_FREQUENCY)
	{
		// consistent frequency tick
		tickAccumulator -= TICK_FREQUENCY;
/*
		SDL_FPoint direction = (SDL_FPoint){0.0f, 0.0f};
		if (KEY_IS_PRESSED(SDL_SCANCODE_A))
		{
			direction.x -= 1.0f;
		}
		if (KEY_IS_PRESSED(SDL_SCANCODE_D))
		{
			direction.x += 1.0f;
		}
		if (KEY_IS_PRESSED(SDL_SCANCODE_W))
		{
			direction.y -= 1.0f;
		}
		if (KEY_IS_PRESSED(SDL_SCANCODE_S))
		{
			direction.y += 1.0f;
		}
		float length = (direction.x * direction.x) + (direction.y * direction.y);
		if (length > 0.0f)
		{
			float unitLength = SDL_sqrtf(length);
			direction.x /= unitLength;
			direction.y /= unitLength;
		}
		float speed = 8.0f;
		player.position.x += direction.x * speed;
		player.position.y += direction.y * speed;*/
	}
}

Core_SparseId selectedEntity = INVALID_ENTITY;

void Editor_Update(float deltaTime)
{
	SDL_FPoint previousMousePosition = Core_MouseGetPreviousPosition(&input);
	SDL_FPoint currentMousePosition	 = Core_MouseGetCurrentPosition(&input);

	if (MOUSE_JUST_PRESSED(SDL_BUTTON_LEFT))
	{
		Core_Entity *entity;
		for (Core_DenseIterator it = Core_EntityGetIterator(&entityContainer); Core_EntityIterate(&entityContainer, &it, &entity);)
		{
			const Core_Transform *transform = &entity->transform;

			SDL_FRect playerRect = {transform->position.x, transform->position.y, transform->size.x, transform->size.y};

			bool shouldDrag = currentMousePosition.x > playerRect.x && currentMousePosition.y > playerRect.y &&
			currentMousePosition.x < playerRect.x + playerRect.w && currentMousePosition.y < playerRect.y + playerRect.h;

			bool shouldSillDrag = previousMousePosition.x > playerRect.x && previousMousePosition.y > playerRect.y &&
			previousMousePosition.x < playerRect.x + playerRect.w && previousMousePosition.y < playerRect.y + playerRect.h;
			if (shouldDrag || shouldSillDrag)
			{
				selectedEntity = entity->id;
				break;
			}
		}
	}

	{
		if (!Core_EntityIdIsValid(selectedEntity))
		{
			return;
		}

		Core_Entity *entity		  = Core_EntityGet(&entityContainer, selectedEntity);
		Core_Transform *transform = &entity->transform;

		bool isHoldingLeftClick = MOUSE_IS_PRESSED(SDL_BUTTON_LEFT);
		SDL_FRect playerRect	= {transform->position.x, transform->position.y, transform->size.x, transform->size.y};

		bool shouldDrag = isHoldingLeftClick && currentMousePosition.x > playerRect.x && currentMousePosition.y > playerRect.y &&
		currentMousePosition.x < playerRect.x + playerRect.w && currentMousePosition.y < playerRect.y + playerRect.h;

		bool shouldSillDrag = isHoldingLeftClick && previousMousePosition.x > playerRect.x && previousMousePosition.y > playerRect.y &&
		previousMousePosition.x < playerRect.x + playerRect.w && previousMousePosition.y < playerRect.y + playerRect.h;

		if (shouldDrag || shouldSillDrag)
		{
			SDL_FPoint target = currentMousePosition;
			target.x -= playerRect.w * 0.5f;
			target.y -= playerRect.h * 0.5f;

			transform->position = target;
		}
	}
}
void Core_Run(SDL_Renderer *renderTarget, SDL_Window *window)
{
	// Loop
	Uint64 tp	   = SDL_GetTicksNS();
	bool isRunning = true;

	while (isRunning)
	{
		Uint64 nextTp	  = SDL_GetTicksNS();
		Uint64 difference = nextTp - tp;
		float deltaTime	  = (float)difference / SDL_NS_PER_SECOND;

		tp = nextTp;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_EVENT_QUIT:
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED: isRunning = false; break;
			}
			// Handle Editor input
			// HandleMicroUiInput(&mctx, &e);

			// Handle GUI Input

			// Handle Game Input
		}
		Core_InputUpdate(&input);

		SDL_UIInputUpdate(&uictx);

		// Early Update
		Editor_DrawUI(window, &uictx, &glyphAtlas);

		if (Core_ShouldExecutePlayMode())
		{
			// Dynamic Update BRRRRR
			Game_FixedUpdates(deltaTime);
		}
		else
		{
			Editor_Update(deltaTime);
		}
		// Late Update

		// Draw
		SDL_SetRenderDrawColor(renderTarget, 0, 0, 0, 0);
		SDL_RenderClear(renderTarget);

		Core_Entity *entity;
		for (Core_DenseIterator it = Core_EntityGetIterator(&entityContainer); Core_EntityIterate(&entityContainer, &it, &entity);)
		{
			const Core_Transform *transform = &entity->transform;
			SDL_SetRenderDrawColor(renderTarget, 0, 0, 255, 255);
			SDL_FRect dstRect = {transform->position.x, transform->position.y, transform->size.x, transform->size.y};
			SDL_RenderFillRect(renderTarget, &dstRect);
		}

		SDL_UIRender(renderTarget, &uictx);

		// Rest of dt is VSYNC
		SDL_RenderPresent(renderTarget);
	}
}
int main(int argc, char *argv[])
{
	// Initialise Core Systems
	SDL_InitSubSystem(~0U);
	TTF_Init();
	SDLNet_Init();

	// Initialise Core Resources
	SDL_CreateWindowAndRenderer(820, 640, SDL_WINDOW_RESIZABLE, &clientWindow, &renderer);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	Core_InputAlloc(&input);

	// Load cursors
	for (int i = 0; i < SDL_NUM_SYSTEM_CURSORS; i++)
	{
		cursors[i] = SDL_CreateSystemCursor((SDL_SystemCursor)i);
	}
	SDL_SetCursor(cursors[SDL_SYSTEM_CURSOR_ARROW]);

	int w;
	int h;
	SDL_RendererLogicalPresentation logicalPresentation;
	SDL_ScaleMode scaleMode;

	SDL_GetRenderLogicalPresentation(renderer, &w, &h, &logicalPresentation, &scaleMode);
	SDL_SetWindowPosition(clientWindow, 0, 0);
	SDL_SetWindowSize(clientWindow, w, h);

	FontCacheAlloc(&fontCache, 8);

	FontCacheEntryConfig fontConfig;
	SDL_zero(fontConfig);
	fontConfig.path = "./assets/PressStart2P.ttf";
	fontConfig.size = 12;

	TTF_Font *font = FontCacheGet(&fontCache, &fontConfig);

	SDL_Color glyphWhiteColor = {255, 255, 255, 255};
	SDL_GlyphAtlasANSCIICreate(renderer, &glyphAtlas, font, glyphWhiteColor);
	SDL_Texture *texture = IMG_LoadTexture(renderer, "./assets/prototype.png");

	SDL_UISpriteAtlasAlloc(&prototypeButtonSpriteAtlas, texture);
	greyProto	= SDL_UISpriteAtlasCreate(&prototypeButtonSpriteAtlas, (SDL_FRect){0.0f, 0.0f, 32.0f, 32.0f});
	greenSprite = SDL_UISpriteAtlasCreate(&prototypeButtonSpriteAtlas, (SDL_FRect){48.0f, 0.0f, 32.0f, 32.0f});
	pinkSprite	= SDL_UISpriteAtlasCreate(&prototypeButtonSpriteAtlas, (SDL_FRect){48.0f, 32.0f, 32.0f, 32.0f});
	blueSprite	= SDL_UISpriteAtlasCreate(&prototypeButtonSpriteAtlas, (SDL_FRect){0.0f, 32.0f, 32.0f, 32.0f});

	SDL_UIContextAlloc(&uictx, SDL_UIMeasureTextImpl, SDL_UIImageMeasureImpl);

	Core_EntityContainerAlloc(&entityContainer, 2);

	if (access("entity_save.txt", F_OK) == 0)
	{
		FILE *file = fopen("entity_save.txt", "r");

		fseek(file, 0, SEEK_SET);

		Core_Entity loadedEntity;
		while (fread(&loadedEntity, sizeof(loadedEntity), 1, file))
		{
			Core_EntityCreate(&entityContainer, loadedEntity.transform);
		}

		fclose(file);
		/*for (Core_EntityDenseIterator it = Core_EntityGetIterator(&entityContainer); Core_EntityIterate(&entityContainer, &it, &entity);)
		{
			printf("Created Entity with Id: %lu\n", entity->id);
		}*/
	}
	else
	{
		Core_EntityCreate(&entityContainer, (Core_Transform){(SDL_FPoint){40, 140}, (SDL_FPoint){20, 20}});
		Core_EntityCreate(&entityContainer, (Core_Transform){(SDL_FPoint){120, 200}, (SDL_FPoint){20, 20}});

		Core_EntityCreate(&entityContainer, (Core_Transform){(SDL_FPoint){40, 280}, (SDL_FPoint){20, 20}});
	}

	// ============================ //
	// ========= RUN LOOP ========= //
	// ============================ //
	Core_Run(renderer, clientWindow);
	// ============================ //
	// ============================ //
	// ============================ //

	FILE *file = fopen("entity_save.txt", "w");

	fseek(file, 0, SEEK_SET);

	Core_Entity *entity;
	for (Core_DenseIterator it = Core_EntityGetIterator(&entityContainer); Core_EntityIterate(&entityContainer, &it, &entity);)
	{
		fwrite(entity, sizeof(*entity), 1, file);
	}

	fclose(file);

	SDL_DestroyTexture(texture);
	SDL_UIContextFree(&uictx);
	SDL_GlyphAtlasANSCIIFree(&glyphAtlas);
	FontCacheFree(&fontCache);

	// Destroy Core Resources
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(clientWindow);

	// Shutdown Core Systems
	SDLNet_Quit();
	TTF_Quit();
	SDL_Quit();
	return 0;
}
