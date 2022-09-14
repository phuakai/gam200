#include <texture.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <GL/glew.h>
#include <fstream>


void Graphics::Texture::loadTexture(std::string path, Graphics::Texture &textureobj)
{
	//std::ifstream ifs{ path, std::ios::in | std::ios::binary };
	//if (!ifs)
	//{
	//	std::cout << "ERROR: Unable to open image file: " << path << "\n";
	//	exit(EXIT_FAILURE);
	//}
	//GLuint width{ 256 }, height{ 256 }, bytes_per_texel{ 4 };
	//char* ptr_texels = new char[width * height * bytes_per_texel];

	////const void* ptr_texels = new float[width * height];
	//ifs.read(ptr_texels, width * height * bytes_per_texel);

	//GLuint texobj_hdl;
	//// define and initialize a handle to texture object that will
	//// encapsulate two-dimensional textures
	//glCreateTextures(GL_TEXTURE_2D, 1, &texobj_hdl);
	//// allocate GPU storage for texture image data loaded from file
	//glTextureStorage2D(texobj_hdl, 1, GL_RGBA8, width, height);
	//// copy image data from client memory to GPU texture buffer memory
	//glTextureSubImage2D(texobj_hdl, 0, 0, 0, width, height,
	//	GL_RGBA, GL_UNSIGNED_BYTE, ptr_texels);
	//// client memory not required since image is buffered in GPU memory
	//delete[] ptr_texels;
	//// nothing more to do - return handle to texture object
	//textureobj.setData(texobj_hdl, textureobj);
	int width, height, channels;
	//("../images/factory.png")

	unsigned char* img = stbi_load("../images/tree.png", &width, &height, &channels, 0);
	if (img == NULL) {
		printf("Error in loading the image\n");
		exit(1);
	}

	std::cout << "Channel " << channels << std::endl;
	glGenTextures(1,  &(textureobj.texture));
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
	//std::cout << "Loaded texnum " << textureobj.texture << std::endl;


}

int Graphics::Texture::getTexid()
{
	return texture;
}

void Graphics::Texture::setData(int handle, Graphics::Texture& textureobj)
{
	textureobj.texture = handle;
}