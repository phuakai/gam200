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
		// Creator
		void							createGroupList();

		// Adders
		void							addUiToGroup(uiObj const& obj, groupName grp);
		void							addGroupToDisplay(UIGroup* grp);

		// Removers
		void							removeGroupFromDisplay(UIGroup * grp);


		void							UIUpdate();
		void							destroyUI();

		//Getters
		std::vector<UIGroup>&			getUiGroupList();
		std::vector<UIGroup*>			getUiListDisplay();

	private:
		std::vector<UIGroup>			uiGroupList;
		std::vector<UIGroup*>			uiListDisplay;
	};

	static UIManager UIMgr;

}

#endif