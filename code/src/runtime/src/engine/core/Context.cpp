#include <engine/core/Context.hpp>


bool Context::startup() {
  m_initializedSystemCount = 0;
  bool allGood = true;
  for (auto sys : m_startupList) {
    if (!sys->startup()) {
      allGood = false;
      break;
    }
    sys->m_IsInitialized = true;
    m_initializedSystemCount++;
  }

  if (!allGood) {
    shutdown();
  }

  return allGood;
}

void Context::shutdown() {
  if (m_initializedSystemCount > 0) {
    for (int i = m_initializedSystemCount - 1; i >= 0; i--) {
      m_startupList[i]->shutdown();
    }

    m_initializedSystemCount = 0;
  }
}


