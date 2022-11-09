/* Start Header ************************************************************************/
/*!
\file		ui.h
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

#include "vec2D.h"
#include "ECS.h"

namespace UIMgr
{

	enum class uiState
	{
		release,
		hover,
		trigger,
		hold
	}; // enum class uiState

	struct uiObj
	{
	public:
		// Constructors
										uiObj					();

		void							updateState				(uiState _state);
		
		// Getters
		EntityID						getId					();
		vector2D::vec2D					getPos					();
		std::vector<vector2D::vec2D>	getVtx					();
		float							getAspectRaio			();
		uiState							getState				();
		bool							getCollidable			();

		// Setters
		void							setState				(uiState _state);

	protected:
		EntityID						id;
		vector2D::vec2D					relPos;							//screen space coords, (0, 0) located at top left
		std::vector<vector2D::vec2D>	vtx;							//ui's vertices in world coords for collision check
		float							aspectRatio;					//used for screen resize
		uiState							state;
		int								layer;							//smaller layer no. = further from user
		bool							collidable;						//require collision checks
	}; // struct uiObj

	struct uiBg : public uiObj
	{
		// Constructors
										uiBg					(EntityID _id, vector2D::vec2D pos, vector2D::vec2D dims);

	private:
		std::list<uiObj>				uiBgList;
	}; // struct uiBg

	struct uiButton : public uiObj
	{
		// Constructors
										uiButton				(EntityID _id, vector2D::vec2D pos, vector2D::vec2D dims, int _layer);
	}; // struct uiButton
	
	struct uiSlider : public uiObj
	{

	private:
		int								step;			// % that slider moves if arrow key used to adjust slider
		int								min;
		int								max;
	}; // struct uiSlider

	struct uiCheckBox : public uiObj
	{

	private:
		bool							isChecked;
	}; // struct uiCheckBox

	struct uiProgressBar : public uiObj
	{

	private:
		int								step;			// % that the bar moves
		int								min;
		int								max;
	}; // struct uiProgressBar

} // namespace UI

