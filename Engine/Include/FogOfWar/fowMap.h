/* Start Header ************************************************************************/
/*!
\file		fowMap.h
\author		Grace Lee, lee.g, 390002621
\par		lee.g\@digipen.edu
\date		Oct 19, 2022
\brief		This file contains the function declarations for the implementation of a 
			fog of war manager

Copyright (C) 2022 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/

#include "fogOfWar.h"
#include "mat3x3.h"
#include <list>
#include <iterator>

namespace fow
{
	struct fogOfWarMap
	{
	public:
		// CONSTRUCTOR
		/******************************************************************************/
		/*!
			Constructor for the fog of war map
		*/
		/******************************************************************************/
										fogOfWarMap					();
		/******************************************************************************/
		/*!
			Constructor for the fog of war map
		*/
		/******************************************************************************/
										fogOfWarMap					(int const& _width, int const& _height, int const& _col, int const& _row, vector2D::vec2D _pos);
		// CREATOR
		/******************************************************************************/
		/*!
			Creator for the fog of war map
		*/
		/******************************************************************************/
		void							createFow();

		// CONVERTER
		/******************************************************************************/
		/*!
			This function converts the position from world space to fog of war map space
		*/
		/******************************************************************************/
		vector2D::vec2D					worldToMap					(vector2D::vec2D const& _worldPos);

		/******************************************************************************/
		/*!
			This function converts the position from fog of war map space to world space
		*/
		/******************************************************************************/
		vector2D::vec2D					mapToWorld					(vector2D::vec2D const& _mapPos);

		// DECONSTRUCTOR
		/******************************************************************************/
		/*!
			Default deconstructor for the fog of war map
		*/
		/******************************************************************************/
										~fogOfWarMap				();
		
		/******************************************************************************/
		/*!
			This function adds a fog of war object if an entity is on the tile
		*/
		/******************************************************************************/
		void							addObjToFow					(fowObj obj);

		/******************************************************************************/
		/*!
			This function updates the position of the fow objects and the states of
			the surrounding tiles
		*/
		/******************************************************************************/
		void 							updateFow					();

		/******************************************************************************/
		/*!
			This function clears the std::list used to manage the fog of war
		*/
		/******************************************************************************/
		void 							destroyFow					();

		// Getter
		/******************************************************************************/
		/*!
			Returns the width of the map
		*/
		/******************************************************************************/
		int  							getWidth					();

		/******************************************************************************/
		/*!
			Returns the height of the map
		*/
		/******************************************************************************/
		int  							getHeight					();

		/******************************************************************************/
		/*!
			Returns the column of the map
		*/
		/******************************************************************************/
		int  							getCol						();

		/******************************************************************************/
		/*!
			Returns the row of the map
		*/
		/******************************************************************************/
		int  							getRow						();

		/******************************************************************************/
		/*!
			Returns the dimensions of the map
		*/
		/******************************************************************************/
		int  							getDims						();

		/******************************************************************************/
		/*!
			Returns the positions of the map in world coordinates
		*/
		/******************************************************************************/
		vector2D::vec2D					getWorldPos					();

		/******************************************************************************/
		/*!
			Returns the list of tiles used for the map
		*/
		/******************************************************************************/
		std::list<fowTile>&				getFowTileMap				();

		/******************************************************************************/
		/*!
			Returns the list of fog of war objects
		*/
		/******************************************************************************/
		std::list<fowObj>&				getFowObjList				();

	private:
		int								width;			// x-axis
		int								height;			// y-axis
		int								col;
		int								row;
		int								dims;
		vector2D::vec2D					worldPos;
		std::list<fowTile>				fowTileMap;
		std::list<fowObj>				fowObjList;

	};// struct fogOfWarMap

}// namespace fow

extern fow::fogOfWarMap fowMap;

