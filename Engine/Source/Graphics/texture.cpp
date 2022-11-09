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
			std::cout << "Freeing the image " << numoftexs << std::endl;
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
		paths.emplace_back("tile000.png"); // 8 - Sprite Test
		paths.emplace_back("tile001.png"); // 9 - Sprite Test
		paths.emplace_back("tile002.png"); // 10 - Sprite Test
		paths.emplace_back("tile003.png"); // 11 - Sprite Test
		paths.emplace_back("DragBox_256x256.png"); // 12 - Drag box
		paths.emplace_back("EnemyBuilding_Reclaimed256x256.png"); // 13 - Enemy reclaimed
		paths.emplace_back("EnemyBuilding_Unclaimed256x256.png"); // 14 - Enemy unclaimed
		paths.emplace_back("Grasspatch_transparent_256x256.png"); // 15 - Tiles
		paths.emplace_back("Animations/BeetleAttack/Character_Beetle_Attack_1_256x256.png"); // 16 - Beetle Attack
		paths.emplace_back("Animations/BeetleAttack/Character_Beetle_Attack_2_256x256.png"); // 17 - Beetle Attack
		paths.emplace_back("Animations/BeetleAttack/Character_Beetle_Attack_3_256x256.png"); // 18 - Beetle Attack
		paths.emplace_back("Animations/BeetleAttack/Character_Beetle_Attack_4_256x256.png"); // 19 - Beetle Attack
		paths.emplace_back("Animations/BeetleAttack/Character_Beetle_Attack_5_256x256.png"); // 20 - Beetle Attack
		paths.emplace_back("Animations/BeetleAttack/Character_Beetle_Attack_6_256x256.png"); // 21 - Beetle Attack
		paths.emplace_back("Animations/BeetleAttack/Character_Beetle_Attack_7_256x256.png"); // 22 - Beetle Attack
		paths.emplace_back("Animations/BeetleAttack/Character_Beetle_Attack_8_256x256.png"); // 23 - Beetle Attack
		paths.emplace_back("Animations/BeetleAttack/Character_Beetle_Attack_9_256x256.png"); // 24 - Beetle Attack
		paths.emplace_back("Animations/BeetleWalk/Character_Beetle_Walk_1_256x256.png"); // 25 - Beetle Walk
		paths.emplace_back("Animations/BeetleWalk/Character_Beetle_Walk_2_256x256.png"); // 26 - Beetle Walk
		paths.emplace_back("Animations/BeetleWalk/Character_Beetle_Walk_3_256x256.png"); // 27 - Beetle Walk
		paths.emplace_back("Animations/BeetleWalk/Character_Beetle_Walk_4_256x256.png"); // 28 - Beetle Walk
		paths.emplace_back("Animations/BeetleWalk/Character_Beetle_Walk_5_256x256.png"); // 29 - Beetle Walk
		paths.emplace_back("Animations/BeetleWalk/Character_Beetle_Walk_6_256x256.png"); // 30 - Beetle Walk
		paths.emplace_back("Animations/RobotEnemy/Robot_Enemy_Attack_1_256x256.png"); // 31 - Robot Enemy Attack
		paths.emplace_back("Animations/RobotEnemy/Robot_Enemy_Attack_2_256x256.png"); // 32 - Robot Enemy Attack
		paths.emplace_back("Animations/RobotEnemy/Robot_Enemy_Attack_3_256x256.png"); // 33 - Robot Enemy Attack
		paths.emplace_back("Animations/RobotEnemy/Robot_Enemy_Attack_4_256x256.png"); // 34 - Robot Enemy Attack
		paths.emplace_back("Animations/RobotEnemy/Robot_Enemy_Attack_5_256x256.png"); // 35 - Robot Enemy Attack
		paths.emplace_back("Animations/RobotEnemy/Robot_Enemy_Attack_6_256x256.png"); // 36 - Robot Enemy Attack
		paths.emplace_back("Enemy.png"); // 37 - Robot Enemy 
		paths.emplace_back("exit.png"); // 38 - Exit
		//paths.emplace_back("pause.png"); // 39 - Pause
		//paths.emplace_back("play.png"); // 40 - Play

		// FOR IMGUI ========================================================
		paths.emplace_back("folder.png"); // 41 - Folder Icon
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