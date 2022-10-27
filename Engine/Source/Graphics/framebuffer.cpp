#include "framebuffer.h"
#include "buffer.h"
#include "input.h" // For screen dimensions
#include <glad/glad.h>
#include <iostream> // Just to cout


void FrameBufferNS::frameBuffer::createFrameBuffer()
{
	createFrameBufferVAO();
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	createFrameBufferTex();
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Frame buffer can't be created" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBufferNS::frameBuffer::createFrameBufferTex()
{
	glGenTextures(1, &texColorbuffer);
	glBindTexture(GL_TEXTURE_2D, texColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Graphics::Input::screenwidth, Graphics::Input::screenheight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorbuffer, 0);
}

void FrameBufferNS::frameBuffer::createFrameBufferVAO()
{
	std::vector<vector2D::Vec2> tex_coord
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
		fBvertexData.emplace_back(tmpVtxData);
	}

	framebuffervboid = Graphics::VBO::init();
	Graphics::VBO::store(framebuffervboid, (int)sizeof(FrameBufferNS::fBvertexData) * (int)fBvertexData.size(), fBvertexData);

	framebuffervaoid = Graphics::VAO::init();
	Graphics::VAO::enableattrib(framebuffervaoid, 0); // Attrib 0
	Graphics::VBO::bind(framebuffervaoid, 0, framebuffervboid, 0, sizeof(float) * 4); // Set buffer binding point 0
	Graphics::VAO::setattrib(framebuffervaoid, 0, 2); // Attrib format
	Graphics::VAO::bindattrib(framebuffervaoid, 0, 0); // Bind attrib

	Graphics::VAO::enableattrib(framebuffervaoid, 1); // Attrib 0
	Graphics::VBO::bind(framebuffervaoid, 1, framebuffervboid, sizeof(float) * 2, sizeof(float) * 4); // Set buffer binding point 0
	Graphics::VAO::setattrib(framebuffervaoid, 1, 2); // Attrib format
	Graphics::VAO::bindattrib(framebuffervaoid, 1, 1); // Bind attrib
}

void FrameBufferNS::frameBuffer::drawFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind to default frame buffer
	glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTextureUnit(0, texColorbuffer);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameBufferNS::frameBuffer::delFrameBuffer()
{
	glDeleteFramebuffers(1, &framebuffer);
	delFrameBufferTex();
}
void FrameBufferNS::frameBuffer::delFrameBufferTex()
{
	glDeleteTextures(1, &(texColorbuffer)); // Delete generated texture
}