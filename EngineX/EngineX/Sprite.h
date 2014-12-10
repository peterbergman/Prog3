
#ifndef __EngineX__Sprite__
#define __EngineX__Sprite__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>

class Sprite;

typedef void (*action_listener)(SDL_Event, Sprite*);

class Sprite {
public:
    Sprite(int, int, int, int);
    void SetRenderer(SDL_Renderer*);
    void AddActionListener(action_listener);
    void AddTimeEventListener(action_listener, int);
    void HandleEvent(SDL_Event);
    void SetX(int);
    void SetY(int);
    int GetX();
    int GetY();
    int GetWidth();
    int GetHeight();
    bool Contains(int, int);
    bool Contains(Sprite*);
    virtual void SetUpTexture() = 0;
    virtual void Draw() = 0;
    virtual ~Sprite();
protected:
    SDL_Renderer* renderer;
    SDL_Rect* boundary;
private:
    std::vector<action_listener> action_event_listeners;
    std::map<int, action_listener> time_event_listeners;
    void HandleActionEvent(SDL_Event, bool);
    void HandleTimeEvent(SDL_Event);
};

#endif
