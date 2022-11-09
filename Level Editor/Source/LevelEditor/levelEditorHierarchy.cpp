#include "levelEditorHierarchy.h"
#include "physicsPartition.h"

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
			ecs.AddComponent<BaseInfo>(selected);
			ecs.AddComponent<Texture>(selected, 0, 1, 1, "");
			ecs.AddComponent<Render>(selected, "square", vector3D::vec3D(0.3f, 0.3f, 0.7f), 0, 0, 0, "instanceshader", true);
			ecs.GetComponent<BaseInfo>(selected)->name = "Entity";
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

					if (j == "Physics")
					{
						ecs.GetComponent<Physics>(selected)->formationManagerID = -1;
					}
					if (j == "BaseInfo")
					{
						ecs.GetComponent<BaseInfo>(selected)->name = "Entity";
					}
				}
				hierarchyList[std::make_pair(100000, 1)].push_back(selected);

				if (ecs.GetComponent<BaseInfo>(prefabs[i])->name == "Building")
				{
					ecs.GetComponent<BaseInfo>(selected)->type = EntityType::BUILDING;
				}
				else if (ecs.GetComponent<BaseInfo>(prefabs[i])->name == "Enemy")
				{
					ecs.GetComponent<BaseInfo>(selected)->type = EntityType::ENEMY;
				}
				else if (ecs.GetComponent<BaseInfo>(prefabs[i])->name == "Player")
				{
					ecs.GetComponent<BaseInfo>(selected)->type = EntityType::PLAYER;
				}
				
				if (ecs.GetComponent<BaseInfo>(selected)->type == EntityType::PLAYER || ecs.GetComponent<BaseInfo>(selected)->type == EntityType::ENEMY)
				{
					mainTree.insertSuccessfully(selected, ecs.GetComponent<BaseInfo>(selected)->position);
				}

				//addHealthBar(selected);
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
		std::vector<EntityID> listOfEntities = folder.second;
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

			//if (ImGui::BeginDragDropSource())
			//{
			//	dragTypeFrom = "Folder";
			//	vector3D::vec3D folderLocation{ (float)folder.first.first , (float)folder.first.second , (float)i };
			//	ImGui::SetDragDropPayload("dragEntity", &folderLocation, sizeof(vector3D::vec3D));
			//	//ImGui::Text("This is a drag and drop source");
			//	ImGui::EndDragDropSource();
			//}

			if (ImGui::BeginDragDropTarget())
			{
				dragTypeTo = "Folder";
				ImGuiDragDropFlags target_flags = 0;	//ImGuiDragDropFlags_AcceptBeforeDelivery;		// Don't wait until the delivery (release mouse button on a target) to do something
				//| ImGuiDragDropFlags_AcceptNoDrawDefaultRect;	// Don't display the yellow rectangle
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("dragEntity", target_flags))
				{
					vector3D::vec3D location = *(const vector3D::vec3D*)payload->Data;
					moveFromFolderID = (int)location.x;
					moveFromFolderLevel = (int)location.y;
					moveFrom = (int)location.z;
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
				if (!ecs.GetComponent<BaseInfo>(listOfEntities[j]) || ecs.GetComponent<BaseInfo>(listOfEntities[j])->type == EntityType::PREFAB)
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

				if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					ImGui::OpenPopup("delete");
				}

				if (ImGui::BeginPopup("delete"))
				{
					if (ImGui::Selectable("Delete"))
					{
						selected = -1;
						ecs.RemoveEntity(listOfEntities[j]);
						listOfEntities.erase(listOfEntities.begin() + j);
						break;
					}
					ImGui::EndPopup();
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
						moveFromFolderID = (int)location.x;
						moveFromFolderLevel = (int)location.y;
						moveFrom = (int)location.z;
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
			//if (dragTypeFrom == "Folder")
			//{
			//	if (dragTypeTo == "Folder")
			//	{
			//		// Finding children under selected folder
			//		auto it = hierarchyList.find(std::make_pair(moveFromFolderID, moveFromFolderLevel));
			//		auto itFrom = it;
			//		int numberOfChildren = 0;
			//		int lowestChildLevel = it->first.second;
			//		for (++it; it != hierarchyList.end(); ++it, ++numberOfChildren)
			//		{
			//			if (it->first.second <= moveFromFolderLevel)
			//			{
			//				break;
			//			}
			//			lowestChildLevel = it->first.second;
			//		}

			//		// Check if parent is dragged into child
			//		auto itTo = hierarchyList.find(std::make_pair(moveToFolderID, moveToFolderLevel));
			//		it = itFrom;
			//		bool check = false;
			//		if (moveFromFolderLevel < moveToFolderLevel)
			//		{
			//			for (int g = 0; g < numberOfChildren; ++g, ++it)
			//			{
			//				if (it->first.first == itTo->first.first)
			//				{
			//					check = true;
			//					break;
			//				}
			//			}
			//		}
			//		if (check)
			//		{
			//			continue;
			//		}

			//		// Check if moving is valid (there is a limit of 4 layers of folders)
			//		if (moveToFolderLevel + (lowestChildLevel - moveFromFolderLevel) >= 4)
			//		{
			//			moveFrom = -1;
			//			moveTo = -1;
			//			continue;
			//		}

			//		// Changing selected folderID
			//		// Unlinks the node in the map and returns the node handler
			//		auto folder = hierarchyList.extract(std::make_pair(moveFromFolderID, moveFromFolderLevel));
			//		int childID = moveToFolderID;
			//		int increment = (int)pow(10, 3 - moveToFolderLevel);
			//		do
			//		{
			//			// Checking if the folder already exists
			//			childID += increment;
			//		} 
			//		while (hierarchyList.find(std::make_pair(childID, moveToFolderLevel + 1)) != hierarchyList.end());

			//		folder.key().first = childID;
			//		folder.key().second = moveToFolderLevel + 1;
			//		hierarchyList.insert(std::move(folder));

			//		it = itFrom;
			//		int currentLevelOffset = moveToFolderLevel + 1;
			//		for (; numberOfChildren > 0; --numberOfChildren)
			//		{
			//			if (moveFromFolderID < moveToFolderLevel)
			//			{
			//				// Is now to child position in map
			//				++it;
			//			}

			//			auto childFolder = hierarchyList.extract(std::make_pair(it->first.first, it->first.second));
			//			//if (childFolder.key().second == 0) { break; 
			//			//	if (childFolder == 0) { break; }
			//			//std::cout << "child folder key" << childFolder.key().second << std::endl;
			//			childFolder.key().second = currentLevelOffset + (childFolder.key().second - moveFromFolderLevel);

			//			childID += (int)pow(10, 3 - childFolder.key().second);
			//			childFolder.key().first = childID;

			//			hierarchyList.insert(std::move(childFolder));
			//		}
			//	}
			//}
			//else // dragTypeFrom == "Entity"
			//{
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
			//}
			
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
}

void levelEditorHierarchy::clear()
{
	hierarchyList.clear();
	selected = -1;
	hierarchyList[std::make_pair(100000, 1)] = ecs.getEntities();
}

int levelEditorHierarchy::getSelected()
{
	return selected;
}
