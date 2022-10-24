#include "fogOfWar.h"
//#include "collision.h"

namespace fow
{
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

	fowTile::fowTile()
	{
		width = height = 0;
		worldPos = vector2D::vec2D(0.f, 0.f);
		gridPos = vector2D::vec2D(0.f, 0.f);
		state = fowTileState::unvisited;
	}
	
	fowTile::fowTile(int _width, int _height, vector2D::vec2D _worldPos, vector2D::vec2D _gridPos)
	{
		width = _width;
		height = _height;
		worldPos = _worldPos;
		gridPos = _gridPos;
		state = fowTileState::unvisited;
	}

	void fowTile::createFowTile(int _width, int _height, vector2D::vec2D _worldPos, vector2D::vec2D _gridPos)
	{
		width = _width;
		height = _height;
		worldPos = _worldPos;
		gridPos = _gridPos;
		state = fowTileState::unvisited;
	}

	void fowTile::removeFromTile(EntityID id, vector2D::vec2D oldMapPos)
	{
		//remove entity from tile
		entitiesOnTile.remove(id);
	}

	void fowTile::addToTile(EntityID id, std::list<vector2D::vec2D> LOS, vector2D::vec2D oldMapPos)
	{
		//add entity to tile
		entitiesOnTile.emplace_back(id);
		//update states of surrounding tile
		state = fowTileState::visible;
	}

	void fowTile::updateTileState()
	{
		state = fowTileState::visible;
	}

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
		int W{ static_cast<int>(currMapPos.x) - visionRad },
			E{ static_cast<int>(currMapPos.x) + visionRad }, 
			S{ static_cast<int>(currMapPos.y) - visionRad },
			N{ static_cast<int>(currMapPos.y) + visionRad };
		//std::cout << W << " " << E << " " << S << " " << N << std::endl;
		// edge handling
		if (mapPos.x < visionRad)
		{
			W = 0;
		}
		else if (mapPos.x >= col - visionRad)
		{
			E = col - 1;
		}
		if (mapPos.y < visionRad)
		{
			S = 0;
		}
		else if (mapPos.y >= row - visionRad)
		{
			N = row - 1;
		}

		//std::list <fowTile*>::const_iterator  it = tileMap.begin();
		// top row
		//if (W < E)
		//	std::cout << W << " " << E << "w is less than e\n";
		//else
		//	std::cout << W << " " << E << "w is more than e, THIS IUS WRONG\n";
		for (int c = W; c <= E; ++c)
		{
			//std::cout << "1";
			frontier.emplace_back(vector2D::vec2D(c, N));
		}
		// right column
		for (int r = N; r >= S; --r)
		{
			//std::cout << "2";
			frontier.emplace_back(vector2D::vec2D(E, r));
		}
		// bottom row
		for (int c = E; c >= W; --c)
		{
			//std::cout << "3";
			frontier.emplace_back(vector2D::vec2D(c, S));
		}
		// left column
		for (int r = S; r <= N; ++r)
		{
			//std::cout << "4";
			frontier.emplace_back(vector2D::vec2D(W, r));
		}

		//std::cout << "bye\n";
		for (int r = S + 1; r <= N - 1; ++r)
		{
			for (int c = W + 1; c <= E - 1; ++c)
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

