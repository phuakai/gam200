#include "mainHeader.h"
#include "ECS.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <filesystem>

rttr::instance GetComponentByName(rttr::type& componentName, const EntityID& entityId);

void imguiInit();
void imguiUpdate();
void imguiDraw();

extern std::vector<EntityID> prefabs;
