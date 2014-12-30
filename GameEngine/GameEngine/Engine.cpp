#include "Engine.h"
#include <sys/time.h>

Uint32 Engine::time_event_type;

Engine::Engine(std::string game_name, int fps, int window_width, int window_height):fps(fps), frame_counter(0) {
    window = new Window(game_name, window_width, window_height);
}

// The main event loop of the game engine.
// Executes the following steps:
// 1. Get a timestamp at the start of the iteration.
// 2. Poll all events that has been emitted since the last iteration (and delegate them).
// 3. Delegate re-drawing of sprites by calling Window::DrawSprites.
// 4. Increment the frame counter.
// 5. Check for collisions.
// 6. Emit a new time event.
// 7. Timeout for 1000 / fps milliseconds.
// 8. Get a timestamp at the end of the iteration.
// 9. Set the total time that the iteration took.
void Engine::Run() {
    is_running = true;
    while (is_running) {
        long start_time = GetTimestamp();
        PollEvent();
        window->DrawSprites(time_elapsed);
        frame_counter++;
        DetectCollision();
        EmitTimeEvent();
        SDL_Delay(1000 / fps);
        long stop_time = GetTimestamp();
        SetTimeElapsed(start_time, stop_time);
    }
}

// Adds a new level to this game engine.
void Engine::AddLevel(Level* level) {
    levels.push_back(level);
}

// Sets the current level of this engine by directly calling Window::LoadLevel.
void Engine::SetCurrentLevel(Level* level) {
    current_level = level;
    window->LoadLevel(level);
}

// Sets the collision listener that is called each time a collision occurs.
void Engine::SetCollisionListener(collision_listener listener) {
    current_collision_listener = listener;
}

// Adds a new time listener to the internal map that contains all time listeners.
// The delay is used as key, meaning that two time listeners with the same delay cannot be
// registered at the same time.
void Engine::AddTimeListener(event_listener listener, int delay) {
    time_listeners[delay] = listener;
}

// Adds a new event listener to the interal map that contains all event listeners.
// The keycode is used as key, meaning that two event listeners with the same keycode cannot be
// registered at the same time.
void Engine::AddEventListener(event_listener listener, int key_code) {
    event_listeners[key_code] = listener;
}

// Returns the actual time (in milliseconds) that has elapsed since the last iteration of the main event loop (ie. the actual time between two frames).
double Engine::GetTimeElapsed() {
    return time_elapsed;
}

// Returns the width of the underlaying window.
int Engine::GetWindowWidth() {
    return window->GetWidth();
}

// Returns the height of the underlaying window.
int Engine::GetWindowHeight() {
    return window->GetHeight();
}

// Sets the flag that is controlling the main event loop to false.
void Engine::Quit() {
    is_running = false;
}

// Sets the time_event_type if not previsouly set by calling SDL_RegisterEvents.
// Then creates a new event if a new time event type ID could be generated.
// The current fps value (user.data1) as well as the frame_counter value (user.data2) is added to the
// event before it is pushed to SDL by calling SDL_PushEvent.
void Engine::EmitTimeEvent() {
    if (time_event_type == 0) {
        time_event_type = SDL_RegisterEvents(1);
    }
    if (time_event_type != ((Uint32)-1)) {
        SDL_Event time_event;
        SDL_zero(time_event);
        time_event.type = time_event_type;
        time_event.user.code = 0;
        time_event.user.data1 = &fps;
        time_event.user.data2 = &frame_counter;
        SDL_PushEvent(&time_event);
    }
}

// Called in each iteration of the main event looop. Iterates through each sprite and checks if that sprites contains any of the other sprites.
// If a collision is detected, then the current collision listener is called (if any).
// This collision detection is only considering overlaping sprite boundaries and does not check for collisions on pixel level.
// The time complexity for this function is O(N^2) where N is the number of sprites added to the game engine.
void Engine::DetectCollision() {
    for (Sprite* sprite : current_level->GetSprites()) {
        for (Sprite* other_sprite : current_level->GetSprites()) {
            if (sprite != other_sprite && sprite->Contains(other_sprite)) { // TODO: transparent pixels
                if (current_collision_listener != nullptr) {
                    current_collision_listener(sprite, other_sprite);
                }
            }
        }
    }
}

// Delegates an event to the correct handler function and propagates the event to the sprites.
void Engine::DelegateEvent(SDL_Event event) {
    if (event.type == SDL_MOUSEMOTION || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.type == SDL_MOUSEWHEEL) {
        HandleEvent(event, true);
    } else if (event.type == SDL_KEYDOWN) {
        HandleEvent(event, false);
    } else if (event.type == Engine::time_event_type) {
        HandleTime(event);
    } else if (event.type == SDL_QUIT) {
        Quit();
    }
    current_level->PropagateEventToSprites(event);
}

// Iterates through each event listener and evaluates if the event listener should be called.
// This is done by checking that the event source corresponds to the key or button registererd for the listner.
void Engine::HandleEvent(SDL_Event event, bool mouse_event) {
    for (std::pair<const int, event_listener>& entry : event_listeners) {
        if (mouse_event && entry.first == event.type) {
            entry.second();
        } else if (!mouse_event && entry.first == event.key.keysym.sym) {
            entry.second();
        }
    }
}

// Iterates through each time listener and evaluates if the time listener should be called.
// This is done by calculating the number of main event loop iterations that should elapse before the time event listener is called.
// Example:
// if the current fps is set to 30 and the delay for a time event listener is set to 60. Then that specific time event listener
// should be called every second main event loop iteration.
void Engine::HandleTime(SDL_Event event) {
    for (std::pair<const int, event_listener>& entry : time_listeners) {
        int fps = *((int*)event.user.data1);
        int frame_counter = *((int*)event.user.data2);
        int rhs = (int)(round(((fps / 1000.0 ) * entry.first)));
        if (rhs > 0) {
            int result = frame_counter % rhs;
            if (result == 0) {
                entry.second();
            }
        } else {
            entry.second();
        }
    }
}

// Polls all events that have been registered since the last iteration of the main event loop and delegates them.
void Engine::PollEvent() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        DelegateEvent(event);
    }
}

// Returns the current timestamp in milliseconds.
long Engine::GetTimestamp() {
    timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

// Sets the time elapsed between two iterations of the main event loop.
void Engine::SetTimeElapsed(long start_time, long stop_time) {
    time_elapsed = (double)(stop_time - start_time);
}

Engine::~Engine() {
    delete window;
    for (Level* level : levels) {
        delete level;
    }
}