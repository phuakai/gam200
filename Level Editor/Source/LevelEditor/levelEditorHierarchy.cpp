#include "levelEditorHierarchy.h"

levelEditorHierarchy& levelEditorHierarchy::getInstance()
{
	static levelEditorHierarchy instance;
	return instance;
}

void levelEditorHierarchy::ImGuiHierarchy()
{
	// Helper class to easy setup a text filter.
	// You may want to implement a more feature-full filtering scheme in your own application.
	static ImGuiTextFilter filter;
	filter.Draw("", ImGui::GetWindowWidth() * 0.73);
	ImGui::SameLine(ImGui::GetWindowWidth() * 0.77);
	ImGui::Button("Clear");

	// Adding folder button =================================================================================
	ImGui::SameLine(ImGui::GetWindowWidth() * 0.9);
	if (ImGui::Button("+"))
	{
		/// Should be a function on its own: called AddFolder(int parent)
		// Adding a new folder, without a parent (0)
		parent.push_back(0);
		listOfEntities.push_back(std::vector<EntityID>());
	}
	// Adding folder button end =============================================================================

	// Drawing folders & entities ===========================================================================
	//static bool dragCheck = false;
	//static int dragEntityNo = -1;
	//static EntityID dragEntity = -1;
	//static int dragEntityVector = -1;
	//static int dragEntityVectorTo = -1;
	//static int dragType = -1;				// 0 - Entity, 1 - File

	int moveFrom = -1, moveTo = -1;
	int moveFromVector = -1, moveToVector = -1;
	static std::string dragType = "";

	for (int i = 0; i < parent.size(); ++i)
	{
		// Drawing Folder 
		bool nodeOpen = false;
		//static int nodeHovered = -1;

		if (parent[i] >= 0)
		{
			ImGuiTreeNodeFlags nodeFlags =
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_SpanAvailWidth;

			//if (dragCheck && nodeHovered == i)
			//{
			//	dragEntityVectorTo = i;
			//}

			nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "File", i);

			//if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
			//{
			//	nodeHovered = i;
			//}
			
			//else if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
			//{
			//	int dragTo = j + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);

			//	dragEntityNo = j;
			//	dragEntityVector = i;
			//	dragEntity = listOfEntities[i][j];
			//	dragCheck = true;
			//	dragType = 1;
			//	ImGui::ResetMouseDragDelta();
			//}

			if (ImGui::BeginDragDropSource())
			{
				dragType = "Folder";
				ImGui::SetDragDropPayload("ENTITYDRAG", &i, sizeof(EntityID));
				ImGui::Text("This is a drag and drop source");
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				ImGuiDragDropFlags target_flags = 0;	//ImGuiDragDropFlags_AcceptBeforeDelivery;		// Don't wait until the delivery (release mouse button on a target) to do something
				//| ImGuiDragDropFlags_AcceptNoDrawDefaultRect;	// Don't display the yellow rectangle
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITYDRAG", target_flags))
				{
					moveFrom = *(const EntityID*)payload->Data;
					moveTo = i;
				}

				ImGui::EndDragDropTarget();
			}
			
		}
		
		if (nodeOpen || parent[i] == -1)
		{
			for (int j = 0; j < listOfEntities[i].size(); ++j)
			{
				EntityID currentEntity = listOfEntities[i][j];
				const char* name = (ecs.GetComponent<BaseInfo>(currentEntity)->name).c_str();

				// Filtering of names
				if (filter.PassFilter(name))
				{
					if (ImGui::Selectable(name))
					{
						selected = currentEntity;
					}

					//else if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
					//{
					//	int dragTo = j + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
					//	if (dragTo >= 0 && dragTo < listOfEntities[i].size())
					//	{
					//		listOfEntities[i][j] = listOfEntities[i][dragTo];
					//		listOfEntities[i][dragTo] = currentEntity;
					//		ImGui::ResetMouseDragDelta();
					//	}
					//	else
					//	{
					//		dragEntityNo = j;
					//		dragEntityVector = i;
					//		dragEntity = listOfEntities[i][j];
					//		dragCheck = true;
					//		dragType = 0;
					//		ImGui::ResetMouseDragDelta();
					//	}
					//}
					//else if (!ImGui::IsItemActive() && dragEntity == listOfEntities[i][j] && dragEntityVector != -1)
					//{
					//	listOfEntities[dragEntityVector].erase(listOfEntities[dragEntityVector].begin() + dragEntityNo);
					//	listOfEntities[dragEntityVectorTo].push_back(dragEntity);

					//	dragEntity = -1;
					//	dragEntityVector = -1;
					//	nodeHovered = -1;
					//	dragCheck = false;
					//}
				}

				if (ImGui::BeginDragDropSource())
				{
					dragType = "Entity";
					ImGui::SetDragDropPayload("ENTITYDRAG", &j, sizeof(EntityID));
					ImGui::Text("This is a drag and drop source");
					ImGui::EndDragDropSource();
				}

				if (ImGui::BeginDragDropTarget())
				{
					ImGuiDragDropFlags target_flags = 0;	//ImGuiDragDropFlags_AcceptBeforeDelivery;		// Don't wait until the delivery (release mouse button on a target) to do something
					//| ImGuiDragDropFlags_AcceptNoDrawDefaultRect;	// Don't display the yellow rectangle
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITYDRAG", target_flags))
					{
						moveFrom = *(const EntityID*)payload->Data;
						moveTo = j;
					}

					ImGui::EndDragDropTarget();
				}
			}
		}

		if (moveFrom != -1 && moveTo != -1)
		{
			// Reorder items
			if (dragType == "Folder")
			{
				int copy_dst = (moveFrom < moveTo) ? moveFrom : moveTo + 1;
				int copy_src = (moveFrom < moveTo) ? moveFrom + 1 : moveTo;
				int copy_count = (moveFrom < moveTo) ? moveTo - moveFrom : moveFrom - moveTo;
				EntityID tmp = listOfEntities[i][moveFrom];
				memmove(&listOfEntities[i][copy_dst], &listOfEntities[i][copy_src], (size_t)copy_count * sizeof(EntityID));
				listOfEntities[i][moveTo] = tmp;
			}
			else // dragType == "Entity"
			{
				int copy_dst = (moveFrom < moveTo) ? moveFrom : moveTo + 1;
				int copy_src = (moveFrom < moveTo) ? moveFrom + 1 : moveTo;
				int copy_count = (moveFrom < moveTo) ? moveTo - moveFrom : moveFrom - moveTo;
				EntityID tmp = listOfEntities[i][moveFrom];
				memmove(&listOfEntities[i][copy_dst], &listOfEntities[i][copy_src], (size_t)copy_count * sizeof(EntityID));
				listOfEntities[i][moveTo] = tmp;
			}
			
			ImGui::SetDragDropPayload("DND_DEMO_NAME", &moveTo, sizeof(EntityID)); // Update payload immediately so on the next frame if we move the mouse to an earlier item our index payload will be correct. This is odd and showcase how the DnD api isn't best presented in this example.
		}

		if (nodeOpen)
		{
			ImGui::TreePop();
		}
	}
	// Drawing folders & entities end =======================================================================


	// button to create entity
	static int counter = 0;
	static char name[100]{ '\0' };
	static EntityID temp;
	static std::vector<int> componentCheck(ecs.getAllRegisteredComponents().size(), 0);

	if (ImGui::Button("Create Entity"))
	{
		if (counter & 1)
		{
			bool check = false;
			for (int i = 0; i < componentCheck.size(); ++i)
			{
				if (componentCheck[i])
				{
					check = true;
					break;
				}
			}

			if (check)
			{
				///std::string nameToString(name);
				///render->name = nameToString;
				///ecs.setEntityName(temp, nameToString);

				for (int i = 0; i < ecs.getAllRegisteredComponents().size(); ++i)
					componentCheck[i] = 0;

				memset(name, 0, sizeof(name));
				counter++;
				listOfEntities[0].push_back(temp);
			}
		}

		else
		{
			temp = ecs.GetNewID();
			ecs.RegisterEntity(temp, "In IMGUI");
			std::cout << temp;
			counter++;
		}
	}

	if (counter & 1)
	{
		for (int i = 0; i < ecs.getAllRegisteredComponents().size(); ++i)
		{
			std::string componentName = ecs.getAllRegisteredComponents()[i];
			bool check = componentCheck[i];
			ImGui::Checkbox(ecs.getAllRegisteredComponents()[i].c_str(), &check);

			if (check)
			{
				if (!componentCheck[i])
				{
					if (componentName == "Render")
						ecs.AddComponent<Render>(temp);
					else if (componentName == "BaseInfo")
						ecs.AddComponent<Texture>(temp);
					else if (componentName == "Texture")
						ecs.AddComponent<Texture>(temp);
					else if (componentName == "Physics")
						ecs.AddComponent<Physics>(temp);
					//else if (componentName == "Stats")
					//	ecs.AddComponent<Stats>(temp);

					componentCheck[i] = 1;
				}

				rttr::type component = rttr::type::get_by_name(componentName);
				rttr::instance componentInstance = GetComponentByName(component, temp);

				for (rttr::property property : component.get_properties())
				{
					if (property.get_type() == rttr::type::get<int>())
					{
						int temp = property.get_value(componentInstance).to_int();
						ImGui::DragInt(property.get_name().data(), &temp, 1.f, 0, 0);
						property.set_value(componentInstance, temp);
					}

					else if (property.get_type() == rttr::type::get<vector2D::vec2D>())
					{
						vector2D::vec2D temp = property.get_value(componentInstance).get_value<vector2D::vec2D>();
						ImGui::DragFloat2(property.get_name().data(), temp.m, 1.0f, -500.0f, 500.0f);
						property.set_value(componentInstance, temp);
					}

					else if (property.get_type() == rttr::type::get<vector3D::vec3D>())
					{
						vector3D::vec3D temp = property.get_value(componentInstance).get_value<vector3D::vec3D>();
						ImGui::DragFloat3(property.get_name().data(), temp.m, 0.01f, 0.0f, 1.0f);
						property.set_value(componentInstance, temp);
					}

					else if (property.get_type() == rttr::type::get<std::string>())
					{
						char tempChar[100];
						std::string temp = property.get_value(componentInstance).to_string();

						strcpy_s(tempChar, temp.c_str());

						ImGui::InputText(property.get_name().data(), tempChar, 100);
						property.set_value(componentInstance, std::string(tempChar));
					}
				}

				if (ecs.GetComponent<BaseInfo>(temp) != nullptr)
					std::cout << ecs.GetComponent<BaseInfo>(temp)->name << std::endl;
			}

			else if (componentCheck[i])
			{
				if (componentName == "Render")
					ecs.RemoveComponent<Render>(temp);
				else if (componentName == "BaseInfo")
					ecs.AddComponent<Texture>(temp);
				else if (componentName == "Texture")
					ecs.RemoveComponent<Texture>(temp);
				else if (componentName == "Physics")
					ecs.RemoveComponent<Physics>(temp);
				//else if (componentName == "Stats")
				//	ecs.RemoveComponent<Stats>(temp);

				componentCheck[i] = 0;
			}
		}

		if (ImGui::Button("Cancel"))
		{
			ecs.RemoveEntity(temp);
			memset(name, 0, sizeof(name));
			counter++;
		}
	}
}

int levelEditorHierarchy::getSelected()
{
	return selected;
}
