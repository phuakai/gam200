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
	curtexid = 0;
	target = GL_TEXTURE_2D_ARRAY;
	textureid = 0;
	//textures.clear();
}

void Graphics::Texture::loadTexture(const char* path, Graphics::Texture& textureobj)
{
	int width, height, channels;
	stbi_set_flip_vertically_on_load(1);
	unsigned char* img = stbi_load(path, &width, &height, &channels, 0);
	if (img == NULL) {
		printf("Error in loading the image\n");
		exit(1);
	}
	//glGenTextures(1, &(textureobj.textureid));
	glCreateTextures(GL_TEXTURE_2D, 1, &textureobj.textureid);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glBindTexture(GL_TEXTURE_2D, (textureobj.textureid));
	//glTextureParameteri(textureobj.textureid, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTextureParameteri(textureobj.textureid, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTextureParameteri(textureobj.textureid, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTextureParameteri(textureobj.textureid, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	if (img)
	{
		// Target, level, internal format, width, height, border, format, type, data
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
		// Target, level, internal format, width, height
		//glTexStorage3D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 1);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
		// Target, level, xoffset, yoffset, width, height, format, type, data
		//glTexSubImage3D(GL_TEXTURE_2D, 0, 0, 0, 0, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, img);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, img);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		// 
		//glGenerateMipmap(GL_TEXTURE_2D);
	}


	////glGenTextures(1, &texid);
	////glBindTexture(textureobj.target, texid);
	//glCreateTextures(GL_TEXTURE_2D_ARRAY, 1, &textureobj.textureid);
	//// set the texture wrapping/filtering options (on the currently bound texture object)
	//
	//int totaltiles = 1;
	//// load and generate the texture
	//if (img)
	//{
	//	//glTexImage2D(textureobj.target, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	//	//glGenerateMipmap(textureobj.target);
	//	glTextureStorage3D(textureobj.textureid, 1, GL_RGB8, width, height, totaltiles);
	//	for (int tile = 0; tile < totaltiles; tile++)
	//	{
	//		
	//		glTextureSubImage3D(textureobj.textureid, 0, 0, 0, 0, width, height, 1, GL_RGBA, GL_UNSIGNED_BYTE, img);
	//		glTextureParameteri(textureobj.textureid, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//		glTextureParameteri(textureobj.textureid, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//		glTextureParameteri(textureobj.textureid, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//		glTextureParameteri(textureobj.textureid, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//		glBindTextureUnit(0, textureobj.textureid);
	//		std::cout << "Entered here " << textureobj.textureid << std::endl;
	//	}
	//}
	stbi_image_free(img);
	textureobj.curtexid++;


	//int width, height, channels;
	//stbi_set_flip_vertically_on_load(1);
	//unsigned char* img = stbi_load(path, &width, &height, &channels, 0);
	//if (img == NULL) {
	//	printf("Error in loading the image\n");
	//	exit(1);
	//}
	//glGenTextures(1, &(textureobj.texture));
	//glBindTexture(GL_TEXTURE_2D, (textureobj.texture));
	//// set the texture wrapping/filtering options (on the currently bound texture object)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//// load and generate the texture
	//if (img)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//stbi_image_free(img);
}

void Graphics::Texture::deleteTexture(Graphics::Texture& textureobj)
{
	//glDeleteTextures(1, &(textureobj.texture));
}

int Graphics::Texture::getTexid()
{
	std::cout << "texture id" << std::endl;
	//return texture;
}

void Graphics::Texture::setData(int handle, Graphics::Texture& textureobj)
{
	std::cout << "set data" << std::endl;
	//textureobj.texture = handle;
}

void Graphics::createTextureVector(Texture& texobjs, int texturecount)
{
	//Texture tmp;
	std::cout << "Texture count " << texturecount << std::endl;
	//for (int i = 0; i < texturecount; i++)
	//{
	//	texobjs.emplace_back(tmp);
	//}
}