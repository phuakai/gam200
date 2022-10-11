#include "physicsTransform.h"
#define _USE_MATH_DEFINES //for M_PI
#include <math.h>

physicsTransform::physicsTransform()
{
	positionX = 0.f;
	positionY = 0.f;
	sin = 0.f;
	cos = 0.f;
}

physicsTransform::physicsTransform(vector2D::vec2D pos, float angle)
{
	float angleInRad{angle * static_cast<float>(M_PI) / 180.f};
	positionX = pos.x;
	positionY = pos.y;
	sin = sinf(angleInRad);
	cos = cosf(angleInRad);
}

physicsTransform::physicsTransform(float x, float y, float angle)
{
	float angleInRad{ angle * static_cast<float>(M_PI) / 180.f };
	positionX = x;
	positionY = y;
	sin = sinf(angleInRad);
	cos = cosf(angleInRad);
}

vector2D::vec2D physicsTransform::phyTransform(vector2D::vec2D v, physicsTransform transform)
{
	return vector2D::vec2D(transform.cos * v.x - transform.sin * v.y + transform.positionX,
							transform.sin * v.x + transform.cos * v.y + transform.positionY);
}