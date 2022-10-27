#pragma once

#include "vec2D.h"

namespace FrameBufferNS
{
	struct fBvertexData {
		vector2D::vec2D posVtx;
		vector2D::vec2D txtVtx;
	};

	class frameBuffer
	{
	public:
		void createFrameBuffer();

		void createFrameBufferTex();

		void createFrameBufferVAO();
		
		void drawFrameBuffer();

		void delFrameBuffer();

		void delFrameBufferTex();

		unsigned int framebuffer;
		unsigned int framebuffervaoid;
		unsigned int framebuffervboid;
		unsigned int texColorbuffer;
	};

	
}