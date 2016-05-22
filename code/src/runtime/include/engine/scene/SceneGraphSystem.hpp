#pragma once
#include <engine/core/Context.hpp>
#include <engine/core/System.hpp>
#include <engine/events/EventSystem.hpp>
#include <engine/graphics/RendererSystem.hpp>
#include <engine/scene/Entity.hpp>

#include <engine/utils/Stack.hpp>

#include <bitset>
#include <unordered_map>
#include <vector>

class SceneGraphSystem : public System {
private:
  EventSystem *m_events;
  RendererSystem *m_renderer;

  EntityManager m_entityManager;

  Stack<Entity::Id> m_freeAtEndOfFrame;

public:
  CONSTRUCT_SYSTEM(SceneGraphSystem),m_freeAtEndOfFrame(100), m_entityManager() {}

  Entity create();
  void destroy(Entity::Id id);

  template <typename... Components>
  EntityManager::View<Components...> entities_with_components() {
    return m_entityManager.entities_with_components<Components...>();
  }

  void startSimulate();
  void prepareDraw(double interp);
  void endFrame();

  bool startup() override;
  void shutdown() override;

};