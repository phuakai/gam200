#include "app.h"
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
	 
	ImGui::Begin("Camera");
	ImVec2 size;
	size.x = ImGui::GetWindowWidth();
	size.y = ImGui::GetWindowHeight();
	auto tex = mainFrame.framebuffer;
	ImGui::Image((void*)tex, size);
	ImGui::End();

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


	// button to create entity
	//static char name[100]{ '\0' };
	//static bool checks[100] = { false };
	//static EntityID temp;
	//static Render* render;
	//static Texture* texture;
	//static Movement* movement;
	//static Stats* stats;
	//static std::vector<int> componentCheck(ecs.getAllRegisteredComponents().size(), 0);

//	if (ImGui::Button("Create Entity"))
//	{
//		if (counter & 1)
//		{
//			//if (name[0] != '\0')
//			// creating entity
//			//ecs.RegisterEntity(ecs.GetNewID(), name);
//
//			//for (int i = 0; i < 1; ++i)
//			//{
//			//	if (checks[i])
//			//		ecs.AddComponent<rttr::type::get_by_name(ecs.getAllRegisteredComponents()[i])>();
//			//}
//			if (name[0] == '\0' || render == nullptr)
//			{
//				checks[99] = true;
//			}
//
//			else
//			{
//				std::string nameToString(name);
//				render->name = nameToString;
//				ecs.setEntityName(temp, nameToString);
//
//				for (int i = 0; i < ecs.getAllRegisteredComponents().size(); ++i)
//					componentCheck[i] = 0;
//
//				memset(name, 0, sizeof(name));
//				checks[99] = false;
//				counter++;
//			}
//		}
//		else
//		{
//			temp = ecs.GetNewID();
//			ecs.RegisterEntity(temp, "In IMGUI");
//			std::cout << temp;
//			counter++;
//		}
//	}
//
//	if (counter & 1)
//	{
//		if (checks[99])
//			ImGui::Text("Please Input Entity Name");
//
//		ImGui::InputText("Entity Name", name, 100);
//
//		for (int i = 0; i < ecs.getAllRegisteredComponents().size(); ++i)
//		{
//			std::string componentName = ecs.getAllRegisteredComponents()[i];
//			//rttr::instance instance = rttr::instance::instance(componentList[i]);
//			rttr::type component = rttr::type::get_by_name(componentName);
//
//			// odd numbers, to join the 2nd checkbox in the same line
//			//if (i & 1)
//			//	ImGui::SameLine();
//
//			ImGui::Checkbox(ecs.getAllRegisteredComponents()[i].c_str(), &checks[i]);
//
//			if (checks[i])
//			{
//				// setting all of the components for the 1st time
//				if (!componentCheck[i])
//				{
//					std::cout << "test" << std::endl;
//					// i wanna make this not hardcoded :((
//					if (componentName == "Render")
//						render = ecs.AddComponent<Render>(temp);
//					else if (componentName == "Texture")
//						texture = ecs.AddComponent<Texture>(temp);
//					else if (componentName == "Movement")
//						movement = ecs.AddComponent<Movement>(temp);
//					else if (componentName == "Stats")
//						stats = ecs.AddComponent<Stats>(temp);
//
//					componentCheck[i] = 1;
//				}
//
//				if (componentName == "Render")
//				{
//					//static const char* types[] = { "square", "circle" };
//					//static int typeCurrent = 0;
//					//ImGui::Combo("Type", &typeCurrent, types, IM_ARRAYSIZE(types));
//					//render->type = types[typeCurrent];
//
//					ImGui::DragFloat2("Position", render->position.m, 1.0f, -500.0f, 500.0f);
//					ImGui::DragFloat3("Color", render->color.m, 0.01f, 0.0f, 1.0f);
//					ImGui::DragFloat2("Dimension", render->dimension.m, 0.5f, 0.0f, 500.0f);
//				}
//
//				else if (componentName == "Movement")
//				{
//					//static const char* types[] = { "square", "circle" };
//					//static int typeCurrent = 0;
//					//ImGui::Combo("Type", &typeCurrent, types, IM_ARRAYSIZE(types));
//					//render->type = types[typeCurrent];
//
///*					static std::vector<std::string> entities = ecs.getEntityNames();
//					static int currentItem = 0;*/ // Here we store our selection data as an index.
//					//	ImGui::ListBox("Target", &currentItem, 1, entities.data(), entities.size());
//
//					//static ImGuiComboFlags flags = 0;
//
//					//if (ImGui::BeginCombo("combo",combo_preview_value,flags)) {
//					//	for (int i = 0; i < entities.size(); ++i) {
//					//		const bool isSelected = (currentItem == i);
//					//		if (ImGui::Selectable(entities[i], isSelected)) {
//					//			currentItem = i;
//					//		}
//					//	}
//					//}
//					//ImGui::EndCombo;
//					//const char* list = VectorOfStringGetter(entities, entities.size());
//					//ImGui::Combo("Target", &currentItem, list, 3);
//					//ImGui::ListBox("Symbols", currentItem, VectorOfStringGetter((void*)&entities, (int)entities.size()), entities.size(), 3);
//					//ImGui::Combo("Example", &currentItem, entities);
//
//					//ImGui::DragFloat2("Target", render->position.m, 1.0f, -500.0f, 500.0f);
//					ImGui::DragFloat("Force", &movement->force, 0.01f, 0.0f, 1.0f);
//					ImGui::DragFloat("Speed", &movement->speed, 0.5f, 0.0f, 500.0f);
//				}
//
//
//				//std::string comp = ecs.getAllRegisteredComponents()[i];
//				//rttr::type theComp = rttr::type::get_by_name(ecs.getAllRegisteredComponents()[i]);
//
//				//rttr::instance IR = r.create();
//
//				//auto e =r.get_properties();
//				//auto q = *e.begin();
//
//				//std::cout << prop.get_name();
//				//std::cout << VR.get_type().get_name().to_string();
//				//rttr::instance instance = .get_type().get_raw_type().is_wrapper() ? componentList[i].get_wrapped_instance() : componentList[i];
//
//				//for (auto& property : component.get_properties())
//				//{
//					//std::cout << typeid(property.get_type().get_name().to_string()) << std::endl;
//					//rttr::property propertyType = instance.get_type().get_by_name(property.get_name()).get_property();
//
//					//if (property.get_type() == rttr::type::get<vector2D::vec2D>())
//					//{
//						//static vector2D::vec2D temp;
//						//ImGui::DragFloat2(property.get_name().data(), temp.m, 1.0f, -500.0f, 500.0f);
//
//						//instance.get_type().get_by_name(property.get_name()).set_property_value(property.get_name(), temp);
//
//						//std::cout << instance.get_type().get_property_value("position").to_string() << "test";
//						//std::cout << instance.get_type().get_name().to_string() << std::endl;
//
//					//}
//					//ImGui::InputText(property.get_name().data(), name, 100);
//				//}
//			}
//
//			else if (componentCheck[i])
//			{
//				// i wanna make this not hardcoded :((
//				if (componentName == "Render")
//					ecs.RemoveComponent<Render>(temp);
//				else if (componentName == "Texture")
//					ecs.RemoveComponent<Texture>(temp);
//				else if (componentName == "Movement")
//					ecs.RemoveComponent<Movement>(temp);
//				else if (componentName == "Stats")
//					ecs.RemoveComponent<Stats>(temp);
//
//				componentCheck[i] = 0;
//			}
//		}
//
//		if (ImGui::Button("Cancel"))
//		{
//			ecs.RemoveEntity(temp);
//			memset(name, 0, sizeof(name));
//			counter++;
//		}
//	}
//
//	//ImGui::TreeNode("THE REST");
//	/*
//	for (auto i : ecs.getEntities()) {
//		std::string str = ecs.getEntityName(i);
//		const char* c = str.c_str();
//		ImGui::Text(c);
//	for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end(); ++obj1)
//	{
//		if (obj1->first != "Camera")
//		{
//			obj1->second.update(Graphics::Input::delta_time);
//		}
//	}
//
//	for (std::map <std::string, GLObject>::iterator obj1 = objects.begin(); obj1 != objects.end() ; ++obj1)
//	{
//		for (GLuint i = 0; i < obj1->second.mdl_ref->second.posvtx_cnt; i++)
//		{
//			obj1->second.ndc_coords[i] = obj1->second.world_to_ndc_xform * obj1->second.worldVertices[i], 1.f;
//		}
//	}
//	*/
//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
//	ImGui::Text("Test");
//	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
//	ImGui::Checkbox("Another Window", &show_another_window);
//
//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
//
//	//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//	//	counter++;
//	ImGui::SameLine();
//	ImGui::Text("counter = %d", counter);
//
//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


	//if (show_another_window)
	//{
	//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//	ImGui::Text("Hello from another window!");
	//	if (ImGui::Button("Close Me"))
	//		show_another_window = false;
	//	ImGui::End();
	//}
	ImGui::Render();
}

void imguiDraw()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
