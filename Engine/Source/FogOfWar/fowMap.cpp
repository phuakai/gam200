
#pragma once

#ifndef FOWMAP_H
#define FOWMAP_H

#include "fowMap.h"

//#if false

namespace fow
{
	fogOfWarMap::fogOfWarMap()
	{
		width = height = col = row = dims = 0;
	}

	fogOfWarMap::fogOfWarMap(int const& _width, int const& _height, int const& _col, int const& _row, vector2D::vec2D _worldPos)
	{
		width = _width;
		height = _height;
		col = _col;
		row = _row;
		dims = col * row;
		worldPos = _worldPos;
	}

	vector2D::vec2D fogOfWarMap::worldToMap(vector2D::vec2D const& worldPos)
	{
		return vector2D::vec2D((worldPos.x + width/2) / width * col, (worldPos.y + height/2) / height * row);
	}

	vector2D::vec2D fogOfWarMap::mapToWorld(vector2D::vec2D const& mapPos)
	{
		return vector2D::vec2D(worldPos.x / col * width + width / 2, worldPos.y / row * height + height /2);
	}

	fogOfWarMap::~fogOfWarMap() {}

	void fogOfWarMap::createFow()
	{
		int tileWidth = width / col;
		int tileHeight = height / row;
		vector2D::vec2D startingPos = vector2D::vec2D(worldPos.x - width / 2 + tileWidth / 2, worldPos.y - height / 2 + tileHeight / 2);
		vector2D::vec2D currPos = startingPos;
		//vector2D::vec2D currMapPos = worldToMap(currPos);
		fowTileMap.emplace_back(fowTile(tileWidth, tileHeight, currPos, worldToMap(currPos)));

		for (int i = 1 ; i < dims ; ++i)
		{
			currPos = (i * tileWidth % width == 0) ? vector2D::vec2D(startingPos.x, currPos.y + tileHeight) : vector2D::vec2D(currPos.x + tileWidth, currPos.y);
			//currMapPos = worldToMap(currPos);
			fowTileMap.emplace_back(fowTile(tileWidth, tileHeight, currPos, worldToMap(currPos)));
		}
	}

	void fogOfWarMap::addObjToFow(fowObj obj)
	{
		fowObjList.emplace_back(obj);
	}

	void fogOfWarMap::updateFow()
	{
		// Change all visible tiles to fogged
		for (std::list<fowObj>::iterator it = fowObjList.begin(); it != fowObjList.end(); ++it)
		{
			vector2D::vec2D objWorldPos{ ecs.GetComponent<BaseInfo>((*it).getid())->position };
			// Obj did not move, no action required
			if ((*it).getWorldPos() == objWorldPos)
				continue;

			vector2D::vec2D currMapPos = worldToMap(objWorldPos);
			vector2D::vec2D prevMapPos = (*it).getMapPos();
			// Obj moved, but did not change grid, no action required
			if (prevMapPos == currMapPos)
				continue;

			// Obj moved and changed grid, update the following:
			std::list<fowTile>::iterator it2 = fowTileMap.begin();
			std::list<vector2D::vec2D> LOS = (*it).getLOS();
			// (a) remove obj from prev til
			std::advance(it2, (int)(prevMapPos.y) * row + (int)(prevMapPos.x));
			(it2)->removeFromTile((*it).getid(), prevMapPos);

			// (b) update states of visible tiles to fogged
			for (std::list<vector2D::vec2D>::iterator it3 = LOS.begin(); it3 != LOS.end(); ++it3)
			{
				it2 = fowTileMap.begin();
				std::advance(it2, (int)(*it3).y * row + (*it3).x);
				if (it2->getTileState() == fowTileState::fogged)
				{
					(ecs.GetComponent<Render>((*it2).getid()))->render = true;
					continue;
				}
				(it2)->updateTileStateToFog();
				(ecs.GetComponent<Render>((*it2).getid()))->render = true;
				(ecs.GetComponent<Texture>((*it2).getid()))->textureID = 1;
			}
		}

		// Change fogged/unvisited tiles to visible
		for (std::list<fowObj>::iterator it = fowObjList.begin(); it != fowObjList.end(); ++it)
		{
			vector2D::vec2D objWorldPos{ ecs.GetComponent<BaseInfo>((*it).getid())->position };
			// Obj did not move, no action required
			if ((*it).getWorldPos() == objWorldPos)
				continue;

			vector2D::vec2D currMapPos = worldToMap(objWorldPos);
			vector2D::vec2D prevMapPos = (*it).getMapPos();
			// Obj moved, but did not change grid, no action required
			if (prevMapPos == currMapPos)
				continue;

			// Obj moved and changed grid, update the following:
			std::list<fowTile>::iterator it2 = fowTileMap.begin();
			std::list<vector2D::vec2D> LOS = (*it).getLOS();
			// (a) world pos
			// (b) map pos
			(*it).fowObjUpdate(objWorldPos, currMapPos, col, row);
			// (c) add obj to curr tile and update states of the tile
			// (d) update frontier pos
			// (e) update LOS pos
			(it2)->addToTile((*it).getid(), (*it).getLOS(), currMapPos);
			LOS = (*it).getLOS();

			// (f) update states of fogged/univisited tiles to visible
			for (std::list<vector2D::vec2D>::iterator it3 = LOS.begin(); it3 != LOS.end(); ++it3)
			{
				it2 = fowTileMap.begin();
				std::advance(it2, (int)(*it3).y * row + (*it3).x);
				if (it2->getTileState() == fowTileState::visible)
				{
					(ecs.GetComponent<Render>((*it2).getid()))->render = false;
					continue;
				}
				(it2)->updateTileStateToVisible();
				(ecs.GetComponent<Render>((*it2).getid()))->render = false;
			}
		}

	}

	void fogOfWarMap::drawFow()
	{

	}

	void fogOfWarMap::destroyFow()
	{
		fowTileMap.clear();
		fowObjList.clear();
	}

	int fogOfWarMap::getWidth()
	{
		return width;
	}
	
	int fogOfWarMap::getHeight()
	{
		return height;
	}
	
	int fogOfWarMap::getCol()
	{
		return col;
	}
	
	int fogOfWarMap::getRow()
	{
		return row;
	}
	
	int fogOfWarMap::getDims()
	{
		return dims;
	}

	vector2D::vec2D fogOfWarMap::getWorldPos()
	{
		return worldPos;
	}

	std::list<fowTile>& fogOfWarMap::getFowTileMap()
	{
		return fowTileMap;
	}

	std::list<fowObj>& fogOfWarMap::getFowObjList()
	{
		return fowObjList;
	}
}

#endif

//#endif