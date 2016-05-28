#pragma once
#include <engine/events/Event.hpp>

struct ResizeWindowEvent : public Event<ResizeWindowEvent> {
  ResizeWindowEvent(glm::vec2 newSize) : newSize(newSize) {}
  glm::vec2 newSize;
};