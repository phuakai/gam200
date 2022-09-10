#include <glhelper.h>
#include <vector>
#include <vec2D.h>

namespace Graphics
{
	class Model
	{
	public:
		// you could add member functions for convenience if you so wish ...
		Model init(); // read mesh data from file ...
		//void release(); // return buffers back to GPU ...

	private:
		GLenum primitive_type = GL_TRIANGLES; // which OpenGL primitive to be rendered?
		GLuint primitive_cnt = 0; // added for tutorial 2
		GLuint posvtx_cnt = 0;

		std::vector <vector2D::vec2D> pos_vtx;
		std::vector <vector2D::vec2D> model_coords;
		std::vector <vector2D::vec2D> ebo_indices;

		GLuint vaoid = 0; // handle to VAO
		GLuint vboid = 0;
		GLuint eboid = 0;
		GLuint draw_cnt = 0; 

		
	};
}