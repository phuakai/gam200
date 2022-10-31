/* Start Header ************************************************************************/
/*!
\file		fowOfWar.cpp
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

#include "fogOfWar.h"

namespace fow
{
	/************************************************************************************
	*
	*	FOG OF WAR TILE
	*
	************************************************************************************/

	direction& operator++(direction& dir) {
		if (dir == direction::ne)
			dir = direction::n;
		else
			dir = static_cast<direction>(static_cast<int>(dir) + 1);
		return dir;
	}

	direction operator++ (direction& dir, int)
	{
		direction tmp(dir);
		tmp = static_cast<direction>(static_cast<int>(dir) + 1);
		return tmp;
	}

	/**************************************************************************/
	/*!
		Constructor for the fog of war tile
	*/
	/**************************************************************************/
	fowTile::fowTile()
	{
		id = width = height = 0;
		worldPos = gridPos = vector2D::vec2D(0.f, 0.f);
		state = fowTileState::unvisited;
	}
	
	/**************************************************************************/
	/*!
		Constructor for the fog of war tile
	*/
	/**************************************************************************/
	fowTile::fowTile(int _width, int _height, vector2D::vec2D _worldPos, vector2D::vec2D _gridPos)
	{
		id = 0;
		width = _width;
		height = _height;
		worldPos = _worldPos;
		gridPos = _gridPos;
		state = fowTileState::unvisited;
	}

	/**************************************************************************/
	/*!
		Creator for the fog of war tile
	*/
	/**************************************************************************/
	void fowTile::createFowTile(int _width, int _height, vector2D::vec2D _worldPos, vector2D::vec2D _gridPos)
	{
		width = _width;
		height = _height;
		worldPos = _worldPos;
		gridPos = _gridPos;
		state = fowTileState::unvisited;
	}

	/**************************************************************************/
	/*!
		This function removes endities from the fog of war tile
	*/
	/**************************************************************************/
	void fowTile::removeFromTile(EntityID _id, vector2D::vec2D oldMapPos)
	{
		//remove entity from tile
		entitiesOnTile.remove(_id);
	}

	/**************************************************************************/
	/*!
		This function adds endities to the fog of war tile
	*/
	/**************************************************************************/
	void fowTile::addToTile(EntityID _id, std::list<vector2D::vec2D> LOS, vector2D::vec2D oldMapPos)
	{
		//add entity to tile
		entitiesOnTile.emplace_back(_id);
		//update states of surrounding tile
		state = fowTileState::visible;
	}

	/**************************************************************************/
	/*!
		This function updates the state of the tile to fogged (second layer)
	*/
	/**************************************************************************/
	void fowTile::updateTileStateToFog()
	{
		state = fowTileState::fogged;
	}

	/**************************************************************************/
	/*!
		This function updates the state of the tile to visible (first layer)
	*/
	/**************************************************************************/
	void fowTile::updateTileStateToVisible()
	{
		state = fowTileState::visible;
	}

	/**************************************************************************/
	/*!
		Getter for entity id on the tile
	*/
	/**************************************************************************/
	EntityID fowTile::getid()
	{
		return id;
	}

	/**************************************************************************/
	/*!
		Getter for tile width
	*/
	/**************************************************************************/
	int fowTile::getWdith()
	{
		return width;
	}

	int fowTile::getHeight()
	{
		return height;
	}

	vector2D::vec2D fowTile::getWorldPos()
	{
		return worldPos;
	}

	vector2D::vec2D fowTile::getGridPos()
	{
		return gridPos;
	}

	fowTileState fowTile::getTileState()
	{
		return state;
	}

	std::list<EntityID> fowTile::getEntitiesOnTile()
	{
		return entitiesOnTile;
	}

	void fowTile::setid(EntityID _id)
	{
		id = _id;
	}

	/************************************************************************************
	* 
	*	FOG OF WAR OBJECT
	* 
	************************************************************************************/
	fowObj::fowObj()
	{
		id = 0;
		worldPos = mapPos = vector2D::vec2D(0.f, 0.f);
	}

	fowObj::fowObj(EntityID _id, vector2D::vec2D _worldPos, vector2D::vec2D _gridPos, int col, int row)
	{
		id = _id;
		worldPos = _worldPos;
		mapPos = _gridPos;
		//std::cout << "this is position: " << mapPos.x << " " << mapPos.y << std::endl;
		updateTilePos(_gridPos, col, row);
	}

	void fowObj::fowObjUpdate(vector2D::vec2D const& newWorldPos, vector2D::vec2D const& newMapPos, int col, int row)
	{
		//Update the following:
		// (a) update frontier pos
		// (b) update LOS pos
		updateTilePos(newMapPos, col, row);
		// (c) world pos
		worldPos = newWorldPos;
		// (d) map pos
		mapPos = newMapPos;

	}

	void fowObj::updateTilePos(vector2D::vec2D const& currMapPos, int col, int row)
	{
		frontier.clear();
		LOS.clear();

		// floor the map pos
		float	W{ static_cast<float>(static_cast<int>(currMapPos.x) - visionRad) },
				E{ static_cast<float>(static_cast<int>(currMapPos.x) + visionRad) }, 
				S{ static_cast<float>(static_cast<int>(currMapPos.y) - visionRad) },
				N{ static_cast<float>(static_cast<int>(currMapPos.y) + visionRad) };

		if (mapPos.x < visionRad)
		{
			W = 0.f;
		}
		else if (mapPos.x >= col - visionRad)
		{
			E = static_cast<float>(col) - 1.f;
		}
		if (mapPos.y < visionRad)
		{
			S = 0.f;
		}
		else if (mapPos.y >= row - visionRad)
		{
			N = static_cast<float>(row) - 1.f;
		}

		// top row
		for (float c = W; c <= E; ++c)
		{
			frontier.emplace_back(vector2D::vec2D(c, N));
		}
		for (float r = N; r >= S; --r)
		{
			frontier.emplace_back(vector2D::vec2D(E, r));
		}
		// bottom row
		for (float c = E; c >= W; --c)
		{
			frontier.emplace_back(vector2D::vec2D(c, S));
		}
		// left column
		for (float r = S; r <= N; ++r)
		{
			frontier.emplace_back(vector2D::vec2D(W, r));
		}

		for (float r = S + 1; r <= N - 1; ++r)
		{
			for (float c = W + 1; c <= E - 1; ++c)
			{
				LOS.emplace_back(vector2D::vec2D(c, r));
			}
		}
	}

	EntityID fowObj::getid()
	{
		return id;
	}

	vector2D::vec2D fowObj::getWorldPos()
	{
		return worldPos;
	}

	vector2D::vec2D fowObj::getMapPos()
	{
		return mapPos;
	}

	std::list<vector2D::vec2D> fowObj::getFrontier()
	{
		return frontier;
	}

	std::list<vector2D::vec2D> fowObj::getLOS()
	{
		return LOS;
	}

#if false
#endif
}

