#include <engine/scene/SceneGraphSystem.hpp>
#include <engine/events/PrepareDrawEvent.hpp>
#include <engine/scene/Drawable.hpp>
#include <engine/scene/Transform.hpp>

bool SceneGraphSystem::startup() {
  RESOLVE_DEPENDENCY(m_events);
  RESOLVE_DEPENDENCY(m_renderer);

  m_events->subscribe<PrepareDrawEvent>([this](const PrepareDrawEvent& e) { prepareDraw(e.interp); });
  m_events->subscribe<"EndFrame"_sh>([this]() { endFrame(); });
  m_events->subscribe<"StartSimulate"_sh>([this]() { startSimulate(); });
  return true;
}

Entity SceneGraphSystem::create() {
  return m_entityManager.create();
}

void SceneGraphSystem::destroy(Entity::Id id) {
  m_freeAtEndOfFrame.push(id);
}

void SceneGraphSystem::startSimulate() {
  auto drawableEntities = m_entityManager.entities_with_components<Transform>();

  Transform::Handle transform;
  for (auto e : drawableEntities) {
    e.unpack<Transform>(transform);
    transform->lastPosition = transform->position;
    transform->lastRotation = transform->rotation;
	  transform->lastScale = transform->scale;
  }
}

void SceneGraphSystem::prepareDraw(double interp) {
  auto transformEntities = m_entityManager.entities_with_components<Transform>();

  Transform::Handle transform;
  for (auto e : transformEntities) {
    e.unpack<Transform>(transform);

    Transform::Handle parent = transform->parent;
    TransformData lastGlobalTransform = { transform->lastPosition, glm::quat_cast(transform->lastRotation), transform->lastScale };
    TransformData thisGlobalTransform = { transform->position, glm::quat_cast(transform->rotation), transform->scale };
    

     while (parent.valid()) {
      TransformData lastParentTransform = { parent->lastPosition, glm::quat_cast(parent->lastRotation), parent->lastScale };
      TransformData thisParentTransform = { parent->position, glm::quat_cast(parent->rotation), parent->scale };

	    // For now, only look at the positions of our parent, scale and rotation needs some additional logic TODO

	    /*
      lastGlobalTransform.scale *= lastParentTransform.scale;
      thisGlobalTransform.scale *= thisParentTransform.scale;

      lastGlobalTransform.rot *= lastParentTransform.rot;
      thisGlobalTransform.rot *= thisParentTransform.rot;

      lastGlobalTransform.pos = lastParentTransform.pos + (lastGlobalTransform.pos * lastParentTransform.scale) * glm::inverse(lastParentTransform.rot);
      thisGlobalTransform.pos = thisParentTransform.pos + (thisGlobalTransform.pos * thisParentTransform.scale) * glm::inverse(thisParentTransform.rot);

	    */
	    lastGlobalTransform.pos = lastParentTransform.pos + lastGlobalTransform.pos;
	    thisGlobalTransform.pos = thisParentTransform.pos + thisGlobalTransform.pos;

      parent = parent->parent;
    }

    transform->lastGlobalTransform = lastGlobalTransform;
    transform->thisGlobalTransform = thisGlobalTransform;
  }

  auto drawableEntities = m_entityManager.entities_with_components<Drawable, Transform>();

	auto cameras = m_entityManager.entities_with_components<Camera>();	
  std::vector<glm::dvec3> cams(m_renderer->getNumPasses());
	for (auto e : cameras) {
    auto passIndex = e.component<Camera>()->renderPassIndex;
    if (passIndex < cams.size()) {
      cams[passIndex] = e.component<Transform>()->thisGlobalTransform.pos;
    }
	}

  Drawable::Handle drawable;
  for (auto e : drawableEntities) {
    e.unpack<Drawable, Transform>(drawable, transform);
    if (drawable->renderPassIndex >= cams.size()) continue;

    auto thisRenderTransform = interpolate(transform->lastGlobalTransform, transform->thisGlobalTransform, interp, cams[drawable->renderPassIndex]);
    if (drawable->visible) {
	    m_renderer->submit({ drawable->material, drawable->geometry, transform->lastRenderTransform, thisRenderTransform, drawable->recursionDepth }, drawable->material.queue, drawable->renderPassIndex);
    }
    transform->lastRenderTransform = thisRenderTransform;
  }

  auto lightEntities = m_entityManager.entities_with_components<Light, Transform>();

  Light::Handle light;
  for (auto e : lightEntities) {
    e.unpack<Light, Transform>(light, transform);

    auto viewMatrix =  glm::lookAt<double>(glm::dvec3(light->dir), { 0.0, 0.0, 0.0 }, glm::dvec3(0.0, 1.0, 0.0));
    auto lightVP = glm::ortho<double>(-7, +7, -7, +7, -7, 7) * static_cast<glm::dmat4>(viewMatrix);

    m_renderer->submit({light->color, light->castShadow,light->dir, light->type, transform->lastGlobalTransform, transform->thisGlobalTransform, lightVP, nullptr, nullptr}, light->renderPassIndex);
  }

}

void SceneGraphSystem::endFrame() {
  for (size_t i = 0; i < m_freeAtEndOfFrame.size(); i++) {
    auto id = m_freeAtEndOfFrame[i];
    m_entityManager.destroy(id);
  }

  m_freeAtEndOfFrame.reset();
}


void SceneGraphSystem::shutdown() {}

