#pragma once

#ifndef FOGOFWAR_H
#define FOGOFWAR_H

//#include "fowMap.h"
#include "vec2D.h"
#include <list>
#include <iterator>
#include "../mainHeader.h"
#include "ECS.h"

namespace fow
{
	#define visionRad 2
	#define tileStateOffset 16

	enum class direction : int
	{
		//  0, 1,  2, 3,  4, 5,  6, 7
		n, nw, w, sw, s, se, e, ne

	};
	//prefix
	direction& operator ++ (const direction& rhs);
	//postfix
	direction operator++ (direction& x, int);
	//direction& operator +=(direction in);

	enum class fowTileState : int
	{
		//north to north east, in counter-clockwise direction
		//Basic states
		unvisited = 0,
		visible = 1,
		fogged = 2,

		// Cloud
		// 3 - 6
		unvisitedN, unvisitedW, unvisitedS, unvisitedE,

		// 7 - 10
		unvisitedNE, unvisitedSW, unvisitedNW, unvisitedSE,

		// 11 - 14
		unvisitedDeadEndN, unvisitedDeadEndW, unvisitedDeadEndS, unvisitedDeadEndE,

		// 15 - 18
		unvisitedDeadEndNW, unvisitedDeadEndNSW, unvisitedDeadEndSE, unvisitedDeadEndNE,

		// Fog (darkened layer)
		// 19 - 22
		foggedN, foggedW, foggedS, foggedE,

		// 23 - 26
		foggedNW, foggedSW, foggedSE, foggedNE,

		// 27 - 30
		foggedDeadEndN, foggedDeadEndW, foggedDeadEndS, foggedDeadEndE,

		// 31 - 34
		foggedDeadEndNW, foggedDeadEndNSW, foggedDeadEndSE, foggedDeadEndNE,

		// Line Of Sight
		// 35 - 38
		losN, losS, losE, losW,

		// 39 - 42
		losNW, losSW, losSE, losNE,

		// 43 - 47
		losDeadEndN, losDeadEndW, losDeadEndS, losDeadEndE,

		// 48 - 51
		losDeadEndNE, losDeadEndNW, losDeadEndSE, losDeadEndNSW
	};

	struct fowTile
	{
	public:
		fowTile();
		fowTile(int _width, int _height, vector2D::vec2D _worldPos, vector2D::vec2D _gridPos);
		void createFowTile(int _width, int _height, vector2D::vec2D _worldPos, vector2D::vec2D _gridPos);
		void removeFromTile(EntityID id, vector2D::vec2D oldMapPos);
		void addToTile(EntityID id, std::list<vector2D::vec2D> LOS, vector2D::vec2D oldMapPos);
		void updateTileStateToFog();
		void updateTileStateToVisible();
		EntityID getid();
		int getWdith();
		int getHeight();
		vector2D::vec2D getWorldPos();
		fowTileState getTileState();
		void setid(EntityID _id);
	private:
		EntityID id;
		int width;
		int height;
		vector2D::vec2D worldPos;
		vector2D::vec2D gridPos;
		fowTileState state;
		std::list<EntityID> entitiesOnTile;
	};

	struct fowObj
	{
	public:
		fowObj();
		fowObj(EntityID _id, vector2D::vec2D _worldPos, vector2D::vec2D _gridPos, int col, int row);
		void fowObjUpdate(vector2D::vec2D const& newWorldPos, vector2D::vec2D const& newMapPos, int col, int row);
		void updateTilePos(vector2D::vec2D const& currMapPos, int col, int row);

		EntityID getid();
		vector2D::vec2D getWorldPos();
		vector2D::vec2D getMapPos();
		std::list<vector2D::vec2D> getFrontier();
		std::list<vector2D::vec2D> getLOS();

	private:
		EntityID id;
		vector2D::vec2D worldPos;
		vector2D::vec2D mapPos;
		std::list<vector2D::vec2D> frontier;			// grid space
		std::list<vector2D::vec2D> LOS;					// grid space
	};

	#if false
	#endif

}

#endif