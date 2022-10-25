#include "fogOfWar.h"
#include "mat3x3.h"
#include <list>
#include <iterator>
#include "../mainHeader.h"
#include "ECS.h"

//#if false
namespace fow
{
	struct fogOfWarMap
	{
	public:
		fogOfWarMap();
		fogOfWarMap(int const& _width, int const& _height, int const& _col, int const& _row, vector2D::vec2D _pos);
		//void worldToMap(vector2D::vec2D worldPos, vector2D::vec2D mapPos);
		//void mapToWorld(vector2D::vec2D mapPos, vector2D::vec2D worldPos);
		vector2D::vec2D worldToMap(vector2D::vec2D const& worldPos);
		vector2D::vec2D mapToWorld(vector2D::vec2D const& mapPos);
		~fogOfWarMap();
		void createFow();
		void addObjToFow(fowObj obj);
		void updateFow();
		void drawFow();
		void destroyFow();

		int getWidth();
		int getHeight();
		int getCol();
		int getRow();
		int getDims();
		std::list<fowTile>& getFowTileMap();
		std::list<fowObj>& getFowObjList();

	private:
		int width; // x-axis
		int height; // y-axis
		int col;
		int row;
		int dims;
		vector2D::vec2D worldPos;
		std::list<fowTile> fowTileMap;
		std::list<fowObj> fowObjList;
	};

	static fogOfWarMap fowMap {1600, 900, 20, 20, vector2D::vec2D(0.f, 0.f)};
}

//#endif