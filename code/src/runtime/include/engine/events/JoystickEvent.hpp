#pragma once
#include <engine/events/SDLEvent.hpp>

struct JoystickEvent : public SDLEvent<JoystickEvent> {
  JoystickEvent(SDL_Event e) : SDLEvent<JoystickEvent>(e) { }
};