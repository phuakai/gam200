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
	if (primtype == GL_TRIANGLES || primtype == GL_TRIANGLE_STRIP || primtype == GL_TRIANGLE_FAN)
	{
		for (int i = 0; i < (totalindicesize-5); i += 6)
		{
			ebodata[i + 0] = 0 + offset;
			ebodata[i + 1] = 1 + offset;
			ebodata[i + 2] = 2 + offset;
			ebodata[i + 3] = 2 + offset;
			ebodata[i + 4] = 3 + offset;
			ebodata[i + 5] = 0 + offset;

			offset += 4;
		}
	}
	
	else if (primtype == GL_POINTS || primtype == GL_LINES || primtype == GL_LINE_LOOP || primtype == GL_LINE_STRIP)
	{
		for (int i = 0; i < (totalindicesize-1); i += 2)
		{
			ebodata[i + 0] = 0 + offset;
			ebodata[i + 1] = 1 + offset;
			offset += 2;
		}
	}

	Graphics::EBO::store(eboid, sizeof(GLushort) * totalindicesize, ebodata);

	Graphics::EBO::bind(vaoid, eboid);

	//std::cout << "Texture units " << texobjs.size() << std::endl;
	glBindTextureUnit(0, texobjs[0].getTexid());
	glBindTextureUnit(1, texobjs[1].getTexid()); // Basetree
	glBindTextureUnit(2, texobjs[2].getTexid()); // Grass
	glBindTextureUnit(3, texobjs[3].getTexid()); // Circuwu
	glBindTextureUnit(4, texobjs[4].getTexid()); // Circuwu
	glBindTextureUnit(5, texobjs[5].getTexid()); // Dragbox
	glBindTextureUnit(6, texobjs[6].getTexid()); // Enemy
	glBindTextureUnit(7, texobjs[7].getTexid()); // BG1
	glBindTextureUnit(8, texobjs[8].getTexid()); // BG2

	GLuint tex_loc = glGetUniformLocation(batchshader.GetHandle(), "ourTexture");
	int samplers[9] = { 0, 1, 2, 3, 4, 5, 6, 7, 8};
	glUniform1iv(tex_loc, 9, samplers);

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
