#pragma once
#include <engine/events/Event.hpp>
#include <ACGL/Math/Math.hh>

struct ResizeWindowEvent : public Event<ResizeWindowEvent> {
  ResizeWindowEvent(glm::vec2 newSize) : newSize(newSize) {}
  glm::vec2 newSize;
};