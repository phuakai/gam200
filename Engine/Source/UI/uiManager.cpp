#include "uiManager.h"
#include "input.h"
#include <iterator>
#include "../Physics/collision.h"

namespace UI
{
	//void UIManager::addUI(uiObj const& obj)
	//{
	//	UIMgr.uiListDisplay.emplace_back(obj);
	//}
	void UIManager::createGroupList()
	{
		uiGroupList.resize(3);
	}

	void UIManager::addUiToGroup(uiObj const& obj, UI::groupName grp)
	{
		uiGroupList[grp].uiList.emplace_back(obj);
	}

	void UIManager::addGroupToDisplay(UIGroup * grp)
	{
		uiListDisplay.emplace_back(grp);
	}

	void UIManager::UIUpdate()
	{
		static vector2D::vec2D currMousePos{};
		Graphics::Input::getCursorPos(&currMousePos);

		if (Graphics::Input::keystateC)
		{
			addGroupToDisplay(&getUiGroupList()[groupName::unit1]);
			for (auto& stateUpdater : uiListDisplay[unit1]->uiList)
			{
				ecs.GetComponent<Render>(stateUpdater.getId())->render = true;
			}
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
							std::cout << "change texture here\n";
						}
					}
					else
					{
						obj.updateState(uiState::release);
					}
					//	std::cout << "fix it!\n";

					//if ()
					//{

					//}
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

}

