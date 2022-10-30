#include "app.h"
#include "mainHeader.h"
#include "levelEditor.h"
#include "levelEditorHierarchy.h"
#include "levelEditorProperties.h"
#include "levelEditorContentBrowser.h"
#include "framebuffer.h"

#include <cstring>

bool show_demo_window;
bool show_another_window;
ImVec4 clear_color;

extern FrameBufferNS::frameBuffer mainFrame; // This is the externed mainFrame variable to read framebuffer id from

void imguiInit()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(Graphics::Input::ptr_to_window, true);
	ImGui_ImplOpenGL3_Init(NULL);

	show_demo_window = true;
	show_another_window = true;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable;
}

void imguiUpdate()
{
	//-----------------------------------------extra imgui stuff here  -- we are so moving this out
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport();

	//ImGuiViewport* viewportID = ImGui::FindViewportByPlatformHandle(Graphics::Input::ptr_to_window);
	//ImGui::GetPlatformIO().Viewports.push_back(viewportID);
	//ImGuiWindow* currentWindow = ImGui::GetCurrentWindow();
	//ImGui::SetWindowViewport(currentWindow, (ImGuiViewportP*)viewportID);
	 
	if (ImGui::Begin("Camera", (bool*)0, ImGuiWindowFlags_NoScrollbar))
	{
		auto tex = mainFrame.framebuffer;
		ImVec2 size = ImGui::GetWindowSize();
		ImGui::Image((ImTextureID)tex, size, ImVec2(0, 1), ImVec2(1, 0));

		double mousePosX = 0.0, mousePosY = 0.0;
		Graphics::Input::getCursorPos(&mousePosX, &mousePosY);

		double screenHeight = (double)Graphics::Input::screenheight;
		double screenWidth = (double)Graphics::Input::screenwidth;

		// Bottom left of the imgui window 
		ImVec2 imguiWindowPosition = ImGui::GetCursorScreenPos();
		// Size of the imgui window
		ImVec2 imguiWindowSize = ImGui::GetWindowSize();

		imguiMouseX = mousePosX;
		imguiMouseY = mousePosY;

		imguiMouseX -= (double)imguiWindowPosition.x - (screenWidth / 2 - (double)imguiWindowSize.x / 2);
		imguiMouseY += (double)imguiWindowPosition.y - (screenHeight / 2 + (double)imguiWindowSize.y / 2);

		imguiMouseX *= screenWidth / (double)imguiWindowSize.x;
		imguiMouseY *= screenHeight / (double)imguiWindowSize.y;

		ImGui::End();
	}

	//static bool inputs_step = true;
	//static float f32_v = 0.123f;
	//const float f32_one = 1.f;

	levelEditorHierarchy& hierarchy = levelEditorHierarchy::getInstance();
	levelEditorProperties& properties = levelEditorProperties::getInstance();
	levelEditorContentBrowser& contentBrowser = levelEditorContentBrowser::getInstance();

	ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
	
	// "Entities" tab =========================================================================================
	ImGui::Begin("Entities");

	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Hierarchy"))
		{
			hierarchy.ImGuiHierarchy();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Not Sure, Not Done"))
		{
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
	// ========================================================================================================

	// "Details" tab ==========================================================================================
	ImGui::Begin("Details");

	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Properties"))
		{
			properties.ImGuiProperties(hierarchy.getSelected());
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Not Sure, Not Done"))
		{
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
	// ========================================================================================================

	// "Prefabs" tab ==========================================================================================
	ImGui::Begin("Content Browser");

	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("File Browser"))
		{
			contentBrowser.ImGuiContentBrowser();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Not Sure, Not Done"))
		{
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
	// ========================================================================================================
	ImGui::Render();
}

void imguiDraw()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
