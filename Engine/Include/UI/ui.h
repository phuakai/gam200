#pragma once

//#ifndef UI_H
//#define UI_H

#include "vec2D.h"
#include "../mainHeader.h"
#include "ECS.h"


namespace UI
{
	enum class uiState
	{
		release,
		hover,
		trigger,
		hold
	};


	struct uiObj
	{
	public:
		// Constructors
										uiObj();

		void							updateState(uiState _state);
		
		// Getters
		EntityID						getId();
		vector2D::vec2D					getPos();
		std::vector<vector2D::vec2D>	getVtx();
		float							getAspectRaio();
		uiState							getState();
		bool							getCollidable();

		// Setters
		void							setState(uiState _state);

	protected:
		EntityID						id;
		vector2D::vec2D					relPos;							//screen space coords, (0, 0) located at top left
		std::vector<vector2D::vec2D>	vtx;							//ui's vertices in world coords for collision check
		float							aspectRatio;					//used for screen resize
		uiState							state;
		int								layer;							//smaller layer no. = further from user
		bool							collidable;						//require collision checks
	};

	struct uiBg : public uiObj
	{
		// Constructors
										uiBg(EntityID _id, vector2D::vec2D pos, vector2D::vec2D dims);

	private:
		std::list<uiObj>				uiBgList;
	};

	struct uiButton : public uiObj
	{
		// Constructors
										uiButton(EntityID _id, vector2D::vec2D pos, vector2D::vec2D dims, int _layer);
	};
	
	struct uiSlider : public uiObj
	{

	private:
		int								step;			// % that slider moves if arrow key used to adjust slider
		int								min;
		int								max;
	};

	struct uiCheckBox : public uiObj
	{

	private:
		bool							isChecked;
	};

	struct uiProgressBar : public uiObj
	{

	private:
		int								step;			// % that the bar moves
		int								min;
		int								max;
	};

}

//#endif