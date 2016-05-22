#pragma once
#include <engine/events/SDLEvent.hpp>

struct TouchEvent : public SDLEvent<TouchEvent> {
  TouchEvent(SDL_Event e) : SDLEvent<TouchEvent>(e) {}
};