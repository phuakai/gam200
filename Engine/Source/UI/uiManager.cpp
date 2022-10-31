#include "uiManager.h"
#include "input.h"
#include <iterator>
#include <algorithm>
#include "../Physics/collision.h"

namespace UI
{
	UIManager::UIManager()
	{
		//actionDisplayWidth = 5;
		//actionDisplayHeight = 5;
		actionDisplayDims = { 5.f, 5.f };
		actionDisplayStartPos = { 0.f, 0.f };

		infoDisplayWidth = 15;
		infoDisplayHeight = 5;
		infoDisplayDims = { 1000, 800 / 6.f };
		infoDisplayGridDims = { infoDisplayDims.x / static_cast<float>(infoDisplayWidth), infoDisplayDims.y / static_cast<float>(infoDisplayHeight) };
		infoDisplayStartPos = { - infoDisplayDims.x / 2.f,
								(-900 / 2.f + 900 / 5.f) + infoDisplayGridDims.y / 2.f };
	}

	void UIManager::createGroupList()
	{
		uiActionGroupList.resize(3);
	}

	void UIManager::addUiToActionGroup(uiObj const& obj, groupName const& grp)
	{
		uiActionGroupList[grp].uiList.emplace_back(obj);
	}

	void UIManager::addActionGroupToDisplay(UIGroup * grp)
	{
		uiActionDisplayList.emplace_back(grp);
		for (auto& stateUpdater : uiActionGroupList[unit1].uiList)
		{
			ecs.GetComponent<Render>(stateUpdater.getId())->render = true;
		}

	}

	void UIManager::addUiToInfoList(uiObj obj, groupName const& grp)
	{
		uiInfoList.emplace_back(obj);
	}

	void UIManager::addInfoDisplay(uiObj* obj)
	{
		// Translate pos of info icon
		int currWidth{}, currHeight{};
		currHeight = static_cast<int>(uiInfoDisplayList.size() / infoDisplayWidth);
		std::cout << "is height 0? " << uiInfoDisplayList.size() << ", " << infoDisplayGridDims.x << std::endl;
		currWidth = uiInfoDisplayList.size() - infoDisplayWidth * currHeight;
		vector2D::vec2D pos{ infoDisplayStartPos.x + currWidth * infoDisplayGridDims.x, infoDisplayStartPos.y - currHeight * infoDisplayGridDims.y };

		//std::cout << "whats this dims? " << currHeight << ", " << currWidth << std::endl;
		std::cout << "whats this pos? " << pos.x << ", " << pos.y << std::endl;
		//std::cout << "whats this display pos? " << infoDisplayStartPos.x << ", " << infoDisplayStartPos.y << std::endl;
		
		uiInfoDisplayList.emplace_back(obj);
		ecs.GetComponent<Render>(obj->getId())->render = true;
		ecs.GetComponent<BaseInfo>(obj->getId())->position = pos;
	}

	void UIManager::removeActionGroupFromDisplay(UIGroup* grp)
	{
		auto removeGrp = std::find(uiActionDisplayList.begin(), uiActionDisplayList.end(), grp);
		if (removeGrp != uiActionDisplayList.end())
		{
			for (auto& stateUpdater : uiActionGroupList[unit1].uiList)
			{
				ecs.GetComponent<Render>(stateUpdater.getId())->render = false;
			}

			uiActionDisplayList.erase(removeGrp);
		}
	}

	void UIManager::removeUiFromInfoList(uiObj obj)
	{
		//uiObj removeGrp = std::find(uiInfoList.begin(), uiInfoList.end(), obj);
		//if (removeGrp != uiInfoList.end())
		//	uiInfoList.erase(removeGrp);
	}

	void UIManager::removeInfoFromDisplay()
	{
		for (auto& stateUpdater : uiInfoDisplayList)
		{
			ecs.GetComponent<Render>(stateUpdater->getId())->render = false;
		}

		uiInfoDisplayList.clear();
	}

	void UIManager::UIUpdate()
	{
		static vector2D::vec2D currMousePos{};
		Graphics::Input::getCursorPos(&currMousePos);

		//if (Graphics::Input::keystateC)
		//{
		//	addActionGroupToDisplay(&getUiActionGroupList()[unit1]);
		//}
		//if (Graphics::Input::keystateP)
		//{
		//	removeActionGroupFromDisplay(&getUiActionGroupList()[unit1]);
		//	addActionGroupToDisplay(&getUiActionGroupList()[building1]);
		//}



		// check for collision btn mouse and ui
		if (uiActionDisplayList.size() > 1)
		{
			for (size_t i = 1 ; i < uiActionDisplayList.size() ; ++i)
			{
				// ui does not respond to inputs
				//if (uiListDisplay[i].uiList)
				//	continue;

				// ui responds to selection
				for (auto & obj : uiActionDisplayList[i]->uiList)
				{
					if (physics::CollisionDetectionCirclePolygon(vector2D::vec2D (static_cast<float>(currMousePos.x), 
																static_cast<float>(currMousePos.y)), 
																mouseRad, 
																obj.getVtx()))
					{
						// mouse collided with ui, action required
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
		uiActionGroupList.clear();
		uiActionDisplayList.clear();
		uiInfoList.clear();
		uiInfoDisplayList.clear();
	}

	std::vector<UIGroup>& UIManager::getUiActionGroupList()
	{
		return uiActionGroupList;
	}

	std::vector<UIGroup*> UIManager::getUiActionDisplayList()
	{
		return uiActionDisplayList;
	}

	std::vector<uiObj>& UIManager::getUiInfoList()
	{
		return uiInfoList;
	}

	std::vector<uiObj*> UIManager::getUiInfoDisplayList()
	{
		return uiInfoDisplayList;
	}

}

