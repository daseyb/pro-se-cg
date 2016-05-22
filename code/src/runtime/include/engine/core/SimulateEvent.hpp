#pragma once
#include <engine/events/Event.hpp>

struct SimulateEvent : public Event<SimulateEvent> {
  SimulateEvent(float dt, float totalTime) : dt(dt), totalTime(totalTime) {};
  float dt;
  float totalTime;
};