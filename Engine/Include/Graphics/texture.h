/* !
@file    texture.h
@author  a.weiren@digipen.edu
@date    23/8/2022

This file loads, sets and deletes the textures in the game
*//*__________________________________________________________________________*/

#pragma once

#include <string>
#include <vector>

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
	private:
		unsigned int texture;
	};

	/******************************************************************************/
	/*!
		This function creates the texture vector containing all available textures
	*/
	/******************************************************************************/
	void createTextureVector(std::vector<Texture>& texobjs, int texturecount);

	static std::vector<Graphics::Texture> textureobjects; // Texture vector

}