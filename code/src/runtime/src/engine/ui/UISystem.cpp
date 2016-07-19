#include <engine/ui/UISystem.hpp>
#include <engine/events/KeyboardEvent.hpp>
#include <engine/events/MouseEvent.hpp>
#include <SDL_syswm.h>

#include <glm/ext.hpp>

#include <engine/events/DrawEvent.hpp>
#include <glow/objects/VertexArray.hh>
#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/objects/Program.hh>
#include <glow/objects/Texture2D.hh>
#include <glow/data/TextureData.hh>
#include <glow/data/SurfaceData.hh>

bool UISystem::startup() {
  RESOLVE_DEPENDENCY(m_events);
  RESOLVE_DEPENDENCY(m_window);
  RESOLVE_DEPENDENCY(m_renderer);

  ImGuiIO &io = ImGui::GetIO();
  io.KeyMap[ImGuiKey_Tab] = SDLK_TAB; // Keyboard mapping. ImGui will use those
                                      // indices to peek into the io.KeyDown[]
                                      // array.
  io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
  io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
  io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
  io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
  io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
  io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
  io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
  io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
  io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
  io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
  io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
  io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
  io.KeyMap[ImGuiKey_A] = SDLK_a;
  io.KeyMap[ImGuiKey_C] = SDLK_c;
  io.KeyMap[ImGuiKey_V] = SDLK_v;
  io.KeyMap[ImGuiKey_X] = SDLK_x;
  io.KeyMap[ImGuiKey_Y] = SDLK_y;
  io.KeyMap[ImGuiKey_Z] = SDLK_z;

  io.RenderDrawListsFn = NULL; // Alternatively you can set this to NULL and
                               // call ImGui::GetDrawData() after
                               // ImGui::Render() to get the same ImDrawData
                               // pointer.
  io.SetClipboardTextFn = [](const char *text) { SDL_SetClipboardText(text); };
  io.GetClipboardTextFn = []() -> const char * {
    return SDL_GetClipboardText();
  };

#ifdef _WIN32
  SDL_SysWMinfo wmInfo;
  SDL_VERSION(&wmInfo.version);
  SDL_GetWindowWMInfo(m_window->getWindowHandle(), &wmInfo);
  io.ImeWindowHandle = wmInfo.info.win.window;
#endif

  setupGLObjects(io);

  m_events->subscribe<"StartFrame"_sh>(
      [this]() { newFrame(); });
  m_events->subscribe<DrawEvent>(
      [this](const DrawEvent &) { prepareRender(); });

  m_events->subscribe<KeyboardEvent>([&io, this](const KeyboardEvent &e) {
    if (e.originalEvent.type == SDL_KEYDOWN ||
        e.originalEvent.type == SDL_KEYUP) {
      int key = e.originalEvent.key.keysym.sym & ~SDLK_SCANCODE_MASK;
      io.KeysDown[key] = (e.originalEvent.type == SDL_KEYDOWN);
      io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
      io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
      io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
    } else if (e.originalEvent.type == SDL_TEXTINPUT) {
      io.AddInputCharactersUTF8(e.originalEvent.text.text);
    }

    if (e.originalEvent.type == SDL_KEYDOWN && e.originalEvent.key.keysym.scancode == SDL_SCANCODE_F1) {
      m_drawUI = !m_drawUI;
    }
  });

  m_events->subscribe<MouseEvent>([&io, this](const MouseEvent &e) {
    if (e.originalEvent.type == SDL_MOUSEWHEEL) {
      if (e.originalEvent.wheel.y > 0) {
        m_mouseWheel = 1;
      } else if (e.originalEvent.wheel.y < 0) {
        m_mouseWheel = -1;
      }
    } else if (e.originalEvent.type == SDL_MOUSEBUTTONDOWN) {
      if (e.originalEvent.button.button == SDL_BUTTON_LEFT)
        m_mousePressed[0] = true;
      if (e.originalEvent.button.button == SDL_BUTTON_RIGHT)
        m_mousePressed[1] = true;
      if (e.originalEvent.button.button == SDL_BUTTON_MIDDLE)
        m_mousePressed[2] = true;
    }
  });

  return true;
}

template <>
GLenum glTypeOf<ImVec2>::type = GL_FLOAT;
template <>
GLenum glTypeOf<ImVec2>::format = GL_RG;
template <>
GLint glTypeOf<ImVec2>::size = 2;
template <>
internal::glBaseType glTypeOf<ImVec2>::basetype = internal::glBaseType::Float;

void UISystem::setupGLObjects(ImGuiIO& io) {

  m_imguiBuffer = ArrayBuffer::create();

  m_imguiElements = ElementArrayBuffer::create();

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
  m_imguiBuffer->defineAttributeWithOffset("Position", GL_FLOAT, 2, OFFSETOF(ImDrawVert, pos));
  m_imguiBuffer->defineAttributeWithOffset("UV", GL_FLOAT, 2, OFFSETOF(ImDrawVert, uv));
  m_imguiBuffer->defineAttributeWithOffset("Color", GL_UNSIGNED_BYTE, 4, OFFSETOF(ImDrawVert, col), glow::AttributeMode::NormalizedInteger);
  m_imguiBuffer->setStride(sizeof(ImDrawVert));
#undef OFFSETOF

  m_imguiVao = VertexArray::create({ m_imguiBuffer }, { m_imguiElements }, GL_TRIANGLES);

  m_imguiProg = Program::createFromFile("ImGui");

  // Build texture atlas
  unsigned char* pixels;
  int width, height;
  // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.
  io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

  
  auto surface = std::make_shared<SurfaceData>();
  // No need to delete pixels since ClearTexData below does that.
  // Just needed to upload it to a texture
  surface->setData({ pixels, pixels + width * height * 4 });
  surface->setFormat(GL_RGBA);
  surface->setType(GL_UNSIGNED_BYTE);
  surface->setWidth(width);
  surface->setHeight(height);

  m_fontTextureData = std::make_shared<TextureData>();
  m_fontTextureData->addSurface(surface);
  m_fontTextureData->setPreferredInternalFormat(GL_RGBA);
  m_fontTextureData->setWidth(width);
  m_fontTextureData->setHeight(height);

  m_fontTexture = Texture2D::createFromData(m_fontTextureData);
  auto boundTex = m_fontTexture->bind();
  boundTex.setMinFilter(GL_LINEAR);
  boundTex.setMagFilter(GL_LINEAR);
  
  // Store our identifier
  io.Fonts->TexID = (void *)(intptr_t)m_fontTexture->getObjectName();

  // Cleanup (don't clear the input data if you want to append new fonts later)
  io.Fonts->ClearInputData();
  io.Fonts->ClearTexData();
}

void UISystem::newFrame() {
  ImGuiIO &io = ImGui::GetIO();

  // Setup display size (every frame to accommodate for window resizing)
  auto windowSize = m_window->getSize();
  io.DisplaySize = ImVec2((float)windowSize.x, (float)windowSize.y);
  io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

  // TODO: get actual delta time
  io.DeltaTime = (float)(1.0f / 60.0f);

  // Setup inputs
  int mx, my;
  Uint32 mouseMask = SDL_GetMouseState(&mx, &my);
  if (SDL_GetWindowFlags(m_window->getWindowHandle()) & SDL_WINDOW_MOUSE_FOCUS && !SDL_GetRelativeMouseMode()) {
    io.MousePos = ImVec2((float)mx, (float)my); // Mouse position, in pixels
                                                // (set to -1,-1 if no mouse /
                                                // on another screen, etc.)
  } else {
    io.MousePos = ImVec2(-1, -1);
  }

  io.MouseDown[0] = m_mousePressed[0] ||
                    (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) !=
                        0; // If a mouse press event came, always pass it as
                           // "mouse held this frame", so we don't miss
                           // click-release events that are shorter than 1
                           // frame.
  io.MouseDown[1] =
      m_mousePressed[1] || (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
  io.MouseDown[2] =
      m_mousePressed[2] || (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
  m_mousePressed[0] = m_mousePressed[1] = m_mousePressed[2] = false;

  io.MouseWheel = m_mouseWheel;
  m_mouseWheel = 0.0f;

  // Hide OS mouse cursor if ImGui is drawing it
  SDL_ShowCursor(io.MouseDrawCursor ? 0 : 1);

  ImGui::NewFrame();
}

void UISystem::renderDrawLists(ImDrawData *drawData) {
  // Backup GL state
  GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
  GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
  GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
  GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
  GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
  GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
  GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
  GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
  GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
  GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
  GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
  GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
  GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
  GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

  // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_SCISSOR_TEST);
  glActiveTexture(GL_TEXTURE0);

  // Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
  ImGuiIO& io = ImGui::GetIO();
  float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
  drawData->ScaleClipRects(io.DisplayFramebufferScale);

  // Setup orthographic projection matrix
  glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
  const float ortho_projection[4][4] =
  {
    { 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
    { 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
    { 0.0f,                  0.0f,                  -1.0f, 0.0f },
    { -1.0f,                  1.0f,                   0.0f, 1.0f },
  };

  auto ortho = glm::make_mat4<float>((float*)ortho_projection);

  auto boundProg = m_imguiProg->use();

  boundProg.setUniform("ProjMtx", ortho);
  boundProg.setTexture("Texture", m_fontTexture);

  for (int n = 0; n < drawData->CmdListsCount; n++) {
    const ImDrawList* cmd_list = drawData->CmdLists[n];
    const ImDrawIdx* idx_buffer_offset = 0;

    {
        auto boundBuffer = m_imguiBuffer->bind();
        auto boundElements = m_imguiElements->bind();

        boundBuffer.setData((GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);
        boundElements.setIndices(cmd_list->IdxBuffer.size(), (ImDrawIdx*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);
    }

    auto boundVAO = m_imguiVao->bind();

    for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++) {
      if (pcmd->UserCallback) {
        pcmd->UserCallback(cmd_list, pcmd);
      } else {
        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
        glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
        glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
      }
      idx_buffer_offset += pcmd->ElemCount;
    }
  }

  // Restore modified GL state
  glUseProgram(last_program);
  glBindTexture(GL_TEXTURE_2D, last_texture);
  glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
  glBindVertexArray(last_vertex_array);
  glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
  glBlendFunc(last_blend_src, last_blend_dst);
  if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
  if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
  if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
  if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
  glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
}

void UISystem::prepareRender() {

  if (!m_drawUI) return;

  m_events->fire<"DrawUI"_sh>();

  ImGui::Render();
  renderDrawLists(ImGui::GetDrawData());
}

void UISystem::shutdown() { ImGui::Shutdown(); }
