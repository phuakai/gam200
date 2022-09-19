
#pragma once
//#if false
#include <list>
#include <vector>
#include "vec2D.h"

struct partitionObj
{
private:
public:
	int partitionId;
	vector2D::vec2D position;

	float maxAlongX;
	float minAlongX;
	float maxAlongY;
	float minAlongY;

	partitionObj();
	~partitionObj();
	void addCreatedObj(vector2D::vec2D pos, float halfWidth, float halfHeight, float angle);

	void addCreatedObj(vector2D::vec2D pos, float rad);

	void reorganizeData();

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