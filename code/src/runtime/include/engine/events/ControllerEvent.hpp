#pragma once
#include <engine/events/SDLEvent.hpp>

struct ControllerEvent : public SDLEvent<ControllerEvent> {
  ControllerEvent(SDL_Event e) : SDLEvent<ControllerEvent>(e) {}
};