#pragma once

#ifndef UI_H
#define UI_H

#include "ui.h"
#include "../mainHeader.h"


namespace UI
{
	#define mouseRad 1

	enum groupName
	{
		base,
		unit1,
		building1
	};

	struct UIGroup
	{
		std::list<uiObj> uiList;
	};

	struct UIManager
	{
		//void addUI(uiObj const& obj);
		void createGroupList();
		void addUiToGroup(uiObj const& obj, UI::groupName grp);
		void addGroupToDisplay(UIGroup * grp);
		void UIUpdate();
		void destroyUI();

		std::vector<UIGroup>& getUiGroupList();
	private:
		//std::list<std::pair<groupName, UIGroup>> uiGroupList;
		std::vector<UIGroup> uiGroupList;
		std::vector<UIGroup*> uiListDisplay;
	};

	static UIManager UIMgr;

}

#endif