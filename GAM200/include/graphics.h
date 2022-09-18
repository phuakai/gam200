#pragma once

#include <GL/glew.h> // for access to OpenGL API declarations 
#include <glm/glm.hpp>
#include <glapp.h>
#include <vec2D.h>
#include <vec3D.h>
#include <mat3x3.h>
#include <model.h>

namespace Graphics {

	class BatchRenderer
	{
	public:

		Model batchmodel{}; // Model of all objects in batch
		GLSLShader batchshader{}; // Shader of all objects in batch
		GLenum primtype{}; // Primitive type for all objects in batch
		int totaldrawcnt{}; // Total count of all vertices to be drawn
		int totalsize{}; // To add via subdata
		std::vector<vertexData> batchdata{}; // All vertices in batch

		int vboid{};
		int vaoid{};
		
		BatchRenderer();
		void BatchRender(); // batch renders all objects in render

	};

	struct GLViewport 
	{
		GLint x, y;
		GLsizei width, height;
	};

	class Camera2D 
	{
		public:
			void init(GLFWwindow*, GLApp::GLObject* ptr);
			void update(GLFWwindow*);

			// Temp
			matrix3x3::mat3x3 getViewxForm();
			// Temp
			matrix3x3::mat3x3 getCamwintoNDCForm();
			// Get world to NDC transformation matrix
			matrix3x3::mat3x3 getWorldtoNDCxForm();

			// Get camera object
			GLApp::GLObject getCameraObject();
			// Get height
			GLint getHeight();

			// Get Window Height
			int getWinHeight();
			// Get Window Width
			int getWinWidth();
		private:
			GLApp::GLObject* pgo{}; // pointer to game object that embeds camera
			vector2D::Vec2 right{};
			vector2D::Vec2 up{};

			matrix3x3::mat3x3 view_xform{};
			matrix3x3::mat3x3 camwin_to_ndc_xform{};
			matrix3x3::mat3x3 world_to_ndc_xform{};

			GLint height{ 1000 };
			GLfloat ar{};

			// window change parameters ...
			GLint min_height{ 500 }, max_height{ 2000 };
			// height is increasing if 1 and decreasing if -1
			GLint height_chg_dir{ 1 };
			// increments by which window height is changed per Z key press
			GLint height_chg_val{ 5 };
			// camera's speed when button U is pressed
			GLfloat linear_speed{ 2.f };
			// keyboard button press flags
			GLboolean camtype_flag{ GL_FALSE }; // button V
			GLboolean zoom_flag{ GL_FALSE }; // button Z
			GLboolean left_turn_flag{ GL_FALSE }; // button H
			GLboolean right_turn_flag{ GL_FALSE }; // button K
			GLboolean move_flag{ GL_FALSE }; // button U

		// you can implement these functions as you wish ...
		
	};
	// define object of type Camera2D ...
	static Camera2D camera2d;

	static std::vector<GLViewport> vps; // container for viewports

	

}