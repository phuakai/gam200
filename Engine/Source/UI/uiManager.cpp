#include "uiManager.h"
#include "input.h"
#include <iterator>
#include <algorithm>
#include "../Physics/collision.h"

namespace UI
{
	void UIManager::createGroupList()
	{
		uiGroupList.resize(3);
	}

	void UIManager::addUiToGroup(uiObj const& obj, groupName grp)
	{
		uiGroupList[grp].uiList.emplace_back(obj);
	}

	void UIManager::addGroupToDisplay(UIGroup * grp)
	{
		uiListDisplay.emplace_back(grp);
	}

	void UIManager::removeGroupFromDisplay(UIGroup* grp)
	{
		auto removeGrp = std::find(uiListDisplay.begin(), uiListDisplay.end(), grp);
		if (removeGrp != uiListDisplay.end())
			uiListDisplay.erase(removeGrp);
	}

	void UIManager::UIUpdate()
	{
		static vector2D::vec2D currMousePos{};
		Graphics::Input::getCursorPos(&currMousePos);

		if (Graphics::Input::keystateC)
		{
			//std::cout << "this is group list size before add: " << uiListDisplay.size() << std::endl;
			addGroupToDisplay(&getUiGroupList()[unit1]);
			for (auto& stateUpdater : uiGroupList[unit1].uiList)
			{
				ecs.GetComponent<Render>(stateUpdater.getId())->render = true;
			}
			//std::cout << "this is group list size after add: " << uiListDisplay.size() << std::endl;
		}

		if (Graphics::Input::keystateP)
		{
			//std::cout << "this is group list size before remove: " << uiListDisplay.size() << std::endl;
			for (auto& stateUpdater : uiGroupList[unit1].uiList)
			{
				ecs.GetComponent<Render>(stateUpdater.getId())->render = false;
			}
			removeGroupFromDisplay(&getUiGroupList()[unit1]);
			//std::cout << "this is group list size after remove: " << uiListDisplay.size() << std::endl;

			//std::cout << "this is group list size before add: " << uiListDisplay.size() << std::endl;
			addGroupToDisplay(&getUiGroupList()[building1]);
			for (auto& stateUpdater : uiGroupList[building1].uiList)
			{
				ecs.GetComponent<Render>(stateUpdater.getId())->render = true;
			}
			//std::cout << "this is group list size after add: " << uiListDisplay.size() << std::endl;
		}

		// check for collision btn mouse and ui
		if (uiListDisplay.size() > 1)
		{
			for (size_t i = 1 ; i < uiListDisplay.size() ; ++i)
			{
				// ui does not respond to inputs
				//if (uiListDisplay[i].uiList)
				//	continue;

				// ui responds to selection
				for (auto & obj : uiListDisplay[i]->uiList)
				{
					if (physics::CollisionDetectionCirclePolygon(vector2D::vec2D (static_cast<float>(currMousePos.x), 
																static_cast<float>(currMousePos.y)), 
																mouseRad, 
																obj.getVtx()))
					{
						// mouse collided with ui, action required
						//std::cout << "this is mouse state: " << int(Graphics::Input::mousestateLeft) << std::endl;
						if (Graphics::Input::mousestateLeft)
						{
							obj.updateState(uiState::trigger);
							std::cout << "do smth here\n";

							// Add group to display
							//addGroupToDisplay(&getUiGroupList()[groupName::unit1]);
							//for (auto& stateUpdater : uiListDisplay[unit1]->uiList)
							//{
							//	ecs.GetComponent<Render>(stateUpdater.getId())->render = true;
							//}
							// Remove group from display

						}
						else
						{
							obj.updateState(uiState::hover);
							//std::cout << "change texture here\n";
						}
					}
					else
					{
						obj.updateState(uiState::release);
					}
				}
			}
		}

	}

	void UIManager::destroyUI()
	{
		uiGroupList.clear();
		uiListDisplay.clear();
	}

	std::vector<UIGroup>& UIManager::getUiGroupList()
	{
		return uiGroupList;
	}

	std::vector<UIGroup*> UIManager::getUiListDisplay()
	{
		return uiListDisplay;
	}

}

