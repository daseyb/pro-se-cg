#pragma once
#include <engine/events/Event.hpp>
#include <SDL.h>

template<typename Derived>
struct SDLEvent : public Event<Derived> {
  SDLEvent(SDL_Event e) : originalEvent(e) {};
  SDL_Event originalEvent;
};