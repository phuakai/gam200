#pragma once

#include <glhelper.h>
#include <vec2D.h>

namespace Graphics
{
	class VAO
	{
		public:
		
			static GLuint init(); // Create vao
			static void bind(GLuint vao); // Binds vao
			static void unbind(); // Unbinds vao
			static void enableattrib(GLuint vao); // Enable attrib
			static void bindattrib(GLuint vao); // Bind attrib
			static void setattrib(GLuint vao); // Set attrib
			static void del(); // Delete vao

	};

	class VBO
	{
	public:

		static GLuint init(); // Create buffer
		static void bind(GLuint vao, GLuint vbo, GLsizei step); // Binds buffer
		static void unbind(); // Unbinds buffer
		static void store(GLuint vbo, int size, std::vector <vector2D::vec2D> data);
		static void setdata(GLuint vbo, int size, std::vector <vector2D::vec2D> data);
		static void del(); // Delete buffer
	};

	class EBO
	{
	public:

		static GLuint init(); // Create buffer
		static void bind(GLuint vao, GLuint ebo); // Binds buffer
		static void unbind(); // Unbinds buffer
		static void store(GLuint ebo, int size, std::vector <GLushort> data);
		static void setdata(GLuint ebo, int size, std::vector <GLushort> data);
		static void del(); // Delete buffer
	};
}