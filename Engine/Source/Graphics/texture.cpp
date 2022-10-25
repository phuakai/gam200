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
	int width, height, channels;
	int width2, height2, channels2;
	stbi_set_flip_vertically_on_load(1);
	
	glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &textureobj.textureid);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glBindTexture(GL_TEXTURE_2D_ARRAY, (textureobj.textureid));
	glTextureParameteri(textureobj.textureid, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(textureobj.textureid, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(textureobj.textureid, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(textureobj.textureid, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	unsigned char* img = stbi_load(textureobj.paths[0], &width, &height, &channels, 0);
	if (img == NULL) {
		printf("Error in loading the image\n");
		exit(1);
	}
	unsigned char* img2 = stbi_load(textureobj.paths[1], &width2, &height2, &channels2, 0);
	if (img2 == NULL) {
		printf("Error in loading the image\n");
		exit(1);
	}
	// Target, level, internal format, width, height
	glTextureStorage3D(textureobj.textureid, 1, GL_RGBA8, width, height, 2);
	glTextureSubImage3D(textureobj.textureid, 0, 0, 0, 0, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, img);
	stbi_image_free(img);
	for (int numoftexs = 1; numoftexs < textureobj.paths.size(); numoftexs++)
	{
		// Target, level, xoffset, yoffset, width, height, format, type, data
		
		std::cout << "Hi it entered here " << std::endl;
		glTextureSubImage3D(textureobj.textureid, 0, 0, 0, numoftexs, width2, height2, 1, GL_RGBA, GL_UNSIGNED_BYTE, img2);
		
		stbi_image_free(img2);
		
	}
}

void Graphics::Texture::deleteTexture(Graphics::Texture& textureobj)
{
	//glDeleteTextures(1, &(textureobj.texture));
}

int Graphics::Texture::getTexid()
{
	return textureid;
}

void Graphics::Texture::setData(int handle, Graphics::Texture& textureobj)
{
	textureobj.textureid = handle;
}
