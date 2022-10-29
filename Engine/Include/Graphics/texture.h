/* !
@file    texture.h
@author  a.weiren@digipen.edu
@date    23/8/2022

This file loads, sets and deletes the textures in the game
*//*__________________________________________________________________________*/

#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>

namespace TextureNS
{
	class Texture
	{
	public:
		/******************************************************************************/
		/*!
			This function constructs the texture object
		*/
		/******************************************************************************/
		Texture();

		static void createTexturePath(const char* path, Texture& textureobj);
		/******************************************************************************/
		/*!
			This function loads the texture object
		*/
		/******************************************************************************/
		static void loadTexture(Texture& textureobj);
		//static void loadTexture(const char* path);

		static void CreateandLoadTexture(Texture& textureobj);

		/******************************************************************************/
		/*!
			This function deletes the texture object
		*/
		/******************************************************************************/
		static void deleteTexture(Texture& textureobj);
		
		/******************************************************************************/
		/*!
			This function set the handle of the texture
		*/
		/******************************************************************************/
		static void setData(int handle, Texture& textureobj);
		/******************************************************************************/
		/*!
			This function gets the texture id
		*/
		/******************************************************************************/
		int getTexid();
	public:
		GLenum target;
		std::vector<const char*> paths;
		//std::vector<char> 
		//std::vector<unsigned int> textures;
		unsigned int textureid;
	};

	static Texture textureobjects; // Texture obj

}