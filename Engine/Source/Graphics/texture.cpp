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

namespace TextureNS
{
	Texture::Texture()
	{
		target = GL_TEXTURE_2D_ARRAY;
		textureid = 0;
		//textures.clear();
	}

	void Texture::createTexturePath(std::string path, Texture& textureobj)
	{
		textureobj.paths.emplace_back(path);
	}

	void Texture::loadTexture(Texture& textureobj)
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
			images.emplace_back(stbi_load(textureobj.paths[numoftexs].c_str(), &width, &height, &channels, 0));
			if (images[numoftexs] == NULL)
			{
				std::cout << "Texture obj size " << textureobj.paths.size() << std::endl;
				std::cout << "Error in loading the image " << numoftexs << std::endl;
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

	void Texture::CreateandLoadTexture(Texture& textureobj, std::vector<std::string>& paths)
	{
		paths.emplace_back("BG256x256.png"); // 1 - Background
		paths.emplace_back("Transcloud.png"); // 2 - Translucent cloud 
		paths.emplace_back("basecloud.png"); // 3 - Normal cloud
		paths.emplace_back("Tank.png"); // 4 - Enemy
		paths.emplace_back("ActionBG.png"); // 5 - Actions Panel Background
		paths.emplace_back("ActionButton.png"); // 6 - Actions buttons
		paths.emplace_back("MiniMapFrame.png"); // 7 - Minimap
		paths.emplace_back("start.png"); // 8 - Sprite Test
		paths.emplace_back("pause.png"); // 9 - Sprite Test
		paths.emplace_back("exit.png"); // 10 - Sprite Test
		paths.emplace_back("tile003.png"); // 11 - Sprite Test
		paths.emplace_back("DragBox_256x256.png"); // 12 - Drag box

		// FOR IMGUI ========================================================
		paths.emplace_back("folder.png"); // 13 - Folder Icon
		// ==================================================================

		//Texture::createTexturePath("../asset/Transcloud.png", textureobj);
		//Texture::createTexturePath("../asset/basecloud.png", textureobj);
		//Texture::createTexturePath("../asset/Tank.png", textureobj);
		//Texture::createTexturePath("../asset/ActionBG.png", textureobj);
		//Texture::createTexturePath("../asset/ActionButton.png", textureobj);
		//Texture::createTexturePath("../asset/MiniMapFrame.png", textureobj);

		std::string path = "../asset/";
		for (size_t numpath = 0; numpath < paths.size(); numpath++)
		{
			std::string tmp = path + paths[numpath];
			Texture::createTexturePath(tmp, textureobj);
		}
		Texture::loadTexture(textureobj); // Load all textures
	}

	void Texture::deleteTexture(Texture& textureobj)
	{
		glDeleteTextures(1, &(textureobj.textureid));
	}

	int Texture::getTexid()
	{
		return textureid;
	}

	void Texture::setData(int handle, Texture& textureobj)
	{
		textureobj.textureid = handle;
	}
}