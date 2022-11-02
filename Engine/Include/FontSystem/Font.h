#pragma once
#include <string>
#include <map>
#include <iostream>

#include <assert.h>
#include <glm/vec2.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "glslshader.h"
#include "input.h"
#include "camera.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Font
{
	static GLSLShader shader;

	struct Character {
		unsigned int TextureID; // ID Handle of the Glyph Texture
		glm::ivec2 Size;		// Size of Glyph
		glm::ivec2 Bearing;		// Offset from Baseline to Left/top of Glyph
		unsigned int Advance;	// Offset to Advance Next Glyph
	};


	void init();
	void RenderFont(GLSLShader& s, std::string text, float x, float y, float scale, glm::vec3 color);
}