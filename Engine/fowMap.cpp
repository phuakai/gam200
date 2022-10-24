//#if false

#pragma once

#ifndef FOWMAP_H
#define FOWMAP_H

#include "fowMap.h"

namespace fow
{
	fogOfWarMap::fogOfWarMap()
	{
		width = 0;
		height = 0;
		col = 0;
		row = 0;
		dims = 0;
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
		vector2D::vec2D currMapPos;
		
		for (int i = 0; i < dims ; ++i)
		{
			currPos = ((i + 1) * tileWidth % width == 0) ? vector2D::vec2D(startingPos.x, currPos.y + tileHeight) :	vector2D::vec2D(currPos.x + tileWidth, currPos.y);
			currMapPos = worldToMap(currPos);
			fowTile tmp{ tileWidth, tileHeight, currPos, currMapPos };
			fowTileMap.emplace_back(tmp);
		}
	}

	void fogOfWarMap::addObjToFow(fowObj obj)
	{
		fowObjList.emplace_back(&obj);
	}

	void fogOfWarMap::updateFow()
	{
		for (std::list<fowObj>::iterator it = fowObjList.begin() ; it != fowObjList.end() ; ++it)
		{
			vector2D::vec2D objWorldPos{ ecs.GetComponent<Render>((*it).getid())->position };
			// Obj did not move, no action required
			if ((*it).getWorldPos() == objWorldPos)
				continue;

			// Obj moved, but did not change grid, no action required
			//vector2D::vec2D objMapPos{ ecs.GetComponent<Render>((**it).getid())->position };
			vector2D::vec2D currMapPos = worldToMap(objWorldPos);
			vector2D::vec2D prevMapPos = (*it).getMapPos();
			if (prevMapPos == prevMapPos)
				continue;

			// Obj moved and changed grid, update the following:
			// (a) remove obj from prev til and update states of the tile
			std::list<fowTile>::iterator it2 = fowTileMap.begin();
			std::advance(it2, (int)(prevMapPos.y) * row + (int)(prevMapPos.y));
			(it2)->removeFromTile((*it).getid(), prevMapPos);
			// (a) update frontier pos
			// (b) update LOS pos
			// (c) world pos
			// (d) map pos
			(*it).fowObjUpdate(objWorldPos, currMapPos, col, row);
			// (f) add obj to curr tile and update states of the tile
			(it2)->addToTile((*it).getid(), (*it).getLOS(), currMapPos);
			std::list<vector2D::vec2D> LOS = (*it).getLOS();
			for (std::list<vector2D::vec2D>::iterator it3 = LOS.begin() ; it3 != LOS.end(); ++it3)
			{
				it2 = fowTileMap.begin();
				std::advance(it2, (int)(*it3).y * row + (*it3).x);
				(it2)->updateTileState();
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

	std::list<fowTile> fogOfWarMap::getFowTileMap()
	{
		return fowTileMap;
	}

	std::list<fowObj> fogOfWarMap::getFowObjList()
	{
		return fowObjList;
	}

}

#endif
//#endif