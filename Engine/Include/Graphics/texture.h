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

namespace Graphics
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

		/******************************************************************************/
		/*!
			This function loads the texture object
		*/
		/******************************************************************************/
		static void loadTexture(const char* path, Graphics::Texture& textureobj);
		//static void loadTexture(const char* path);

		/******************************************************************************/
		/*!
			This function deletes the texture object
		*/
		/******************************************************************************/
		static void deleteTexture(Graphics::Texture& textureobj);
		
		/******************************************************************************/
		/*!
			This function set the handle of the texture
		*/
		/******************************************************************************/
		static void setData(int handle, Graphics::Texture& textureobj);
		/******************************************************************************/
		/*!
			This function gets the texture id
		*/
		/******************************************************************************/
		int getTexid();
	public:
		int curtexid;
		GLenum target;
		//std::vector<char> 
		//std::vector<unsigned int> textures;
		unsigned int textureid;
	};

	/******************************************************************************/
	/*!
		This function creates the texture vector containing all available textures
	*/
	/******************************************************************************/
	void createTextureVector(Texture& texobjs, int texturecount);

	static Graphics::Texture textureobjects; // Texture vector

}