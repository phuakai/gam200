#include "levelEditorHierarchy.h"

std::vector<EntityID> prefabs;

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
	filter.Draw("", ImGui::GetWindowWidth() * 0.5f);
	ImGui::SameLine();
	if (ImGui::Button("Clear", { ImGui::GetWindowWidth() * 0.13f , 20 }))
	{
		filter.Clear();
	}

	// Adding folder button =================================================================================
	ImGui::SameLine();
	/// Should be a function on its own: called AddFolder(int parent)
	if (ImGui::Button("Create New", { ImGui::GetWindowWidth() * 0.25f , 20 }))
	{
		ImGui::OpenPopup("createNew");
	}

	if (ImGui::BeginPopup("createNew"))
	{
		if (ImGui::Selectable("New Folder"))
		{
			int folderID = 0;
			if (hierarchyList.size() > 1)
			{
				folderID = (++hierarchyList.rbegin())->first.first;
			}
			folderID = ((folderID / 1000) + 1) * 1000;
			std::vector<EntityID> entityList;
			hierarchyList[std::make_pair(folderID, 1)] = entityList;
		}
		ImGui::Separator();
		if (ImGui::Selectable("New Entity"))
		{
			selected = ecs.GetNewID();
			hierarchyList[std::make_pair(100000, 1)].push_back(selected);
		}
		for (int i = 0; i < prefabs.size(); i++)
		{
			if (ImGui::Selectable(ecs.GetComponent<BaseInfo>(prefabs[i])->name.c_str()))
			{
				selected = ecs.GetNewID();
				for (auto j : ecs.getEntityComponents(prefabs[i]))
				{
					rttr::type component = rttr::type::get_by_name(j);
					addComponentByName(component, selected);
				}
				hierarchyList[std::make_pair(100000, 1)].push_back(selected);
			}
		}
		ImGui::EndPopup();
	}
	// Adding folder button end =============================================================================

	// Drawing folders & entities ===========================================================================
	int moveFrom = -1, moveTo = -1;
	int moveFromFolderID = -1, moveToFolderID = -1;
	int moveFromFolderLevel = -1, moveToFolderLevel = -1;
	static std::string dragTypeFrom = "", dragTypeTo = "";
	int treeLevel = 0;
	bool parentNodeOpen = true;
	int returnLevel = 1;

	int i = 0;
	for (auto folder : hierarchyList)
	{
		if (folder.first.second > returnLevel && !parentNodeOpen)
		{
			continue;
		}

		// folderID = map < pair < folderID, folderLevel >, listOfEntities >
		// folderID
		ImGui::PushID(i);
		std::vector<EntityID>& listOfEntities = folder.second;
		// Drawing Folder 
		bool nodeOpen = false;

		if (folder.first.first < 100000)
		{
			ImGuiTreeNodeFlags nodeFlags =
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_SpanAvailWidth;

			nodeOpen = ImGui::TreeNodeEx((void*)(intptr_t)i, nodeFlags, std::to_string(folder.first.first).c_str());
			parentNodeOpen = nodeOpen;
			if (nodeOpen)
			{
				++treeLevel;
			}

			if (ImGui::BeginDragDropSource())
			{
				dragTypeFrom = "Folder";
				vector3D::vec3D folderLocation{ (float)folder.first.first , (float)folder.first.second , (float)i };
				ImGui::SetDragDropPayload("dragEntity", &folderLocation, sizeof(vector3D::vec3D));
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
					vector3D::vec3D location = *(const vector3D::vec3D*)payload->Data;
					moveFromFolderID = location.x;
					moveFromFolderLevel = location.y;
					moveFrom = location.z;
					moveToFolderID = folder.first.first;
					moveToFolderLevel = folder.first.second;
					moveTo = 0;
				}

				ImGui::EndDragDropTarget();
			}
		}
		
		if (nodeOpen || folder.first.first == 100000)
		{
			for (int j = 0; j < listOfEntities.size(); ++j)
			{
				if (ecs.GetComponent<BaseInfo>(listOfEntities[j])->type == "Prefab")
				{
					continue;
				}

				ImGui::PushID(j);
				EntityID currentEntity = listOfEntities[j];
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
					vector3D::vec3D entityLocation{ (float)folder.first.first , (float)folder.first.second , (float)j };
					ImGui::SetDragDropPayload("dragEntity", &entityLocation, sizeof(vector3D::vec3D));
					//ImGui::Text("This is a drag and drop source");
					ImGui::EndDragDropSource();
				}

				if (dragTypeFrom != "Folder" && ImGui::BeginDragDropTarget())
				{
					dragTypeTo = "Entity";

					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("dragEntity"))
					{
						vector3D::vec3D location = *(const vector3D::vec3D*)payload->Data;
						moveFromFolderID = location.x;
						moveFromFolderLevel = location.y;
						moveFrom = location.z;
						moveToFolderID = folder.first.first;
						moveToFolderLevel = folder.first.second;
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
				if (dragTypeTo == "Folder")
				{
					// Finding children under selected folder
					int numberOfChildren = 0;
					int lowestChildLevel = 0;
					for (auto it = ++(hierarchyList.find(std::make_pair(moveFromFolderID, moveFromFolderLevel))); it != hierarchyList.end(); ++it, ++numberOfChildren)
					{
						if (it->first.second <= moveFromFolderLevel)
						{
							break;
						}
						lowestChildLevel = it->first.second;
					}
					std::cout << numberOfChildren << std::endl;

					// Check if moving is valid (there is a limit of 4 layers of folders)
					if (lowestChildLevel - moveFromFolderLevel - moveToFolderLevel > 4)
					{
						std::cout << "the hell " << std::endl;
						moveFrom = -1;
						moveTo = -1;
						continue;
					}

					// Changing selected folderID
					// Unlinks the node in the map and returns the node handler
					auto initialFolderPosition = hierarchyList.find(std::make_pair(moveFromFolderID, moveFromFolderLevel));
					auto folder = hierarchyList.extract(std::make_pair(moveFromFolderID, moveFromFolderLevel));

					int childID = moveToFolderID;
					int increment = (int)pow(10, 3 - moveToFolderLevel);
					do
					{
						// Checking if the folder already exists
						childID += increment;
					} 
					while (hierarchyList.find(std::make_pair(childID, moveToFolderLevel + 1)) != hierarchyList.end());

					folder.key().first = childID;
					folder.key().second = moveToFolderLevel + 1;
					hierarchyList.insert(std::move(folder));

					initialFolderPosition = hierarchyList.find(std::make_pair(childID, moveToFolderLevel + 1));
					for (; numberOfChildren > 0; --numberOfChildren)
					{
						if (moveFromFolderID > moveToFolderLevel)
						{
							// Is now to child position in map
							++initialFolderPosition;
						}

						auto childFolder = hierarchyList.extract(std::make_pair(initialFolderPosition->first.first, initialFolderPosition->first.second));

						std::cout << childFolder.key().second << std::endl;
						childFolder.key().second = moveToFolderLevel + 1 + childFolder.key().second - moveFromFolderLevel;

						childID += (int)pow(10, 4 - childFolder.key().second);
						childFolder.key().first = childID;

						hierarchyList.insert(std::move(childFolder));
					}
				}
				//int copy_dst = (moveFrom < moveTo) ? moveFrom : moveTo + 1;
				//int copy_src = (moveFrom < moveTo) ? moveFrom + 1 : moveTo;
				//int copy_count = (moveFrom < moveTo) ? moveTo - moveFrom : moveFrom - moveTo;
				//EntityID tmp = listOfEntities[i][moveFrom];
				//memmove(&listOfEntities[i][copy_dst], &listOfEntities[i][copy_src], (size_t)copy_count * sizeof(EntityID));
				//listOfEntities[i][moveTo] = tmp;
			}
			else // dragTypeFrom == "Entity"
			{
				if (dragTypeTo == "Folder" || moveFromFolderID != moveToFolderID)
				{
					EntityID temp = hierarchyList[std::make_pair(moveFromFolderID, moveFromFolderLevel)][moveFrom];
					hierarchyList[std::make_pair(moveFromFolderID, moveFromFolderLevel)].erase(hierarchyList[std::make_pair(moveFromFolderID, moveFromFolderLevel)].begin() + moveFrom);
					hierarchyList[std::make_pair(moveToFolderID, moveToFolderLevel)].emplace(hierarchyList[std::make_pair(moveToFolderID, moveToFolderLevel)].begin() + moveTo, temp);
				}
				else // dragTypeTo == "Entity" && moveFromFolderID == moveToFolderID
				{
					int copy_dst = (moveFrom < moveTo) ? moveFrom : moveTo + 1;
					int copy_src = (moveFrom < moveTo) ? moveFrom + 1 : moveTo;
					int copy_count = (moveFrom < moveTo) ? moveTo - moveFrom : moveFrom - moveTo;
					EntityID tmp = hierarchyList[std::make_pair(moveFromFolderID, moveFromFolderLevel)][moveFrom];
					memmove(&hierarchyList[std::make_pair(moveFromFolderID, moveFromFolderLevel)][copy_dst], &hierarchyList[std::make_pair(moveFromFolderID, moveFromFolderLevel)][copy_src], (size_t)copy_count * sizeof(EntityID));
					hierarchyList[std::make_pair(moveFromFolderID, moveFromFolderLevel)][moveTo] = tmp;
				}
			}
			
			//ImGui::SetDragDropPayload("DND_DEMO_NAME", &moveTo, sizeof(EntityID)); // Update payload immediately so on the next frame if we move the mouse to an earlier item our index payload will be correct. This is odd and showcase how the DnD api isn't best presented in this example.
			
			moveFrom = -1;
			moveTo = -1;
		}

		// If there is a child, and the tree is opened, dont pop
		// Checking if there is a child
		bool child = false;
		returnLevel = folder.first.second;
		auto itAfter = hierarchyList.find(folder.first);
		if (++itAfter != hierarchyList.end())
		{
			if (itAfter->first.second > returnLevel)
			{
				child = true;
			}
			else if (!nodeOpen)
			{
				if (hierarchyList.find(std::make_pair(folder.first.first + pow(10, 4 - folder.first.second), folder.first.second)) != hierarchyList.end())
				{
					returnLevel = folder.first.second;
				}
				else
				{
					returnLevel = 1;
				}
			}
			else
			{
				returnLevel = itAfter->first.second;
			}
		}
		else
		{
			returnLevel = 0;
		}

		while (nodeOpen && !child)
		{
			ImGui::TreePop();
			--treeLevel;

			if (treeLevel < returnLevel)
			{
				break;
			}
		}

		ImGui::PopID();
		++i;
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
				hierarchyList[std::make_pair(0, 1)].push_back(temp);
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
