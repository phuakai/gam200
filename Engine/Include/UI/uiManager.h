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
		// Constructor
										UIManager();
		// Creator
		void							createGroupList();

		// Adders
		void							addUiToActionGroup(uiObj const& obj, UI::groupName const& grp);
		void							addActionGroupToDisplay(UIGroup* grp);
		void							addUiToInfoList(uiObj obj, groupName const& grp);
		void							addInfoDisplay(uiObj* obj);

		// Removers
		void							removeActionGroupFromDisplay(UIGroup * grp);
		void							removeUiFromInfoList(uiObj obj);	// to be used when units "die"
		void							removeInfoFromDisplay();

		void							UIUpdate();
		void							destroyUI();

		//Getters
		std::vector<UIGroup>&			getUiActionGroupList();
		std::vector<UIGroup*>			getUiActionDisplayList();
		std::vector<uiObj>&				getUiInfoList();
		std::vector<uiObj*>				getUiInfoDisplayList();

	private:
		// Action Group (bottom right panel)
		std::vector<UIGroup>			uiActionGroupList;
		std::vector<UIGroup*>			uiActionDisplayList;
		vector2D::vec2D					actionDisplayDims;
		vector2D::vec2D					actionDisplayStartPos;
		//int								actionDisplayWidth;
		//int								actionDisplayHeight;
		
		// Info (bottom center)
		std::vector<uiObj>				uiInfoList;
		std::vector<uiObj*>				uiInfoDisplayList;
		vector2D::vec2D					infoDisplayDims;
		vector2D::vec2D					infoDisplayStartPos;
		vector2D::vec2D					infoDisplayGridDims;
		int								infoDisplayWidth;
		int								infoDisplayHeight;

	};

	static UIManager UIMgr;

}

#endif