#pragma once

#include <vec2D.h>
#include <mat3x3.h>

class Transform // Input class
{
public:

	matrix3x3::mat3x3 getNDCtoViewport();

	matrix3x3::mat3x3 getViewporttoNDC();

	matrix3x3::mat3x3 getModeltoNDC(); // temp

	matrix3x3::mat3x3 getNDCtoModel(); // temp

	matrix3x3::mat3x3 getModeltoWorld(); // temp

	matrix3x3::mat3x3 getWorldtoModel(); // temp

	matrix3x3::mat3x3 getWorldtoNDC();

	matrix3x3::mat3x3 getNDCtoWorld();


private:
	//// Model to NDC
	//matrix3x3::mat3x3 model_to_ndc_xform{};
	//// NDC to Model
	//matrix3x3::mat3x3 ndc_to_model_xform{};
	//// Model to World
	//matrix3x3::mat3x3 model_to_world_xform{};
	//// World to Model
	//matrix3x3::mat3x3 world_to_model_xform{};
	//// World to NDC
	//matrix3x3::mat3x3 world_to_ndc_xform{};
	//// NDC to World
	//matrix3x3::mat3x3 ndc_to_world_xform{};

	//// Temp
	//matrix3x3::mat3x3 viewport_to_ndc_xform{};
	//// Temp
	//matrix3x3::mat3x3 ndc_to_viewport_xform{};
};

vector2D::vec2D convertNDCtoWorld(vector2D::vec2D ndc);

static Transform transform;