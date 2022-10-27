#pragma once

namespace FrameBufferNS
{
	class frameBuffer
	{
	public:
		void createFrameBuffer();

		void createFrameBufferTex();

		void drawFrameBuffer();

		void delFrameBuffer();

		void delFrameBufferTex();

		unsigned int framebuffer;
		unsigned int texColorbuffer;
	};

	
}