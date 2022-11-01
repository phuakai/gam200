/* !
@file    buffer.cpp
@author  a.weiren@digipen.edu
@date    23/8/2022

This file handles the VAO, VBO and EBO of the game
*//*__________________________________________________________________________*/

#include <buffer.h>
namespace BufferNS
{
	GLuint VAO::createVAO()
	{
		GLuint vao;
		glCreateVertexArrays(1, &vao); // Creates a vertex array object (can replace vao with an array if multiple buffers)
		return vao;
	}

	void VAO::bindVAO(GLuint vao)
	{
		glBindVertexArray(vao);
	}

	void VAO::unbindVAO()
	{
		glBindVertexArray(0);
	}

	void VAO::enableVAOattrib(GLuint vao, int attrib)
	{
		glEnableVertexArrayAttrib(vao, attrib);// Enables the vertex array attrib for index 0 of vao
		// When enabled, vertex attribute array will be accessed and used for rendering 
		// when calls are made to vertex array commands such as glDrawArrays, glDrawElements, glDrawRangeElements, glMultiDrawElements, or glMultiDrawArrays.
	}

	void VAO::bindVAOattrib(GLuint vao, int attribindex, int bufferindex)
	{
		glVertexArrayAttribBinding(vao, attribindex, bufferindex); // Associates a vertex attribute and a vertex buffer binding for a VAO
		// Name of vertex array object, index of vertex attrib, index of vertex buffer binding index
	}

	void VAO::setVAOattrib(GLuint vao, int index, int unitsize, int offset)
	{
		glVertexArrayAttribFormat(vao, index, unitsize, GL_FLOAT, GL_FALSE, offset);// Specify the organisation of vertex arrays
		// Name of vertex array object, index of vertex array object being set, size (num of values per vertex that is stored in array), type of data,
		// GL_TRUE = data should be normalized, GL_FALSE = data converted directly as fixed-point values, offset (distance between elements of buffer)
	}

	void VAO::delVAO(GLuint vao)
	{
		glDeleteVertexArrays(1, &vao);
	}

	GLuint VBO::createVBO()
	{
		GLuint vbo;
		glCreateBuffers(1, &vbo); // Creates a buffer named vbo (can replace vbo with an array if multiple buffers)
		return vbo;
	}

	void VBO::bindVBO(GLuint vao, int index, GLuint vbo, int offset, GLsizei step)
	{
		glVertexArrayVertexBuffer(vao, index, vbo, offset, step); // Binds a buffer to a vertex array object
		// Name of vertex array object, index for vertex buffer object to bind to, name of buffer to be binded, offset of first element, stride/step (distance between elements of buffer)
	}


	void VBO::unbindVBO()
	{
		//glBindVertexArray(0);
	}

	void VBO::createVBOstorage(GLuint vbo, int size, std::vector <ModelNS::modelVtxData> data)
	{
		glNamedBufferStorage(vbo, size, data.data(), GL_DYNAMIC_STORAGE_BIT); // Creates a buffer object's storage
		// vbo is buffer name, followed by size of buffer (float type * number of data), data stored in buffer, and finally the flag of the storage system
		// GL_DYNAMIC_STORAGE_BIT allows contents of the data to be updated after creation by calling glBufferSubData, 
		// else you can only use server-side calls such as glCopyBufferSubData and glClearBufferSubData.
	}

	void VBO::createVBOstorage(GLuint vbo, int size, std::vector <FrameBufferNS::fBvertexData> data)
	{
		glNamedBufferStorage(vbo, size, data.data(), GL_DYNAMIC_STORAGE_BIT); // Creates a buffer object's storage
		// vbo is buffer name, followed by size of buffer (float type * number of data), data stored in buffer, and finally the flag of the storage system
		// GL_DYNAMIC_STORAGE_BIT allows contents of the data to be updated after creation by calling glBufferSubData, 
		// else you can only use server-side calls such as glCopyBufferSubData and glClearBufferSubData.
	}

	void VBO::createVBOstorage(GLuint vbo, int size, std::vector <matrix4x4::mat4x4> data)
	{
		glNamedBufferStorage(vbo, size, data.data(), GL_DYNAMIC_STORAGE_BIT); // Creates a buffer object's storage
		// vbo is buffer name, followed by size of buffer (float type * number of data), data stored in buffer, and finally the flag of the storage system
		// GL_DYNAMIC_STORAGE_BIT allows contents of the data to be updated after creation by calling glBufferSubData, 
		// else you can only use server-side calls such as glCopyBufferSubData and glClearBufferSubData.
	}

	void VBO::editVBOstorage(GLuint vbo, int offset, int size, std::vector <vector2D::vec2D> data)
	{
		glNamedBufferSubData(vbo, offset, size, data.data()); // Set new buffer index with subdata
	}

	void VBO::delVBO(GLuint vbo)
	{
		glDeleteBuffers(1, &vbo);
	}

	GLuint EBO::createEBO()
	{
		GLuint ebo;
		glCreateBuffers(1, &ebo); // Creates a buffer named ebo (can replace ebo with an array if multiple buffers)
		return ebo;
	}

	void EBO::bindEBO(GLuint vao, GLuint ebo)
	{
		glVertexArrayElementBuffer(vao, ebo); // Configures element array buffer binding of a vertex array object
		// Name of vertex array object, name of buffer object used for element array buffer binding
	}


	void EBO::unbindEBO()
	{
		//glBindVertexArray(0);
	}

	void EBO::createEBOstorage(GLuint ebo, int size, std::vector <GLushort> data)
	{
		glNamedBufferStorage(ebo, size, data.data(), GL_DYNAMIC_STORAGE_BIT); // Creates a buffer object's storage
		// vbo is buffer name, followed by size of buffer (float type * number of data), data stored in buffer, and finally the flag of the storage system
		// GL_DYNAMIC_STORAGE_BIT allows contents of the data to be updated after creation by calling glBufferSubData, 
		// else you can only use server-side calls such as glCopyBufferSubData and glClearBufferSubData.
	}

	void EBO::editEBOstorage(GLuint ebo, int size, std::vector <GLushort> data)
	{
		glNamedBufferSubData(ebo, 0, size, data.data()); // Set new buffer index with subdata
	}

	void EBO::delEBO(GLuint ebo)
	{
		glDeleteBuffers(1, &ebo);
	}
}