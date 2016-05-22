#pragma once
#include <engine/events/SDLEvent.hpp>

struct KeyboardEvent : public SDLEvent<KeyboardEvent> {
  KeyboardEvent(SDL_Event e) : SDLEvent<KeyboardEvent>(e) {}
};