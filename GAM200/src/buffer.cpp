#include <buffer.h>

GLuint Graphics::VAO::init()
{
	GLuint vao;
	glCreateVertexArrays(1, &vao); // Creates a vertex array object (can replace vao with an array if multiple buffers)
	return vao;
}

void Graphics::VAO::bind(GLuint vao)
{
	glBindVertexArray(vao);
}

void Graphics::VAO::unbind()
{
	glBindVertexArray(0);
}

void Graphics::VAO::enableattrib(GLuint vao, int attrib)
{
	glEnableVertexArrayAttrib(vao, attrib);// Enables the vertex array attrib for index 0 of vao
	// When enabled, vertex attribute array will be accessed and used for rendering 
	// when calls are made to vertex array commands such as glDrawArrays, glDrawElements, glDrawRangeElements, glMultiDrawElements, or glMultiDrawArrays.
}

void Graphics::VAO::bindattrib(GLuint vao, int attribindex, int bufferindex)
{
	glVertexArrayAttribBinding(vao, attribindex, bufferindex); // Associates a vertex attribute and a vertex buffer binding for a VAO
	// Name of vertex array object, index of vertex attrib, index of vertex buffer binding index
}

void Graphics::VAO::setattrib(GLuint vao, int index)
{
	glVertexArrayAttribFormat(vao, index, 2, GL_FLOAT, GL_FALSE, 0);// Specify the organisation of vertex arrays
	// Name of vertex array object, index of vertex array object being set, size (num of values per vertex that is stored in array), type of data,
	// GL_TRUE = data should be normalized, GL_FALSE = data converted directly as fixed-point values, offset (distance between elements of buffer)
}

void Graphics::VAO::del()
{

}

GLuint Graphics::VBO::init()
{
	GLuint vbo;
	glCreateBuffers(1, &vbo); // Creates a buffer named vbo (can replace vbo with an array if multiple buffers)
	return vbo;
}

void Graphics::VBO::bind(GLuint vao, int index, GLuint vbo, int offset, GLsizei step)
{
	glVertexArrayVertexBuffer(vao, index, vbo, offset, step); // Binds a buffer to a vertex array object
	// Name of vertex array object, index for vertex buffer object to bind to, name of buffer to be binded, offset of first element, stride/step (distance between elements of buffer)
}


void Graphics::VBO::unbind()
{
	//glBindVertexArray(0);
}

void Graphics::VBO::store(GLuint vbo, int size, std::vector <vector2D::vec2D> data)
{
	glNamedBufferStorage(vbo, size, data.data(), GL_DYNAMIC_STORAGE_BIT); // Creates a buffer object's storage
	// vbo is buffer name, followed by size of buffer (float type * number of data), data stored in buffer, and finally the flag of the storage system
	// GL_DYNAMIC_STORAGE_BIT allows contents of the data to be updated after creation by calling glBufferSubData, 
	// else you can only use server-side calls such as glCopyBufferSubData and glClearBufferSubData.
}

void Graphics::VBO::setdata(GLuint vbo, int offset, int size, std::vector <vector2D::vec2D> data)
{
	glNamedBufferSubData(vbo, 0, size, data.data()); // Set new buffer index with subdata
}

void Graphics::VBO::del()
{

}

GLuint Graphics::EBO::init()
{
	GLuint ebo;
	glCreateBuffers(1, &ebo); // Creates a buffer named ebo (can replace ebo with an array if multiple buffers)
	return ebo;
}

void Graphics::EBO::bind(GLuint vao, GLuint ebo)
{
	glVertexArrayElementBuffer(vao, ebo); // Configures element array buffer binding of a vertex array object
	// Name of vertex array object, name of buffer object used for element array buffer binding
}


void Graphics::EBO::unbind()
{
	//glBindVertexArray(0);
}

void Graphics::EBO::store(GLuint ebo, int size, std::vector <GLushort> data)
{
	glNamedBufferStorage(ebo, size, data.data(), GL_DYNAMIC_STORAGE_BIT); // Creates a buffer object's storage
	// vbo is buffer name, followed by size of buffer (float type * number of data), data stored in buffer, and finally the flag of the storage system
	// GL_DYNAMIC_STORAGE_BIT allows contents of the data to be updated after creation by calling glBufferSubData, 
	// else you can only use server-side calls such as glCopyBufferSubData and glClearBufferSubData.
}

void Graphics::EBO::setdata(GLuint ebo, int size, std::vector <GLushort> data)
{
	glNamedBufferSubData(ebo, 0, size, data.data()); // Set new buffer index with subdata
}

void Graphics::EBO::del()
{

}