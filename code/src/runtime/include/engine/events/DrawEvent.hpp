#pragma once
#include <engine/events/Event.hpp>

struct DrawEvent : public Event<DrawEvent> {
  DrawEvent(double i, double totalTime) : interp(i), totalTime(totalTime) { }
  double interp;
  double totalTime;
};