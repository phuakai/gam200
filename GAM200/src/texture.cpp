#include <texture.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glew.h>
#include <fstream>


Graphics::Texture::Texture()
{
	texture = 0;
}

void Graphics::Texture::loadTexture(const char* path, Graphics::Texture& textureobj)
{
	int width, height, channels;
	unsigned char* img = stbi_load(path, &width, &height, &channels, 0);
	if (img == NULL) {
		printf("Error in loading the image\n");
		exit(1);
	}
	glGenTextures(1, &(textureobj.texture));
	glBindTexture(GL_TEXTURE_2D, (textureobj.texture));
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	if (img)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	stbi_image_free(img);
}

void Graphics::Texture::deleteTexture(Graphics::Texture& textureobj)
{
	glDeleteTextures(1, &(textureobj.texture));
}

int Graphics::Texture::getTexid()
{
	return texture;
}

void Graphics::Texture::setData(int handle, Graphics::Texture& textureobj)
{
	textureobj.texture = handle;
}

void Graphics::createTextureVector(std::vector<Texture>& texobjs, int texturecount)
{
	Texture tmp;
	std::cout << "Texture count " << texturecount << std::endl;
	for (int i = 0; i < texturecount; i++)
	{
		texobjs.emplace_back(tmp);
	}
}