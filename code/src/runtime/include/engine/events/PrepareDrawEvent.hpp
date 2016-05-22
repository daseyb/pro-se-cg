#pragma once
#include <engine/events/Event.hpp>

struct PrepareDrawEvent : public Event<PrepareDrawEvent> {
  PrepareDrawEvent(double i, double totalTime) : interp(i), totalTime(totalTime) { }
  double interp;
  double totalTime;
};