#pragma once
#include <engine/events/SDLEvent.hpp>

struct MouseEvent : public SDLEvent<MouseEvent> {
  MouseEvent(SDL_Event e) : SDLEvent<MouseEvent>(e) {}
};