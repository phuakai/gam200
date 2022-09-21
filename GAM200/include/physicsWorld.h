#pragma once

class flatWorld
{
public:
	float minBodySize;// { 0.01f * 0.01f };
	float maxBodySize;// { 64.f * 64.f };

	float minDensity;// = 0.5f;
	float maxDensity;// = 21.4f;

	flatWorld();
	flatWorld(float minSize, float maxSize, float minDen, float maxDen);
};

static flatWorld world;