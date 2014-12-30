#include "Level.h"
#include "Window.h"

Level::Level() {
    
}

// Adds a new sprite to the internal window by taking in a sprite pointer as argument.
// First sends the renderer for the window to the sprite since the sprite needs it in order to draw itself.
// When the sprite has access to the render, it can create its texture. This is done here by calling Sprite::SetUpTexture.
// After these steps, the sprite can be added to the vector of sprites which will be rendererd during the next iteration of the main event loop.
void Level::AddSprite(Sprite* sprite) {
    sprites.push_back(sprite);
    if (is_loaded) {
        window->LoadSprite(sprite);
    }
}

// Removes an existing sprite from the window by taking in a sprite pointer as argument.
// Both frees the memory allocated by the sprite object and removes it from the vector of sprites.
void Level::RemoveSprite(Sprite* sprite) {
    delete sprite;
    for (int i = 0; i < sprites.size(); i++) {
        if (sprite == sprites[i]) {
            sprites.erase(sprites.begin() + i);
        }
    }
}

// Returns a vector of all sprites that have been added to the window.
std::vector<Sprite*> Level::GetSprites() {
    return sprites;
}

// Sets the background of the level by loading the image located at the the path specified as argument.
// The background is added to the level as a new StaticSprite which is then by calling Window::AddSprite.
void Level::SetBackground(std::string background_image_path) {
    Sprite* background_sprite = StaticSprite::GetInstance(background_image_path, 0, 0, 0, 0);
    AddSprite(background_sprite);
}

// Sets the loaded flag of this level.
void Level::SetLoaded(bool is_loaded) {
    this->is_loaded = is_loaded;
}

// Sets the window pointer needed to load more sprites is new ones are added after the level is loaded.
void Level::SetWindow(Window* window) {
    this->window = window;
}

// Delegates an event to the sprites that have been added to the level.
void Level::PropagateEventToSprites(SDL_Event event) {
    for (Sprite* sprite : sprites) {
        sprite->DelegateEvent(event);
    }
}

Level::~Level() {
    for (Sprite* sprite : sprites) {
        delete sprite;
    }
}