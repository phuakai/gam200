#include "framebuffer.h"
#include "input.h" // For screen dimensions
#include <glad/glad.h>
#include <iostream> // Just to cout

void FrameBufferNS::frameBuffer::createFrameBuffer()
{
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

void FrameBufferNS::frameBuffer::drawFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Bind to default frame buffer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(0, texColorbuffer);
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