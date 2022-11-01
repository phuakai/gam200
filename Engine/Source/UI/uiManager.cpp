/* Start Header ************************************************************************/
/*!
\file		uiManager.cpp
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Oct 26, 2022
\brief		This file contains the function definitions collision detections and responses
			between circle and polygon objects

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "uiManager.h"
#include "mainHeader.h"
#include "input.h"
#include <iterator>
#include <algorithm>
#include "camera.h"
#include "../Physics/collision.h"

extern CameraNS::Camera2D camera2d;

namespace UI
{
	UIManager::UIManager()
	{
		actionDisplayCol = actionDisplayRow = infoDisplayCol = infoDisplayRow = 0;
		actionDisplayDims = actionDisplayStartPos = actionDisplayGridDims = infoDisplayDims = infoDisplayGridDims = infoDisplayStartPos = {};
	}

	void UIManager::createUiManager(float minimapRatio, float infoPanelRatio, float actionPanelRatio)
	{
		int camHeight = camera2d.getHeight();
		int camWidth = camera2d.getWidth();
		actionDisplayCol = 5;
		actionDisplayRow = 5;
		actionDisplayDims = { 5.f, 5.f };
		actionDisplayStartPos = { 0.f, 0.f };

		infoDisplayCol = 15;
		infoDisplayRow = 5;
		infoDisplayDims = { camWidth * infoPanelRatio * 0.9f, camHeight * 0.9f / 6.f };
		infoDisplayGridDims = { infoDisplayDims.x / static_cast<float>(infoDisplayCol), infoDisplayDims.y / static_cast<float>(infoDisplayRow) };
		infoDisplayStartPos = { -infoDisplayDims.x / 2.f,
								(-camHeight / 2.f + 900 / 5.f) + infoDisplayGridDims.y / 2.f };

		uiActionGroupList.resize(3); // no. of variable in enum groupname
	}

	void UIManager::addUiToActionGroup(uiObj const& obj, groupName const& grp)
	{
		uiActionGroupList[static_cast<int>(grp)].uiList.emplace_back(obj);
	}

	void UIManager::addActionGroupToDisplay(UIGroup * grp)
	{
		uiActionDisplayList.emplace_back(grp);
		for (auto& stateUpdater : uiActionGroupList[static_cast<int>(groupName::unit1)].uiList)
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
		currHeight = static_cast<int>(uiInfoDisplayList.size() / infoDisplayCol);
		currWidth = static_cast<int>(uiInfoDisplayList.size()) - infoDisplayCol * currHeight;
		vector2D::vec2D pos{ infoDisplayStartPos.x + currWidth * infoDisplayGridDims.x, infoDisplayStartPos.y - currHeight * infoDisplayGridDims.y };

		uiInfoDisplayList.emplace_back(obj);
		ecs.GetComponent<Render>(obj->getId())->render = true;
		ecs.GetComponent<BaseInfo>(obj->getId())->position = pos;
	}

	void UIManager::removeActionGroupFromDisplay(UIGroup* grp)
	{
		auto removeGrp = std::find(uiActionDisplayList.begin(), uiActionDisplayList.end(), grp);
		if (removeGrp != uiActionDisplayList.end())
		{
			for (auto& stateUpdater : uiActionGroupList[static_cast<int>(groupName::unit1)].uiList)
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
		Graphics::Input::getCursorScreenPos(&currMousePos);

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

