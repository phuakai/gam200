/* Start Header ************************************************************************/
/*!
\file		ui.cpp
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

#include "ui.h"

namespace UI
{
	uiObj::uiObj()
	{
		id = 0;
		relPos = { 0.f, 0.f };
		aspectRatio = 0.f;
		state = uiState::release;
		layer = 0;
		collidable = true;
	}

	//uiObj::uiObj(EntityID _id, vector2D::vec2D _pos, vector2D::vec2D dims)
	//{
	//	id = _id;
	//	relPos = _pos;
	//	aspectRatio = 0;
	//	state = uiState::release;
	//}

	EntityID uiObj::getId()
	{
		return id;
	}

	vector2D::vec2D uiObj::getPos()
	{
		return relPos;
	}

	std::vector<vector2D::vec2D> uiObj::getVtx()
	{
		return vtx;
	}

	float uiObj::getAspectRaio()
	{
		return aspectRatio;
	}

	uiState uiObj::getState()
	{
		return state;
	}

	bool uiObj::getCollidable()
	{
		return collidable;
	}

	void uiObj::setState(uiState _state)
	{
		state = _state;
	}

	void uiObj::updateState(uiState _state)
	{
		state = _state;
	}

	uiBg::uiBg(EntityID _id, vector2D::vec2D pos, vector2D::vec2D dims)
	{
		id = _id;
		aspectRatio = dims.x / dims.y;
		//relPos = { 0.f, 0.f };
		collidable = false;
		vtx.emplace_back(vector2D::vec2D(pos.x - dims.x, pos.y - dims.y));
		vtx.emplace_back(vector2D::vec2D(pos.x + dims.x, pos.y - dims.y));
		vtx.emplace_back(vector2D::vec2D(pos.x + dims.x, pos.y + dims.y));
		vtx.emplace_back(vector2D::vec2D(pos.x - dims.x, pos.y + dims.y));
	}
	
	uiButton::uiButton(EntityID _id, vector2D::vec2D pos, vector2D::vec2D dims, int _layer)
	{
		id = _id;
		aspectRatio = dims.x / dims.y;
		//relPos = { 0.f, 0.f };
		layer = _layer;
		vtx.emplace_back(vector2D::vec2D(pos.x - dims.x, pos.y - dims.y));
		vtx.emplace_back(vector2D::vec2D(pos.x + dims.x, pos.y - dims.y));
		vtx.emplace_back(vector2D::vec2D(pos.x + dims.x, pos.y + dims.y));
		vtx.emplace_back(vector2D::vec2D(pos.x - dims.x, pos.y + dims.y));
	}

} // namespace UI

