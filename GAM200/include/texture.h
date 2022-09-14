#pragma once

#include <string>

namespace Graphics
{
	class Texture
	{
	public:
		static void loadTexture(std::string path, Graphics::Texture &textureobj);
		static void setData(int handle, Graphics::Texture& textureobj);
		int getTexid();
	private:
		unsigned int texture;
	};
}