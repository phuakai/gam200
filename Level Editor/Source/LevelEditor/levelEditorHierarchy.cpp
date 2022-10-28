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

	/// Should be a function on its own: called AddFolder(int parent)
	if (ImGui::Button("+"))
	{
		// main parent ID starts from 1000, then 2000, 3000
		latestParentID += 1000;
		parent.push_back(latestParentID);
		listOfEntities.push_back(std::vector<EntityID>());
	}
	// Adding folder button end =============================================================================

	// Drawing folders & entities ===========================================================================
	int moveFrom = -1, moveTo = -1;
	int moveFromVector = -1, moveToVector = -1;
	int treeLevel = 0;
	static std::string dragTypeFrom = "", dragTypeTo = "";

	for (int i = 0; i < parent.size(); ++i)
	{
		ImGui::PushID(i);
		// Drawing Folder 
		bool nodeOpen = false;
		//static int nodeHovered = -1;

		if (parent[i] > 0)
		{
			ImGuiTreeNodeFlags nodeFlags =
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_SpanAvailWidth;

			nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, "File", i);
			++treeLevel;

			if (ImGui::BeginDragDropSource())
			{
				dragTypeFrom = "Folder";
				vector2D::vec2D entityLocation{ (float)i , 0.f };
				ImGui::SetDragDropPayload("dragEntity", &entityLocation, sizeof(vector2D::vec2D));
				//ImGui::Text("This is a drag and drop source");
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				dragTypeTo = "Folder";
				ImGuiDragDropFlags target_flags = 0;	//ImGuiDragDropFlags_AcceptBeforeDelivery;		// Don't wait until the delivery (release mouse button on a target) to do something
				//| ImGuiDragDropFlags_AcceptNoDrawDefaultRect;	// Don't display the yellow rectangle
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("dragEntity", target_flags))
				{
					vector2D::vec2D entityLocation = *(const vector2D::vec2D*)payload->Data;
					moveFromVector = entityLocation.x;
					moveFrom = entityLocation.y;
					moveToVector = i;
					moveTo = 0;
				}

				ImGui::EndDragDropTarget();
			}
			
		}
		
		if (nodeOpen || parent[i] == 0)
		{
			for (int j = 0; j < listOfEntities[i].size(); ++j)
			{
				ImGui::PushID(j);
				EntityID currentEntity = listOfEntities[i][j];
				const char* name = (ecs.GetComponent<BaseInfo>(currentEntity)->name).c_str();

				// Filtering of names
				if (filter.PassFilter(name))
				{
					if (ImGui::Selectable(name))
					{
						selected = currentEntity;
					}
				}

				if (ImGui::BeginDragDropSource())
				{
					dragTypeFrom = "Entity"; 
					vector2D::vec2D entityLocation{ (float)i , (float)j };
					ImGui::SetDragDropPayload("dragEntity", &entityLocation, sizeof(vector2D::vec2D));
					//ImGui::Text("This is a drag and drop source");
					ImGui::EndDragDropSource();
				}

				if (dragTypeFrom != "Folder" && ImGui::BeginDragDropTarget())
				{
					dragTypeTo = "Entity";

					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("dragEntity"))
					{
						vector2D::vec2D entityLocation = *(const vector2D::vec2D*)payload->Data;
						moveFromVector = entityLocation.x;
						moveFrom = entityLocation.y;
						moveToVector = i;
						moveTo = j;
					}

					ImGui::EndDragDropTarget();
				}

				ImGui::PopID();
			}
		}

		if (moveFrom != -1 && moveTo != -1)
		{
			// Reorder items
			if (dragTypeFrom == "Folder")
			{
				int copy_dst = (moveFrom < moveTo) ? moveFrom : moveTo + 1;
				int copy_src = (moveFrom < moveTo) ? moveFrom + 1 : moveTo;
				int copy_count = (moveFrom < moveTo) ? moveTo - moveFrom : moveFrom - moveTo;
				EntityID tmp = listOfEntities[i][moveFrom];
				memmove(&listOfEntities[i][copy_dst], &listOfEntities[i][copy_src], (size_t)copy_count * sizeof(EntityID));
				listOfEntities[i][moveTo] = tmp;
			}
			else // dragTypeFrom == "Entity"
			{
				if (dragTypeTo == "Folder")
				{
					// change 4 to macro
					int levelID = 4 - treeLevel - 1;

					if (levelID < 0)
					{
						continue;
					}

					int levelMask = pow(10, levelID);

					int moveFromVectorID = parent[moveFromVector];		// to move the children of this parent as well
					int moveToVectorID = parent[moveToVector];

					while (true)
					{
						// Checking if there are children in the parent to move into
						if (moveToVector + 1 < parent.size() && parent[moveToVector + 1] == moveToVectorID + levelMask)
						{
							++moveToVector;
							moveToVectorID += levelMask;
						}
						else
						{
							break;
						}
					}

					// Changing the folder to be under the parent folder, after its children 
					parent[moveFromVector] = moveToVectorID + levelMask;

					// Finding and changing the original folder's children
					// Finding the treeLevel of the original folder
					int moveFromLevelID = 0;
					bool check = false;
					while (true)
					{
						if (moveFromLevelID > 4)
						{
							check = true;
							break;
						}

						if (!(moveFromVectorID % (int)pow(10, moveFromLevelID + 1)))
						{
							++moveFromLevelID;
						}
						else
						{
							break;
						}
					}
					if (check)
					{
						continue;
					}

					moveFromLevelID /= 10;
					int moveFromLevelMask = pow(10, moveFromLevelID);
					auto nextFolderIndex = std::find(parent.begin(), parent.end(), moveFromLevelID + moveFromLevelMask);

					if (nextFolderIndex == parent.end())
					{
						continue;
					}

					int numberOfChildren = std::distance(parent.begin(), nextFolderIndex) - moveFromVector;

					//while (numberOfChildren) 
					//{

					//}
				}
				else // dragTypeTo == "Entity"
				{
					int copyDst, copySrc, copyCount;
					if (moveFromVector == moveToVector)
					{
						copyDst = (moveFrom < moveTo) ? moveFrom : moveTo + 1;
						copySrc = (moveFrom < moveTo) ? moveFrom + 1 : moveTo;
						copyCount = (moveFrom < moveTo) ? moveTo - moveFrom : moveFrom - moveTo;
					}
					else
					{
						int moveFromVectorID = parent[moveFromVector];
						int moveToVectorID = parent[moveToVector];
						int difference = abs(moveFromVectorID - moveToVectorID);
						
						while (true)
						{
							
						}
						
					}
					int copy_dst = (moveFrom < moveTo) ? moveFrom : moveTo + 1;
					int copy_src = (moveFrom < moveTo) ? moveFrom + 1 : moveTo;
					int copy_count = (moveFrom < moveTo) ? moveTo - moveFrom : moveFrom - moveTo;
					EntityID tmp = listOfEntities[i][moveFrom];
					memmove(&listOfEntities[i][copy_dst], &listOfEntities[i][copy_src], (size_t)copy_count * sizeof(EntityID));
					listOfEntities[i][moveTo] = tmp;
				}
			}
			
			ImGui::SetDragDropPayload("DND_DEMO_NAME", &moveTo, sizeof(EntityID)); // Update payload immediately so on the next frame if we move the mouse to an earlier item our index payload will be correct. This is odd and showcase how the DnD api isn't best presented in this example.
		}

		if (nodeOpen)
		{
			--treeLevel;
			ImGui::TreePop();
		}

		ImGui::PopID();
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
			ecs.RegisterEntity(temp);
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
