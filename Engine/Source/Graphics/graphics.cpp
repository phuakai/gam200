/* !
@file    graphics.cpp
@author  a.weiren@digipen.edu
@date    23/8/2022

This file handles the batch rendering of the game
*//*__________________________________________________________________________*/
#include <app.h>
#include <buffer.h>
#include <texture.h>
#include <iostream>
#include "framebuffer.h"
#include "ecs.h"
#include "mainHeader.h"
#include "transform.h"
#include "camera.h"

#include "stb_image.h"

extern FrameBufferNS::frameBuffer mainFrame;
extern CameraNS::Camera2D camera2d;

void RenderNS::InstancedRenderer::InstanceRender(TextureNS::Texture& texobjs)
{
	instanceshader.Use(); //Use shader prog

	glBindVertexArray(vaoid);

	GLuint headervboid = BufferNS::VBO::createVBO();
	BufferNS::VBO::createVBOstorage(headervboid, sizeof(ModelNS::modelVtxData) * headerdata.size(), headerdata); // Data passed in
	// Note that for instance, stored data is 1 position, 1 colour, 1 texture pos, 1 texture index (or texture array later on)
	// and an array consisting of the offsets for the different instance positions

	GLuint instancevboid = BufferNS::VBO::createVBO();
	BufferNS::VBO::createVBOstorage(instancevboid, sizeof(matrix3x3::mat3x3) * instancedata.size(), instancedata); // Data passed in

	// Position
	BufferNS::VAO::enableVAOattrib(vaoid, 0); // Attrib 0
	BufferNS::VBO::bindVBO(vaoid, 0, headervboid, 0, sizeof(float) * 8); // Set buffer binding point 
	// Pos is vec2
	BufferNS::VAO::setVAOattrib(vaoid, 0, 2); // Attrib format
	BufferNS::VAO::bindVAOattrib(vaoid, 0, 0); // Bind attrib

	// Colour
	BufferNS::VAO::enableVAOattrib(vaoid, 1); // Attrib 1
	BufferNS::VBO::bindVBO(vaoid, 1, headervboid, sizeof(float) * 2, sizeof(float) * 8); // Set buffer binding point 
	// Colour is vec3
	BufferNS::VAO::setVAOattrib(vaoid, 1, 3); // Attrib format
	BufferNS::VAO::bindVAOattrib(vaoid, 1, 1); // Bind attrib

	// Texture Position (U/V)
	BufferNS::VAO::enableVAOattrib(vaoid, 2); // Attrib 2
	BufferNS::VBO::bindVBO(vaoid, 2, headervboid, sizeof(float) * 5, sizeof(float) * 8); // Set buffer binding point 
	// Texpos is vec2
	BufferNS::VAO::setVAOattrib(vaoid, 2, 2); // Attrib format 
	BufferNS::VAO::bindVAOattrib(vaoid, 2, 2); // Bind attrib 

	// Texture Index
	BufferNS::VAO::enableVAOattrib(vaoid, 3); // Attrib 3
	BufferNS::VBO::bindVBO(vaoid, 3, headervboid, sizeof(float) * 7, sizeof(float) * 8); // Set buffer binding point 
	// Texindex is 1 float
	BufferNS::VAO::setVAOattrib(vaoid, 3, 1); // Attrib format 
	BufferNS::VAO::bindVAOattrib(vaoid, 3, 3); // Bind attrib 

	BufferNS::VBO::bindVBO(vaoid, 4, instancevboid, 0, sizeof(matrix3x3::mat3x3)); // Set buffer binding point 
	// Matrix requires n consecutive input locations, where N is the columns in a matrix
	// So mat3x3 is 3 vertex attributes
	int matrix_loc = 4;
	for (int col = 0; col < 3; col++)
	{
		// Instancing offset array
		BufferNS::VAO::enableVAOattrib(vaoid, matrix_loc+col); // Attrib 4 to 7
		// Not sure what to put for last parameter of bind
		//glVertexArrayBindingDivisor(vaoid, matrix_loc + col, 1); // Same as below
		glVertexAttribDivisor(matrix_loc + col, 1);
		BufferNS::VAO::setVAOattrib(vaoid, matrix_loc + col, 3, sizeof(float) * 3 * col); // Attrib format 
		BufferNS::VAO::bindVAOattrib(vaoid, matrix_loc + col, 4); // Bind attrib 
	}
	// Creating ebo
	GLuint eboid = BufferNS::EBO::createEBO();
	ebodata[0] = 0;
	ebodata[1] = 1;
	ebodata[2] = 2;
	ebodata[3] = 2;
	ebodata[4] = 3;
	ebodata[5] = 0;
	BufferNS::EBO::createEBOstorage(eboid, sizeof(GLushort) * ebodata.size(), ebodata);
	BufferNS::EBO::bindEBO(vaoid, eboid);

	glBindTextureUnit(0, texobjs.textureid);
	GLuint tex_loc = glGetUniformLocation(instanceshader.GetHandle(), "arrayTexture");
	glUniform1i(tex_loc, 0);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL, entitycounter);

	mainFrame.drawFrameBuffer();
	GLuint tex2_loc = glGetUniformLocation(frameshader.GetHandle(), "screenTexture");
	glUniform1i(tex2_loc, 0);
	instanceshader.UnUse();
	BufferNS::VAO::unbindVAO();

	glDeleteBuffers(1, &headervboid);
	glDeleteBuffers(1, &instancevboid);
	glDeleteBuffers(1, &eboid);
}

void RenderNS::InstancedRenderer::InstanceClear()
{
	instancedata.clear();
}

void RenderNS::InstancedRenderer::InstanceDelete()
{
	glDeleteBuffers(1, &vaoid);
}




RenderNS::BatchRenderer::BatchRenderer()
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

void RenderNS::BatchRenderer::BatchRender(std::vector<TextureNS::Texture>& texobjs)
{
	batchshader.Use();
	glBindVertexArray(vaoid);

	vboid = BufferNS::VBO::createVBO();
	BufferNS::VBO::createVBOstorage(vboid, sizeof(ModelNS::modelVtxData) * totalsize, batchdata);

	// Position
	BufferNS::VAO::enableVAOattrib(vaoid, 0); // Attrib 0
	BufferNS::VBO::bindVBO(vaoid, 0, vboid, 0, sizeof(float) * 8); // Set buffer binding point 
	BufferNS::VAO::setVAOattrib(vboid, 0, 2); // Attrib format
	BufferNS::VAO::bindVAOattrib(vaoid, 0, 0); // Bind attrib

	// Colour
	BufferNS::VAO::enableVAOattrib(vaoid, 1); // Attrib 1
	BufferNS::VBO::bindVBO(vaoid, 1, vboid, sizeof(float) * 2, sizeof(float) * 8); // Set buffer binding point 
	BufferNS::VAO::setVAOattrib(vaoid, 1, 3); // Attrib format
	BufferNS::VAO::bindVAOattrib(vaoid, 1, 1); // Bind attrib

	// Texture Position (U/V)
	BufferNS::VAO::enableVAOattrib(vaoid, 2); // Attrib 2
	BufferNS::VBO::bindVBO(vaoid, 2, vboid, sizeof(float) * 5, sizeof(float) * 8); // Set buffer binding point 
	BufferNS::VAO::setVAOattrib(vaoid, 2, 2); // Attrib format 
	BufferNS::VAO::bindVAOattrib(vaoid, 2, 2); // Bind attrib 

	// Texture Index
	BufferNS::VAO::enableVAOattrib(vaoid, 3); // Attrib 3
	BufferNS::VBO::bindVBO(vaoid, 3, vboid, sizeof(float) * 7, sizeof(float) * 8); // Set buffer binding point 
	BufferNS::VAO::setVAOattrib(vaoid, 3, 1); // Attrib format 
	BufferNS::VAO::bindVAOattrib(vaoid, 3, 3); // Bind attrib 

	eboid = BufferNS::EBO::createEBO();
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

	BufferNS::EBO::createEBOstorage(eboid, sizeof(GLushort) * totalindicesize, ebodata);

	BufferNS::EBO::bindEBO(vaoid, eboid);

	glDrawElements(primtype, totaldrawcnt, GL_UNSIGNED_SHORT, NULL);

	BufferNS::VAO::unbindVAO();

	batchshader.UnUse();
}

void RenderNS::BatchRenderer::BatchClear()
{
	totalindicesize = 0;
	totaldrawcnt = 0;
	totalsize = 0;
	ebodata.clear();
	batchdata.clear();
	glDeleteBuffers(1, &vboid);
	glDeleteBuffers(1, &eboid);
}

void RenderNS::BatchRenderer::BatchDelete()
{
	glDeleteBuffers(1, &vaoid);
}

void RenderNS::DrawFunc(RenderNS::InstancedRenderer& instanceobj, FrameBufferNS::frameBuffer& frame, GLSLShader shadertouse, std::map<std::string, ModelNS::Model> models, TextureNS::Texture texobj)
{
	instanceobj.instanceshader = shadertouse;
	entitydraw(instanceobj, models);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindFramebuffer(GL_FRAMEBUFFER, frame.framebuffer);
	glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	instanceobj.InstanceRender(texobj);
	frame.drawFrameBuffer();
	instanceobj.InstanceClear();

}

void RenderNS::entitydraw(RenderNS::InstancedRenderer& instanceobj, std::map<std::string, ModelNS::Model> models)
{
	std::vector<EntityID> entities = ecs.getEntities();
	for (int i = 0; i < entities.size(); i++)
	{
		if (ecs.GetComponent<Render>(entities[i]) == nullptr) // Added check for NIL objects
		{
			continue;
		}

		BaseInfo* curobjBaseInfo = ecs.GetComponent<BaseInfo>(entities[i]);

		// Below code (2 lines) is for fow
		if (!ecs.GetComponent<Render>(entities[i])->render)
		{
			continue;
		}

		int texid{};
		Texture* curobjTexture = ecs.GetComponent<Texture>(entities[i]);
		if (ecs.GetComponent<Texture>(entities[i]) != nullptr)
		{
			texid = curobjTexture->textureID;
			//std::cout << "Sprite step " << curobjTexture->spriteStep << std::endl;
		}
		Render* curobj = ecs.GetComponent<Render>(entities[i]);

		std::vector<vector3D::Vec3> clr_vtx
		{
			vector3D::Vec3(curobj->color.r, curobj->color.g, curobj->color.b), vector3D::Vec3(curobj->color.r, curobj->color.g, curobj->color.b),
			vector3D::Vec3(curobj->color.r, curobj->color.g, curobj->color.b), vector3D::Vec3(curobj->color.r, curobj->color.g, curobj->color.b)
		};

		std::vector<vector2D::Vec2> texcoord;
		texcoord.emplace_back(vector2D::Vec2(0.f, 0.f)); // Bottom left
		texcoord.emplace_back(vector2D::Vec2(1.f, 0.f)); // Bottom right
		texcoord.emplace_back(vector2D::Vec2(1.f, 1.f)); // Top right
		texcoord.emplace_back(vector2D::Vec2(0.f, 1.f)); // Top left
		// 
		//texcoord.emplace_back(vector2D::Vec2(((float)(curobjTexture->spriteStep - 1) / curobjTexture->numberOfSprites), 0.f)); // Bottom left
		//texcoord.emplace_back(vector2D::Vec2(((float)(curobjTexture->spriteStep) / curobjTexture->numberOfSprites), 0.f)); // Bottom right
		//texcoord.emplace_back(vector2D::Vec2(((float)(curobjTexture->spriteStep) / curobjTexture->numberOfSprites), 1.f)); // Top right
		//texcoord.emplace_back(vector2D::Vec2(((float)(curobjTexture->spriteStep - 1) / curobjTexture->numberOfSprites), 1.f)); // Top left


		ModelNS::modelVtxData tmpHeaderData;
		std::vector<ModelNS::modelVtxData> vertexData;
		std::vector<matrix3x3::mat3x3> testdata;

		std::vector<vector2D::vec2D> poscoord; // CALCULATE POSITION FROM CENTER
		float halfwidth = curobjBaseInfo->dimension.x / 2.f;
		float halfheight = curobjBaseInfo->dimension.y / 2.f;
		poscoord.emplace_back(vector2D::vec2D(curobjBaseInfo->position.x - halfwidth, curobjBaseInfo->position.y - halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobjBaseInfo->position.x + halfwidth, curobjBaseInfo->position.y - halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobjBaseInfo->position.x + halfwidth, curobjBaseInfo->position.y + halfheight));
		poscoord.emplace_back(vector2D::vec2D(curobjBaseInfo->position.x - halfwidth, curobjBaseInfo->position.y + halfheight));


		std::vector <vector2D::vec2D> ndccoord;
		for (int i = 0; i < poscoord.size(); i++)
		{
			ndccoord.emplace_back(poscoord[i]);
		}

		for (int j = 0; j < ndccoord.size(); ++j)
		{
			ModelNS::modelVtxData tmpVtxData;
			//tmpVtxData.posVtx = ndccoord[i];

			tmpVtxData.clrVtx = clr_vtx[j];
			tmpVtxData.posVtx = models["square"].model_coords[j];
			tmpVtxData.txtVtx = texcoord[j];
			tmpVtxData.txtIndex = texid; // To be removed
			vertexData.emplace_back(tmpVtxData);
		}

		matrix3x3::mat3x3 translate = Transform::createTranslationMat(vector2D::vec2D(curobjBaseInfo->position.x, curobjBaseInfo->position.y));
		matrix3x3::mat3x3 scale = Transform::createScaleMat(vector2D::vec2D(curobjBaseInfo->dimension.x * 2.5f, curobjBaseInfo->dimension.y * 2.5f));
		matrix3x3::mat3x3 rot = Transform::createRotationMat(0.f);

		matrix3x3::mat3x3 model_to_world = translate * rot * scale;


		matrix3x3::mat3x3 world_to_ndc_xform = camera2d.getWorldtoNDCxForm();

		matrix3x3::mat3x3 model_to_ndc_xformnotglm = world_to_ndc_xform * model_to_world;

		matrix3x3::mat3x3 model_to_ndc_xform = matrix3x3::mat3x3
		(
			//model_to_ndc_xformnotglm.m[0], model_to_ndc_xformnotglm.m[3], model_to_ndc_xformnotglm.m[6],
			//model_to_ndc_xformnotglm.m[1], model_to_ndc_xformnotglm.m[4], model_to_ndc_xformnotglm.m[7],
			//model_to_ndc_xformnotglm.m[2], model_to_ndc_xformnotglm.m[5], texid
			model_to_ndc_xformnotglm.m[0], curobj->color.r, curobj->color.g,
			curobj->color.b, model_to_ndc_xformnotglm.m[4], model_to_ndc_xformnotglm.m[7],
			model_to_ndc_xformnotglm.m[2], model_to_ndc_xformnotglm.m[5], texid + (curobjTexture->spriteStep - 1)

		);

		testdata.emplace_back(model_to_ndc_xform); // Emplace back a base 1, 1 translation

		instanceobj.headerdata.clear();
		instanceobj.ebodata.clear();
		instanceobj.headerdata.insert(instanceobj.headerdata.end(), vertexData.begin(), vertexData.end());
		instanceobj.instancedata.insert(instanceobj.instancedata.end(), testdata.begin(), testdata.end());
		instanceobj.ebodata.insert(instanceobj.ebodata.end(), models["square"].primitive.begin(), models["square"].primitive.end());
		instanceobj.vaoid = models["square"].getVAOid();
		instanceobj.entitycounter++;

		testdata.clear();
	}
}
