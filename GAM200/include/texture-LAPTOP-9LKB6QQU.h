#pragma once

#include <string>
#include <vector>

namespace Graphics
{
	class Texture
	{
	public:
		Texture();
		static void loadTexture(const char* path, Graphics::Texture& textureobj);
		static void setData(int handle, Graphics::Texture& textureobj);
		int getTexid();
	private:
		unsigned int texture;
	};

	void createTextureVector(std::vector<Texture> &texobjs, int texturecount);

	static std::vector<Graphics::Texture> textureobjects; // Texture vector
	//void clearTextureVector();
}