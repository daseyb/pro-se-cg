#include <engine/events/EventSystem.hpp>


bool EventSystem::startup() { return true; }

void EventSystem::shutdown() { m_registeredCallbacks.clear(); }
