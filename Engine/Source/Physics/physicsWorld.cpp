#include <physicsWorld.h>

flatWorld::flatWorld() 
{
	minBodySize = 0.01f * 0.01f;
	maxBodySize = 1000.f * 1000.f;
	minDensity = 0.5f;
	maxDensity = 21.4f;
}


flatWorld::flatWorld(float minSize, float maxSize, float minDen, float maxDen)
{
	minBodySize = minSize;
	maxBodySize = maxSize;
	minDensity = minDen;
	maxDensity = maxDen;
}
