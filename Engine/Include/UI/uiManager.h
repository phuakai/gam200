/* Start Header ************************************************************************/
/*!
\file		uiManager.h
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

#pragma once

#ifndef UI_H
#define UI_H

#include "ui.h"

namespace UIMgr
{
	#define mouseRad 1

	struct UIGroup
	{
		std::list<uiObj> uiList;
	};

	struct UIManager
	{
		enum class groupName : int
		{
			hud,
			actionPanel,
			map,
			unit1,
			building1
		};

		enum class uiType : int
		{
			uiBg,
			uiButton
		};

		enum class uiLocation : int
		{
			nil,
			hud,
			actionPanel,
			map
		};
		//rttr::registration::class_<ui>("ui")
		//	.property("group", &ui::group)
		//	.property("uiType", &ui::uiType) // is it uibg/uibutton? 
		//	.property("location", &ui::location)	// is it in hud/map/action panel? store into the respective list


		// CONSTRUCTOR
		/******************************************************************************/
		/*!
			Constructor for the UI manager
		*/
		/******************************************************************************/
										UIManager							();
		// CREATOR
		/******************************************************************************/
		/*!
			Creates the list of group consisting of all the different types of 
			units/buildings
		*/
		/******************************************************************************/
		void							createUiManager						();

		// ADDERS
		/******************************************************************************/
		/*!
			Adds a ui object to a ui group
		*/
		/******************************************************************************/
		void							addUiToActionGroup					(uiObj const& obj, groupName const& grp);
		//void							addUiToActionGroup					(uiObj const& obj, groupName const& grp);
		
		/******************************************************************************/
		/*!
			Adds a group to the action panel to be displayed
		*/
		/******************************************************************************/
		void							addActionGroupToDisplay				(UIGroup* grp);
		
		/******************************************************************************/
		/*!
			Adds a group to the action panel to be displayed
		*/
		/******************************************************************************/
		void							addUiToInfoList						(uiObj obj, groupName const& grp);
		
		/******************************************************************************/
		/*!
			Adds a ui object to the info panel to be displayed
		*/
		/******************************************************************************/
		void							addInfoDisplay						(uiObj* obj);

		// REMOVERS
		/******************************************************************************/
		/*!
			Removes an action group from the action group display list
		*/
		/******************************************************************************/
		void							removeActionGroupFromDisplay		(UIGroup * grp);

		/******************************************************************************/
		/*!
			Removes an action group from the action display list
		*/
		/******************************************************************************/
		void							removeUiFromInfoList				(uiObj obj);	// to be used when units "die"

		/******************************************************************************/
		/*!
			Removes a ui object from the info display list
		*/
		/******************************************************************************/
		void							removeInfoFromDisplay				();

		/******************************************************************************/
		/*!
			Checks for collision detection between the mouse and the ui objects
		*/
		/******************************************************************************/
		void							UIUpdate							();

		/******************************************************************************/
		/*!
			Clears the containers containing the ui
		*/
		/******************************************************************************/
		void							destroyUI							();

		// GETTERS
		/******************************************************************************/
		/*!
			Gets the list of groups in the ui action list
		*/
		/******************************************************************************/
		std::vector<UIGroup>&			getUiActionGroupList				();

		/******************************************************************************/
		/*!
			Gets the list of pointers to the displayed action group
		*/
		/******************************************************************************/
		std::vector<UIGroup*>			getUiActionDisplayList				();

		/******************************************************************************/
		/*!
			Gets the list of ui objects
		*/
		/******************************************************************************/
		std::vector<uiObj>&				getUiInfoList						();

		/******************************************************************************/
		/*!
			Gets the list of pointer to the ui objects displayed
		*/
		/******************************************************************************/
		std::vector<uiObj*>				getUiInfoDisplayList				();

	private:
		// Action Group (bottom right panel)
		std::vector<UIGroup>			uiActionGroupList;
		std::vector<UIGroup*>			uiActionDisplayList;
		vector2D::vec2D					actionDisplayDims;
		vector2D::vec2D					actionDisplayStartPos;
		vector2D::vec2D					actionDisplayGridDims;
		int								actionDisplayCol;
		int								actionDisplayRow;
		
		// Info (bottom center panel)
		std::vector<uiObj>				uiInfoList;
		std::vector<uiObj*>				uiInfoDisplayList;
		vector2D::vec2D					infoDisplayDims;
		vector2D::vec2D					infoDisplayStartPos;
		vector2D::vec2D					infoDisplayGridDims;
		int								infoDisplayCol;
		int								infoDisplayRow;

		// Minimap (bottom left panel)
		
		
		float							mapRatio;
		float							hudRatio;
		float							actionPanelRatio;

	};

	static UIManager UIMgr;

}

#endif