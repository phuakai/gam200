
//#if false


#include "physicsPartition.h"
#include "GLHelper.h"
#include <iostream>

partitionObj::partitionObj()
{
	//partitionId = 0;
	position.x = 0.f;
	position.y = 0.f;

	boudingBox.min.x = FLT_MAX;
	boudingBox.min.y = FLT_MAX;
	boudingBox.max.x = FLT_MIN;
	boudingBox.max.x = FLT_MIN;
}

partitionObj::partitionObj(int id, vector2D::vec2D pos, float width, float height, float angle)
{
	//partitionId = id;
	position.x = pos.x;
	position.y = pos.y;

	boudingBox.min.x = FLT_MAX;
	boudingBox.min.y = FLT_MAX;
	boudingBox.max.x = FLT_MIN;
	boudingBox.max.x = FLT_MIN;
}

partitionObj::partitionObj(int id, vector2D::vec2D pos, float rad)
{
	//partitionId = id;
	position.x = pos.x;
	position.y = pos.y;

	boudingBox.min.x = pos.x - rad;
	boudingBox.min.y = pos.y - rad;
	boudingBox.max.x = pos.x + rad;
	boudingBox.max.x = pos.y + rad;
}

partitionObj::~partitionObj()
{

}

void partitionObj::addCreatedObj(vector2D::vec2D pos, float halfWidth, float halfHeight, float angle)
{
	//++partitionId;
	position = pos;

	// Box is aligned with x and y axis
	if (((int)angle % 90) == 0 && (angle - (int)angle) == 0)
	{

		boudingBox.max.x = position.x + halfWidth;
		boudingBox.min.x = position.x +-halfWidth;
		boudingBox.max.y = position.y + halfHeight;
		boudingBox.min.y = position.y - halfHeight;
	}
	// Box is NOT aligned with x and y axis
	else
	{

	}
}

void partitionObj::addCreatedObj(vector2D::vec2D pos, float rad)
{
	//++partitionId;
}

void partitionObj::reorganizeData()
{

}

void partitionObj::removeDeletedObj()
{
	//--partitionId;
}



partitionGrid::partitionGrid()
{
	gridColumn = 4; // assuming win width is 1600, each width is 400
	gridRow = 3; // assuming win height is 900, each height is 300
	section = gridColumn * gridRow;
}

partitionGrid::~partitionGrid()
{

}

void partitionGrid::setGridColumn(int col)
{
	if (col <= 0)
	{
		std::cout << "Grid column should be more than 0!\n";
		return;
	}

	gridColumn = col;
	computeYCoords();
	updateSections();
}

void partitionGrid::setGridRow(int row)
{
	if (row <= 0)
	{
		std::cout << "Grid row should be more than 0!\n";
		return;
	}

	gridRow = row;
	computeXCoords();
	updateSections();
}

int partitionGrid::getGridColumn()
{
	return gridColumn;
}

int partitionGrid::getGridRow()
{
	return gridRow;
}

int partitionGrid::getNoOfSections()
{
	return gridRow * gridColumn;
}

void partitionGrid::updateSections()
{
	section = gridColumn * gridRow;
}

void partitionGrid::computeXCoords()
{
	if (gridColumn <= 0)
	{
		std::cout << "Grid has less than 1 column!\n";
		return;
	}
	float gridWidth{ static_cast<float>(GLHelper::width / gridColumn) };
	gridXRange.clear();
	for (int i{ 1 }; i <= gridColumn; ++i)
	{
		gridXRange[i].first = i * gridWidth;				// Lower limit
		gridXRange[i].second = (i + 1) * gridWidth;			// Upper limit
	}
}

void partitionGrid::computeYCoords()
{
	if (gridRow == 0)
	{
		std::cout << "Grid has less than 1 row!\n";
		return;
	}
	float gridHeight{ static_cast<float>(GLHelper::height / gridRow) };
	gridYRange.clear();
	for (int i{ 1 }; i <= gridRow; ++i)
	{
		gridYRange[i].first = i * gridHeight;				// Lower limit
		gridYRange[i].second = (i + 1) * gridHeight;		// Upper limit
	}
}

//#endif