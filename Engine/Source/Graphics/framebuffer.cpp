#include "framebuffer.h"
#include "buffer.h"
#include "input.h" // For screen dimensions
#include <glad/glad.h>
#include <iostream> // Just to cout

namespace FrameBufferNS
{
	void frameBuffer::createFrameBuffer()
	{
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		createFrameBufferVAO(); // Create VAO for frame buffer (quad that covers screen)
		createFrameBufferTex(); // Create frame buffer texture for draw calls to render to
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) // Make sure frame buffer is created
		{
			std::cout << "Frame buffer can't be created" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind frame buffer
	}

	void frameBuffer::createFrameBufferTex() // Creates texture for draw calls to render to in framebuffer
	{
		glGenTextures(1, &texColorbuffer); // Generates a buffer
		glBindTexture(GL_TEXTURE_2D, texColorbuffer); // Binds the buffer to texture2d
		// Below creates space for texture based on size of screen
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Graphics::Input::screenwidth, Graphics::Input::screenheight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorbuffer, 0); // Attach texture to frame buffer
	}

	void frameBuffer::createFrameBufferVAO() // Create VAO for frame buffer (quad that covers screen)
	{
		std::vector<vector2D::Vec2> tex_coord // 6 indices since using DrawArray (4 if using element but requires EBO)
		{
			vector2D::Vec2(0.f, 1.f), vector2D::Vec2(0.f, 0.f),
			vector2D::Vec2(1.f, 0.f), vector2D::Vec2(0.f, 1.f),
			vector2D::Vec2(1.f, 0.f), vector2D::Vec2(1.f, 1.f)
		};
		std::vector<vector2D::Vec2> pos_coord
		{
			vector2D::Vec2(-1.f, 1.f), vector2D::Vec2(-1.f, -1.f),
			vector2D::Vec2(1.f, -1.f), vector2D::Vec2(-1.f, 1.f),
			vector2D::Vec2(1.f, -1.f), vector2D::Vec2(1.f, 1.f)
		};

		std::vector<FrameBufferNS::fBvertexData> fBvertexData;

		for (int tmp = 0; tmp < 6; tmp++)
		{
			FrameBufferNS::fBvertexData tmpVtxData;
			tmpVtxData.posVtx = pos_coord[tmp];
			tmpVtxData.txtVtx = tex_coord[tmp];
			fBvertexData.emplace_back(tmpVtxData); // Emplaces the 6 vec2's into a storage container
		}

		framebuffervboid = BufferNS::VBO::createVBO();
		BufferNS::VBO::createVBOstorage(framebuffervboid, (int)sizeof(FrameBufferNS::fBvertexData) * (int)fBvertexData.size(), fBvertexData); // Pass storage container into VBO

		framebuffervaoid = BufferNS::VAO::createVAO();
		BufferNS::VAO::enableVAOattrib(framebuffervaoid, 0); // Attrib 0
		BufferNS::VBO::bindVBO(framebuffervaoid, 0, framebuffervboid, 0, sizeof(float) * 4); // Set buffer binding point 0
		BufferNS::VAO::setVAOattrib(framebuffervaoid, 0, 2); // Attrib format
		BufferNS::VAO::bindVAOattrib(framebuffervaoid, 0, 0); // Bind attrib

		BufferNS::VAO::enableVAOattrib(framebuffervaoid, 1); // Attrib 1
		BufferNS::VBO::bindVBO(framebuffervaoid, 1, framebuffervboid, sizeof(float) * 2, sizeof(float) * 4); // Set buffer binding point 1
		BufferNS::VAO::setVAOattrib(framebuffervaoid, 1, 2); // Attrib format
		BufferNS::VAO::bindVAOattrib(framebuffervaoid, 1, 1); // Bind attrib
	}

	void frameBuffer::drawFrameBuffer()
	{
		frameshader.Use(); // Use frame shader
		BufferNS::VAO::bindVAO(framebuffervaoid);

		glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind to default frame buffer
		glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glBindTexture(GL_TEXTURE_2D, texColorbuffer); // Automatically binds texture to sampler2D
		glDrawArrays(GL_TRIANGLES, 0, 6);

		BufferNS::VAO::unbindVAO();
		frameshader.UnUse();

	}

	void frameBuffer::delFrameBuffer()
	{
		glDeleteFramebuffers(1, &framebuffer);
		delFrameBufferTex();
		delFrameBufferVAO();
	}
	void frameBuffer::delFrameBufferTex()
	{
		glDeleteTextures(1, &(texColorbuffer)); // Delete generated texture
	}

	void frameBuffer::delFrameBufferVAO()
	{
		glDeleteVertexArrays(1, &framebuffervaoid);
		glDeleteBuffers(1, &framebuffervboid);
	}
}