#include <engine/events/EventSystem.hpp>
#include <ACGL/ACGL.hh>
#include <algorithm>




bool EventSystem::startup() { return true; }

void EventSystem::shutdown() { m_registeredCallbacks.clear(); }
