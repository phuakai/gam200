#include <graphics.h>
#include <buffer.h>
#include <texture.h>
#include <iostream>



Graphics::BatchRenderer::BatchRenderer()
{
	//Model batchmodel{};
	//GLSLShader batchshader{};
	primtype = 0; // Primitive type for all objects in batch
	totaldrawcnt = 0; // Total count of all vertices to be drawn
	totalsize = 0; // To add via subdata
	//std::vector<vertexData> batchdata{}; // All vertices in batch
	totalindicesize = 0;
	//std::vector<GLushort> ebodata{};

	vboid = 0;
	vaoid = 0;
	eboid = 0;
}

void Graphics::BatchRenderer::BatchRender(std::vector<Texture>& texobjs)
{
	batchshader.Use();
	glBindVertexArray(vaoid);

	vboid = Graphics::VBO::init();
	Graphics::VBO::store(vboid, sizeof(Graphics::vertexData) * totalsize, batchdata);

	// Position
	Graphics::VAO::enableattrib(vaoid, 0); // Attrib 0
	Graphics::VBO::bind(vaoid, 0, vboid, 0, sizeof(float) * 8); // Set buffer binding point 
	Graphics::VAO::setattrib(vboid, 0, 2); // Attrib format
	Graphics::VAO::bindattrib(vaoid, 0, 0); // Bind attrib

	// Colour
	Graphics::VAO::enableattrib(vaoid, 1); // Attrib 1
	Graphics::VBO::bind(vaoid, 1, vboid, sizeof(float) * 2, sizeof(float) * 8); // Set buffer binding point 
	Graphics::VAO::setattrib(vaoid, 1, 3); // Attrib format
	Graphics::VAO::bindattrib(vaoid, 1, 1); // Bind attrib

	// Texture Position (U/V)
	Graphics::VAO::enableattrib(vaoid, 2); // Attrib 2
	Graphics::VBO::bind(vaoid, 2, vboid, sizeof(float) * 5, sizeof(float) * 8); // Set buffer binding point 
	Graphics::VAO::setattrib(vaoid, 2, 2); // Attrib format 
	Graphics::VAO::bindattrib(vaoid, 2, 2); // Bind attrib 

	// Texture Index
	Graphics::VAO::enableattrib(vaoid, 3); // Attrib 3
	Graphics::VBO::bind(vaoid, 3, vboid, sizeof(float) * 7, sizeof(float) * 8); // Set buffer binding point 
	Graphics::VAO::setattrib(vaoid, 3, 1); // Attrib format 
	Graphics::VAO::bindattrib(vaoid, 3, 3); // Bind attrib 

	eboid = Graphics::EBO::init();
	int offset = 0;
	for (int i = 0; i < totalindicesize; i+=6)
	{
		ebodata[i + 0] = 0 + offset;
		ebodata[i + 1] = 1 + offset;
		ebodata[i + 2] = 2 + offset;
		ebodata[i + 3] = 2 + offset;
		ebodata[i + 4] = 3 + offset;
		ebodata[i + 5] = 0 + offset;

		offset += 4;
	}

	Graphics::EBO::store(eboid, sizeof(GLushort) * totalindicesize, ebodata);

	Graphics::EBO::bind(vaoid, eboid);

	//std::cout << "Texture units " << texobjs.size() << std::endl;
	glBindTextureUnit(0, texobjs[0].getTexid());
	glBindTextureUnit(1, texobjs[1].getTexid());

	GLuint tex_loc = glGetUniformLocation(batchshader.GetHandle(), "ourTexture");
	int samplers[2] = { 0, 1 };
	glUniform1iv(tex_loc, 2, samplers);

	GLboolean UniformModulate = glGetUniformLocation(batchshader.GetHandle(), "modulatebool");
	//std::cout << "Modul " << GLApp::modulate << " Text " << GLApp::textures << std::endl;
	glUniform1i(UniformModulate, GLApp::modulate); // Modulate bool temp

	GLboolean UniformTextures = glGetUniformLocation(batchshader.GetHandle(), "texturebool");
	glUniform1i(UniformTextures, GLApp::textures); // Texture bool temp

	glDrawElements(primtype, totaldrawcnt, GL_UNSIGNED_SHORT, NULL);

	Graphics::VAO::unbind();

	batchshader.UnUse();

}

void Graphics::BatchRenderer::BatchClear()
{
	totalindicesize = 0;
	totaldrawcnt = 0;
	totalsize = 0;
	ebodata.clear();
	batchdata.clear();
	glDeleteBuffers(1, &vboid);
	glDeleteBuffers(1, &eboid);
}

void Graphics::BatchRenderer::BatchDelete()
{
	glDeleteBuffers(1, &vaoid);
}
