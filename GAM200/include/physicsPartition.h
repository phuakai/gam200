
#pragma once
//#if false
#include <list>
#include <vector>
#include "vec2D.h"

/******************************************************************************/
/*!

*/
/******************************************************************************/
struct AABB
{
	vector2D::vec2D	min;																// Min point of bounding box
	vector2D::vec2D	max;																// Max point of bounding box
};

struct partitionObj
{
private:
public:
	int partitionId;
	vector2D::vec2D position;

	AABB minMaxPoints;

	//float maxAlongX;
	//float minAlongX;
	//float maxAlongY;
	//float minAlongY;

	/******************************
	* Constructors
	******************************/
	/******************************************************************************/
	/*!
	\brief
		This function constructs the partiction object with a specified row and col
	*/
	/******************************************************************************/
	partitionObj();

	/******************************
	* Deconstructors
	******************************/
	/******************************************************************************/
	/*!
	\brief
		This function deconstructs the partiction object with a specified row and col
	*/
	/******************************************************************************/
	~partitionObj();

	/******************************
	* Creator
	******************************/
	/******************************************************************************/
	/*!
	\brief
		This function creates a body with the propertiels of a box
	*/
	/******************************************************************************/
	void addCreatedObj(vector2D::vec2D pos, float halfWidth, float halfHeight, float angle);
	void addCreatedObj(vector2D::vec2D pos, float rad);

	/******************************
	* Update
	******************************/
	void reorganizeData();

	/******************************
	* Destroyer
	******************************/
	void removeDeletedObj();
};

static std::list <std::pair<int, partitionObj>> partitionStorage;


struct partitionGrid
{
private:
	int gridColumn;
	int gridRow;
	int section;

	std::vector <std::pair<float, float>> gridXRange;
	std::vector <std::pair<float, float>> gridYRange;
public:
	partitionGrid();
	~partitionGrid();

	void setGridColumn(int col);
	void setGridRow(int row);

	int getGridColumn();
	int getGridRow();
	int getNoOfSections();

	void updateSections();
	void computeXCoords();
	void computeYCoords();
};

static partitionGrid phyPartitionGrid;
//static std::list <partition> partitionStorage;

//#endif