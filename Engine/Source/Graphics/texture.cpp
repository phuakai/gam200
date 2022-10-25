/* !
@file    texture.cpp
@author  a.weiren@digipen.edu
@date    23/8/2022

This file loads, sets and deletes the textures in the game
*//*__________________________________________________________________________*/
#include <texture.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#include <GL/glew.h>
#include <glad/glad.h>
#include <fstream>


Graphics::Texture::Texture()
{
	target = GL_TEXTURE_2D_ARRAY;
	textureid = 0;
	//textures.clear();
}

void Graphics::Texture::createTexturePath(const char* path, Graphics::Texture& textureobj)
{
	textureobj.paths.emplace_back(path);
}

void Graphics::Texture::loadTexture(Graphics::Texture& textureobj)
{
	int width{};
	int height{};
	int channels{};
	std::vector<unsigned char*> images;
	stbi_set_flip_vertically_on_load(1);
	
	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &textureobj.textureid);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glBindTexture(GL_TEXTURE_2D_ARRAY, (textureobj.textureid));
	glTextureParameteri(textureobj.textureid, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(textureobj.textureid, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(textureobj.textureid, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(textureobj.textureid, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (textureobj.paths.size() == 0)
	{
		std::cout << "No image path found" << std::endl;
		return;
	}
	for (int numoftexs = 0; numoftexs < textureobj.paths.size(); numoftexs++)
	{
		images.emplace_back(stbi_load(textureobj.paths[numoftexs], &width, &height, &channels, 0));
		if (images[numoftexs] == NULL) 
		{
			std::cout << "Error in loading the image" << std::endl;
			exit(1);
		}
	}
	
	// Target, level, internal format, width, height
	glTextureStorage3D(textureobj.textureid, 1, GL_RGBA8, width, height, images.size());
	for (int numoftexs = 0; numoftexs < textureobj.paths.size(); numoftexs++)
	{
		// Target, level, xoffset, yoffset, width, height, format, type, data
		glTextureSubImage3D(textureobj.textureid, 0, 0, 0, numoftexs, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, images[numoftexs]);
		stbi_image_free(images[numoftexs]);
	}
}

void Graphics::Texture::deleteTexture(Graphics::Texture& textureobj)
{
	glDeleteTextures(1, &(textureobj.textureid));
}

int Graphics::Texture::getTexid()
{
	return textureid;
}

void Graphics::Texture::setData(int handle, Graphics::Texture& textureobj)
{
	textureobj.textureid = handle;
}
