#pragma once
#include <engine/events/SDLEvent.hpp>

struct WindowEvent : public SDLEvent<WindowEvent> {
  WindowEvent(SDL_Event e) : SDLEvent<WindowEvent>(e) {}
};