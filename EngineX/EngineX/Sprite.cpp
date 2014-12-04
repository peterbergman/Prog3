#include "Sprite.h"
Sprite::Sprite(std::string file_name, int x_pos, int y_pos, int height, int width) {
    boundary = new SDL_Rect();
    boundary->x = x_pos;
    boundary->y = y_pos;
    boundary->h = height;
    boundary->w = width;
    SDL_Surface* surface = IMG_Load(file_name.c_str());
    if (surface == nullptr) { // TODO: throw exception to application
        std::cout << "Could not load image... :(" << std::endl;
    } else {
        texture = SDL_CreateTextureFromSurface(window->GetRenderer(), surface);
        SDL_FreeSurface(surface);
        if (texture == nullptr) { // TODO: throw exception to application
            std::cout << "Could not create texture... :(" << std::endl;
        }
    }
}

void Sprite::SetWindow(Window* window) {
    this->window = window;
}

void Sprite::Draw() {
    SDL_RenderCopy(window->GetRenderer(), texture, NULL, NULL);
}

Sprite::~Sprite() {
    delete boundary;
    SDL_DestroyTexture(texture);
}