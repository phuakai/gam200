#include <glhelper.h>
#include <vec2D.h>

namespace Graphics
{
	class VAO
	{
		public:
		
			GLuint init(); // Create vao
			void bind(GLuint vao); // Binds vao
			void unbind(); // Unbinds vao
			void enableattrib(GLuint vao); // Enable attrib
			void bindattrib(GLuint vao); // Bind attrib
			void setattrib(GLuint vao); // Set attrib
			void del(); // Delete vao

	};

	class VBO
	{
	public:

		GLuint init(); // Create buffer
		void bind(GLuint vao, GLuint vbo, GLsizei step); // Binds buffer
		void unbind(); // Unbinds buffer
		void store(GLuint vbo, int size, std::vector <vector2D::vec2D> data);
		void setdata(GLuint vbo, int size, std::vector <vector2D::vec2D> data);
		void del(); // Delete buffer
	};

	class EBO
	{
	public:

		GLuint init(); // Create buffer
		void bind(GLuint vao, GLuint ebo); // Binds buffer
		void unbind(); // Unbinds buffer
		void store(GLuint ebo, int size, std::vector <vector2D::vec2D> data);
		void setdata(GLuint ebo, int size, std::vector <vector2D::vec2D> data);
		void del(); // Delete buffer
	};
}