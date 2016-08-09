#include <engine/scene/SceneGraphSystem.hpp>
#include <engine/events/PrepareDrawEvent.hpp>
#include <engine/scene/Drawable.hpp>
#include <engine/scene/Transform.hpp>
#include <glm/ext.hpp>
#include <engine/ui/UISystem.hpp>

#undef near
#undef far

bool SceneGraphSystem::startup() {
  RESOLVE_DEPENDENCY(m_events);
  RESOLVE_DEPENDENCY(m_renderer);

  m_events->subscribe<PrepareDrawEvent>(
      [this](const PrepareDrawEvent &e) { prepareDraw(e.interp); });
  m_events->subscribe<"EndFrame"_sh>([this]() { endFrame(); });
  m_events->subscribe<"StartSimulate"_sh>([this]() { startSimulate(); });
  m_events->subscribe<"DrawUI"_sh>([this]() {
      ImGui::Begin("Entities");
      auto allEntities = m_entityManager.entities_for_debugging();
      for (auto& entity : allEntities) {
          std::string entityName = std::string("Id: ") + std::to_string(entity.id().index());
          if (ImGui::TreeNode(entityName.c_str())) {
              auto component_mask = entity.component_mask();

              ImGui::LabelText("Components", "%d", component_mask.count());
              if (entity.has_component<Camera>() && ImGui::TreeNode("Camera")) {
                  auto camera = entity.component<Camera>();
                  ImGui::InputFloat("Near", &camera->near);
                  ImGui::InputFloat("Far", &camera->far);
                  ImGui::InputFloat("Focal Distance", &camera->focalDistance);
                  ImGui::InputFloat("Lens Radius", &camera->lensRadius);
                  ImGui::SliderFloat("FoV", &camera->fov, 40, 150);
                  ImGui::TreePop();
              }

              if (entity.has_component<Transform>() && ImGui::TreeNode("Transform")) {
                  auto transform = entity.component<Transform>();
                  ImGui::InputFloat3("Position", (float*)&transform->position);
                  ImGui::InputFloat3("Scale", (float*)&transform->scale);

                  glm::vec3 rotation;
                  glm::extractEulerAngleXYZ(transform->rotation, rotation.x, rotation.y, rotation.z);
                  ImGui::InputFloat3("Rotation", (float*)&rotation);
                  transform->rotation = glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
                  ImGui::TreePop();
              }

              if (entity.has_component<Drawable>() && ImGui::TreeNode("Drawable")) {
                  auto drawable = entity.component<Drawable>();

                  ImGui::Checkbox("Visible", &drawable->visible);
                  ImGui::Separator();
                  ImGui::ColorEdit3("Diffuse Color", (float*)&drawable->material.diffuseColor);
                  m_renderer->showTextureChooser(drawable->material.diffuseTexture, entityName + std::string("diff"));

                  ImGui::ColorEdit3("Specular Color", (float*)&drawable->material.specularColor);
                  m_renderer->showTextureChooser(drawable->material.specularTexture, entityName + std::string("spec"));

                  ImGui::ColorEdit3("Emissive Color", (float*)&drawable->material.emissiveColor);
                  m_renderer->showTextureChooser(drawable->material.emissiveTexture, entityName + std::string("emmi"));

                  ImGui::SliderFloat("IOR", (float*)&drawable->material.eta, 1.0f, 6.0f);
                  ImGui::SliderFloat("Refractiveness", (float*)&drawable->material.refractiveness, 0.0f, 1.0f);
                  ImGui::SliderFloat("Roughness", (float*)&drawable->material.roughness, 0.0f, 1.0f);

                  m_renderer->showTextureChooser(drawable->material.normalsTexture, entityName + std::string("norm"));

                  ImGui::TreePop();
              }

              if (entity.has_component<Light>() && ImGui::TreeNode("Light")) {
                  auto light = entity.component<Light>();

                  ImGui::ColorEdit4("Color", (float*)&light->color);
                  ImGui::InputFloat("Size", (float*)&light->size);

                  ImGui::TreePop();
              }

              ImGui::TreePop();
          }
      }

      ImGui::End();
  }, -1);
  return true;
}

Entity SceneGraphSystem::create() { return m_entityManager.create(); }

void SceneGraphSystem::destroy(Entity::Id id) { m_freeAtEndOfFrame.push(id); }

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
  auto transformEntities =
      m_entityManager.entities_with_components<Transform>();

  Transform::Handle transform;
  for (auto e : transformEntities) {
    e.unpack<Transform>(transform);

    Transform::Handle parent = transform->parent;
    TransformData lastGlobalTransform = {
        transform->lastPosition, glm::quat_cast(transform->lastRotation),
        transform->lastScale};
    TransformData thisGlobalTransform = {transform->position,
                                         glm::quat_cast(transform->rotation),
                                         transform->scale};

    while (parent.valid()) {
      TransformData lastParentTransform = {parent->lastPosition,
                                           glm::quat_cast(parent->lastRotation),
                                           parent->lastScale};
      TransformData thisParentTransform = {
          parent->position, glm::quat_cast(parent->rotation), parent->scale};

      lastGlobalTransform.scale *= lastParentTransform.scale;
      thisGlobalTransform.scale *= thisParentTransform.scale;

      lastGlobalTransform.rot *= lastParentTransform.rot;
      thisGlobalTransform.rot *= thisParentTransform.rot;

      lastGlobalTransform.pos =
          lastParentTransform.pos +
          (lastGlobalTransform.pos * lastParentTransform.scale) *
              glm::inverse(lastParentTransform.rot);
      thisGlobalTransform.pos =
          thisParentTransform.pos +
          (thisGlobalTransform.pos * thisParentTransform.scale) *
              glm::inverse(thisParentTransform.rot);

      lastGlobalTransform.pos =
          lastParentTransform.pos + lastGlobalTransform.pos;
      thisGlobalTransform.pos =
          thisParentTransform.pos + thisGlobalTransform.pos;

      parent = parent->parent;
    }

    transform->lastGlobalTransform = lastGlobalTransform;
    transform->thisGlobalTransform = thisGlobalTransform;
  }

  auto drawableEntities =
      m_entityManager.entities_with_components<Drawable, Transform>();

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
    if (drawable->renderPassIndex >= cams.size())
      continue;

    auto thisRenderTransform = interpolate(
        transform->lastGlobalTransform, transform->thisGlobalTransform, interp);
    if (drawable->visible) {
      m_renderer->submit({drawable->material, drawable->geometry,
                          transform->lastRenderTransform, thisRenderTransform}, drawable->renderPassIndex);
    }
    transform->lastRenderTransform = thisRenderTransform;
  }

  auto lightEntities =
      m_entityManager.entities_with_components<Light, Transform>();

  Light::Handle light;
  for (auto e : lightEntities) {
    e.unpack<Light, Transform>(light, transform);

    auto viewMatrix = glm::lookAt<double>(
        glm::dvec3(light->dir), {0.0, 0.0, 0.0}, glm::dvec3(0.0, 1.0, 0.0));
    auto lightVP = glm::ortho<double>(-7, +7, -7, +7, -7, 7) *
                   static_cast<glm::dmat4>(viewMatrix);

    m_renderer->submit(LightData{light->color, light->size, light->dir,
                                 light->type, transform->lastGlobalTransform,
                                 transform->thisGlobalTransform, lightVP},
                       light->renderPassIndex);
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
