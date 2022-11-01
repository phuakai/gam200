#include "app.h"
#include "mainHeader.h"
#include "levelEditorHierarchy.h"
#include "levelEditorProperties.h"
#include "levelEditorContentBrowser.h"
#include "framebuffer.h"
#include <cstring>
#include <ImGuizmo.h>
#include "GraphEditor.h"
#include <math.h>

bool show_demo_window;
bool show_another_window;
ImVec4 clear_color;

extern FrameBufferNS::frameBuffer mainFrame; // This is the externed mainFrame variable to read framebuffer id from

static std::filesystem::path saveLocation = std::filesystem::current_path();

static void ShowExampleAppCustomNodeGraph(bool* opened);

void menu()
{
	if (ImGui::Button("New"))
	{
	}

	if (ImGui::Button("Open"))
	{
		ImGui::OpenPopup("openFile");
	}
	if (ImGui::BeginPopupModal("openFile", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Select File To Open");
		ImGui::Separator();

		if (ImGui::Button("<<"))
		{
			saveLocation = saveLocation.parent_path();
		}

		for (auto& p : std::filesystem::directory_iterator(saveLocation))
		{
			auto relativePath = std::filesystem::relative(p.path(), saveLocation);
			std::string path = relativePath.string();

			if (p.is_directory())
			{
				if (ImGui::Button(path.c_str()))
				{
					saveLocation /= p.path().filename();
				}
			}
			else
			{
				if (path.find(".json") != std::string::npos)
				{
					ImGui::Button(path.c_str());
				}
			}

			ImGui::NextColumn();
		}

		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}


	if (ImGui::Button("Save"))
	{

	}
	if (ImGui::Button("Save As..."))
	{

	}

	ImGui::Separator();

	if (ImGui::Button("Quit"))
	{
		exit(1);
	}
	//if (ImGui::BeginMenu("Open Recent"))
	//{
	//	ImGui::MenuItem("fish_hat.c");
	//	ImGui::MenuItem("fish_hat.inl");
	//	ImGui::MenuItem("fish_hat.h");
	//	if (ImGui::BeginMenu("More.."))
	//	{
	//		ImGui::MenuItem("Hello");
	//		ImGui::MenuItem("Sailor");
	//		if (ImGui::BeginMenu("Recurse.."))
	//		{
	//			ShowExampleMenuFile();
	//			ImGui::EndMenu();
	//		}
	//		ImGui::EndMenu();
	//	}
	//	ImGui::EndMenu();
	//}
}

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
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_MenuBar;

	if (ImGui::Begin("ImGui", (bool*)0, windowFlags))
	{
		static bool show_app_main_menu_bar = false;
		
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				menu();

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		if (ImGui::BeginTabBar("Hierarchy", tab_bar_flags))
		{
			hierarchy.ImGuiHierarchy();

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
	// ========================================================================================================

	// "Details" tab ==========================================================================================
	ImGui::Begin("Details");

	if (ImGui::BeginTabBar("Properties", tab_bar_flags))
	{
		properties.ImGuiProperties(hierarchy.getSelected());

		ImGui::EndTabBar();
	}

	ImGui::End();
	// ========================================================================================================

	// "Prefabs" tab ==========================================================================================
	ImGui::Begin("Content Browser");

	if (ImGui::BeginTabBar("File Browser", tab_bar_flags))
	{
		contentBrowser.ImGuiContentBrowser(hierarchy.getSelected());

		ImGui::EndTabBar();
	}

	ImGui::End();
	// ========================================================================================================

	ShowExampleAppCustomNodeGraph((bool*)1);

	ImGui::Render();
}

void imguiDraw()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

const float NODE_SLOT_RADIUS = 4.0f;
const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);
bool dragCheck = false;

struct Node
{
	int     ID;
	char    Name[32];
	ImVec2  Pos, Size;
	float   Value;
	ImVec4  Color;
	int     InputsCount, OutputsCount;

	Node(int id, const char* name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count) { ID = id; strcpy_s(Name, name); Pos = pos; Value = value; Color = color; InputsCount = inputs_count; OutputsCount = outputs_count; }

	ImVec2 GetInputSlotPos(int slot_no) const { return ImVec2(Pos.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)InputsCount + 1)); }
	ImVec2 GetOutputSlotPos(int slot_no) const { return ImVec2(Pos.x + Size.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)OutputsCount + 1)); }
};
struct NodeLink
{
	int     InputIdx, InputSlot, OutputIdx, OutputSlot;

	NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) { InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot; }
};

Node* outputNode;
int outputNodeSlot;
int outputIndex;
std::string linkType;
Node* inputNode;

bool nodeLinkButtonCheck(ImVec2 nodeLink)
{
	ImVec2 mousePos = ImGui::GetMousePos();

	if (mousePos.x > nodeLink.x - NODE_SLOT_RADIUS && mousePos.x < nodeLink.x + NODE_SLOT_RADIUS &&
		mousePos.y > nodeLink.y - NODE_SLOT_RADIUS && mousePos.y < nodeLink.y + NODE_SLOT_RADIUS)
	{
		return true;
	}
	return false;
}

static void ShowExampleAppCustomNodeGraph(bool* opened)
{
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Behaviour Tree", opened))
	{
		ImGui::End();
		return;
	}

	// State
	static ImVector<Node> nodes;
	static ImVector<NodeLink> links;
	static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	static bool inited = false;
	static bool show_grid = true;
	static int node_selected = -1;

	// Initialization
	ImGuiIO& io = ImGui::GetIO();
	if (!inited)
	{
		nodes.push_back(Node(0, "MainTex", ImVec2(40, 50), 0.5f, ImColor(255, 100, 100), 1, 1));
		nodes.push_back(Node(1, "BumpMap", ImVec2(40, 150), 0.42f, ImColor(200, 100, 200), 1, 1));
		nodes.push_back(Node(2, "Combine", ImVec2(270, 80), 1.0f, ImColor(0, 200, 100), 2, 2));
		links.push_back(NodeLink(0, 0, 2, 0));
		links.push_back(NodeLink(1, 0, 2, 1));
		inited = true;
	}

	// Draw a list of nodes on the left side
	bool open_context_menu = false;
	int node_hovered_in_list = -1;
	int node_hovered_in_scene = -1;
	ImGui::BeginChild("node_list", ImVec2(100, 0));
	ImGui::Text("Nodes");
	ImGui::Separator();
	for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
	{
		Node* node = &nodes[node_idx];
		ImGui::PushID(node->ID);
		if (ImGui::Selectable(node->Name, node->ID == node_selected))
			node_selected = node->ID;
		if (ImGui::IsItemHovered())
		{
			node_hovered_in_list = node->ID;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}
		ImGui::PopID();
	}
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginGroup();

	// Create our child canvas
	ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);
	ImGui::SameLine(ImGui::GetWindowWidth() - 100);
	ImGui::Checkbox("Show grid", &show_grid);
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(60, 60, 70, 200));
	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	ImGui::PopStyleVar(); // WindowPadding
	ImGui::PushItemWidth(120.0f);

	const ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	ImVec2 mousePos = ImGui::GetMousePos();

	// Display grid
	if (show_grid)
	{
		ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
		float GRID_SZ = 64.0f;
		ImVec2 win_pos = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetWindowSize();
		for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
			draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
		for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
			draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
	}

	// Display links
	draw_list->ChannelsSplit(2);
	draw_list->ChannelsSetCurrent(0); // Background
	for (int link_idx = 0; link_idx < links.Size; link_idx++)
	{
		NodeLink* link = &links[link_idx];
		Node* node_inp = &nodes[link->InputIdx];
		Node* node_out = &nodes[link->OutputIdx];
		ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->InputSlot);
		ImVec2 p2 = offset + node_out->GetInputSlotPos(link->OutputSlot);
		draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
	}

	// Display nodes
	for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
	{
		Node* node = &nodes[node_idx];
		ImGui::PushID(node->ID);
		ImVec2 node_rect_min = offset + node->Pos;

		// Display node contents first
		draw_list->ChannelsSetCurrent(1); // Foreground
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		ImGui::BeginGroup(); // Lock horizontal position
		ImGui::Text("%s", node->Name);
		ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
		ImGui::ColorEdit3("##color", &node->Color.x);
		ImGui::EndGroup();

		// Save the size of what we have emitted and whether any of the widgets are being used
		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
		node->Size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->Size;

		// Display node box
		draw_list->ChannelsSetCurrent(0); // Background
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->Size);
		if (ImGui::IsItemHovered())
		{
			node_hovered_in_scene = node->ID;
			open_context_menu |= ImGui::IsMouseClicked(ImGuiMouseButton_Right);
		}
		bool node_moving_active = ImGui::IsItemActive();
		if (node_widgets_active || node_moving_active)
			node_selected = node->ID;

		if (node_moving_active && !dragCheck)
		{
			for (int slot_idx = 0; slot_idx < node->OutputsCount; slot_idx++)
			{
				if (nodeLinkButtonCheck(offset + node->GetOutputSlotPos(slot_idx)))
				{
					outputNode = node;
					outputNodeSlot = slot_idx;
					outputIndex = node_idx;
					dragCheck = true;
					node_moving_active = false;
					linkType = "link";
					break;
				}
			}
		}

		if (node_moving_active && !dragCheck)
		{
			for (int slot_idx = 0; slot_idx < node->InputsCount; slot_idx++)
			{
				if (nodeLinkButtonCheck(offset + node->GetInputSlotPos(slot_idx)))
				{
					bool check = false;
					for (int i = 0; i < links.Size; ++i)
					{
						if (links[i].OutputIdx == node_idx && links[i].OutputSlot == slot_idx)
						{
							outputNode = &nodes[links[i].InputIdx];
							outputNodeSlot = links[i].InputSlot;
							outputIndex = links[i].InputIdx;
							dragCheck = true;
							node_moving_active = false;
							linkType = "unlink";
							check = true;
							links.erase(links.begin() + i);
							break;
						}
					}
					if (check)
					{
						break;
					}
				}
			}
		}

		if (node_moving_active && ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !dragCheck)
			node->Pos = node->Pos + io.MouseDelta;

		ImU32 node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID)) ? IM_COL32(75, 75, 75, 255) : IM_COL32(60, 60, 60, 255);
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
		for (int slot_idx = 0; slot_idx < node->InputsCount; slot_idx++)
			draw_list->AddCircleFilled(offset + node->GetInputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		for (int slot_idx = 0; slot_idx < node->OutputsCount; slot_idx++)
			draw_list->AddCircleFilled(offset + node->GetOutputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));

		ImGui::PopID();
	}

	if (dragCheck)
	{
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			draw_list->AddBezierCurve(offset + outputNode->GetOutputSlotPos(outputNodeSlot), offset + outputNode->GetOutputSlotPos(outputNodeSlot) + ImVec2(+50, 0), mousePos + ImVec2(-50, 0), mousePos, IM_COL32(200, 200, 100, 255), 3.0f);
		}
		else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			bool check = false;
			for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
			{
				for (int slot_idx = 0; slot_idx < nodes[node_idx].InputsCount; slot_idx++)
				{
					if (nodeLinkButtonCheck(offset + nodes[node_idx].GetInputSlotPos(slot_idx)))
					{
						links.push_back(NodeLink(outputIndex, outputNodeSlot, node_idx, slot_idx));
						check = true;
						break;
					}
				}
				if (check)
				{
					break;
				}
			}

			dragCheck = false;
		}
	}

	draw_list->ChannelsMerge();

	// Open context menu
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
	{
		if (/*ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) ||*/ !ImGui::IsAnyItemHovered())
		{
			node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
			open_context_menu = true;
		}
	}

	if (open_context_menu)
	{
		ImGui::OpenPopup("context_menu");
		if (node_hovered_in_list != -1)
			node_selected = node_hovered_in_list;
		if (node_hovered_in_scene != -1)
			node_selected = node_hovered_in_scene;
	}

	// Draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("context_menu"))
	{
		Node* node = node_selected != -1 ? &nodes[node_selected] : NULL;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node)
		{
			ImGui::Text("Node '%s'", node->Name);
			ImGui::Separator();
			if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
			if (ImGui::MenuItem("Delete", NULL, false, false)) {}
			if (ImGui::MenuItem("Copy", NULL, false, false)) {}
		}
		else
		{
			if (ImGui::MenuItem("Add")) { nodes.push_back(Node(nodes.Size, "New node", scene_pos, 0.5f, ImColor(100, 100, 200), 2, 2)); }
			if (ImGui::MenuItem("Paste", NULL, false, false)) {}
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{

	}

	// Scrolling
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f))
		scrolling = scrolling + io.MouseDelta;

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::EndGroup();

	ImGui::End();
}