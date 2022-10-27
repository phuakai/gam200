#pragma once

#include "vec2D.h"
#include <glslshader.h>

namespace FrameBufferNS
{
	struct fBvertexData {
		vector2D::vec2D posVtx;
		vector2D::vec2D txtVtx;
	};

	struct frameBuffer
	{
	public:
		void createFrameBuffer();

		void createFrameBufferTex();

		void createFrameBufferVAO();
		
		void drawFrameBuffer();

		void delFrameBuffer();

		void delFrameBufferTex();

		void delFrameBufferVAO();

		unsigned int framebuffer;
		unsigned int framebuffervaoid;
		unsigned int framebuffervboid;
		unsigned int texColorbuffer;
		GLSLShader frameshader{}; // Shader of all objects in instance
	};

	
}