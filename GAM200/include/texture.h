#pragma once

#include <string>

namespace Graphics
{
	class Texture
	{
	public:
		static void loadTexture(std::string path, Graphics::Texture &textureobj);
		static void setData();
		int getTexid();
	private:
		unsigned int texture;
	};
}