#include <graphics.h>
#define _USE_MATH_DEFINES //for pi
#include <math.h>
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

/*  _________________________________________________________________________*/
/*! Graphics::Camera2D::init

@param GLFWwindow* pWindow
Pointer to GLFW window currently in use

@param GLApp::GLObject* ptr
Pointer to Camera object

@return none

This function is called once at the initialization of the camera to compute and initialize the camera window
*/
void Graphics::Camera2D::init(GLFWwindow* pWindow, GLApp::GLObject* ptr)
{
	// assign address of object of type GLApp::GLObject with
	// name "Camera" in std::map container GLApp::objects ...
	pgo = ptr;

	// compute camera window's aspect ratio ...
	GLsizei fb_width, fb_height;
	glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
	ar = static_cast<GLfloat>(fb_width) / fb_height;
	int width = int(ar * height);

	// compute camera's up and right vectors ...
	up = { -sin(pgo->orientation.x), cos(pgo->orientation.x) };
	right = { cos(pgo->orientation.x), sin(pgo->orientation.x) };
	// at startup, the camera must be initialized to free camera ...
	view_xform = matrix3x3::mat3x3(1, 0, -pgo->modelCenterPos.x,
		0, 1, -pgo->modelCenterPos.y,
		0, 0, 1);
	
	camwin_to_ndc_xform = matrix3x3::mat3x3(float( 2 / width), 0, 0,
		0, float(2 / height), 0,
		0, 0, 1);
	world_to_ndc_xform = camwin_to_ndc_xform * view_xform;
}

/*  _________________________________________________________________________*/
/*! Graphics::Camera2D::update

@param GLFWwindow* pWindow
Pointer to GLFW window currently in use

@return none

This function is called once per frame to compute and update the camera window
*/
void Graphics::Camera2D::update(GLFWwindow* pWindow)
{
	// compute camera window's aspect ratio ...
	GLsizei fb_width, fb_height;
	glfwGetFramebufferSize(pWindow, &fb_width, &fb_height);
	ar = static_cast<GLfloat>(fb_width) / fb_height;
	int width = int(ar * height);

	// compute camera's up and right vectors ...
	up = { -sin(pgo->orientation.x), cos(pgo->orientation.x) };
	right = { cos(pgo->orientation.x), sin(pgo->orientation.x) };
	// at startup, the camera must be initialized to free camera ...

	// compute other matrices ...
	camwin_to_ndc_xform = matrix3x3::mat3x3(2.f / (float)width, 0, 0,
		0, 2.f / (float)height, 0,
		0, 0, 1);

	if (GLHelper::keystateW == GL_TRUE)
	{
		pgo->modelCenterPos = pgo->modelCenterPos + linear_speed * up;
	}

	if (GLHelper::keystateS == GL_TRUE)
	{
		pgo->modelCenterPos = pgo->modelCenterPos - linear_speed * up;
	}

	if (GLHelper::keystateA == GL_TRUE)
	{
		if (pgo->orientation.x / M_PI * 180 >= 360)
		{
			pgo->orientation.x = 0;
		}
		pgo->orientation.x += pgo->orientation.y;
	}

	if (GLHelper::keystateD == GL_TRUE)
	{
		if (pgo->orientation.x / M_PI * 180 <= -360)
		{
			pgo->orientation.x = 0;
		}
		pgo->orientation.x -= pgo->orientation.y;
	}

	if (GLHelper::keystateV == GL_TRUE)
	{
		if (camtype_flag == GL_TRUE)
		{
			camtype_flag = GL_FALSE;
		}
		else
		{
			camtype_flag = GL_TRUE;
		}
		GLHelper::keystateV = GL_FALSE;
	}

	if (GLHelper::keystateZ == GL_TRUE)
	{
		if (height >= max_height)
		{
			height_chg_dir = -1;
		}
		else if (height <= min_height)
		{
			height_chg_dir = 1;
		}
		height += height_chg_val * height_chg_dir;
	}

	if (camtype_flag == GL_FALSE)
	{
		view_xform = matrix3x3::mat3x3(1, 0, -pgo->modelCenterPos.x,
			0, 1, -pgo->modelCenterPos.y,
			0, 0, 1);
	}
	else
	{
		view_xform = matrix3x3::mat3x3(right.x, right.y, -(right.x * pgo->modelCenterPos.x + right.y * pgo->modelCenterPos.y),
			up.x, up.y, -(up.x * pgo->modelCenterPos.x + up.y * pgo->modelCenterPos.y),
			0, 0, 1);
	}

	world_to_ndc_xform = camwin_to_ndc_xform * view_xform;
	camera2d.world_to_ndc_xform = world_to_ndc_xform; // sets static object's world to ndc
	// check keyboard button presses to enable camera interactivity

	// update camera aspect ratio - this must be done every frame
	// because it is possible for the user to change viewport
	// dimensions

	// update camera's orientation (if required)

	// update camera's up and right vectors (if required)

	// update camera's position (if required)

	// implement camera's zoom effect (if required)

	// compute appropriate world-to-camera view transformation matrix

	// compute window-to-NDC transformation matrix

	// compute world-to-NDC transformation matrix

}

matrix3x3::mat3x3 Graphics::Camera2D::getViewxForm()
{
	return Graphics::Camera2D::view_xform;
}
matrix3x3::mat3x3 Graphics::Camera2D::getCamwintoNDCForm()
{
	return Graphics::Camera2D::camwin_to_ndc_xform;
}
matrix3x3::mat3x3 Graphics::Camera2D::getWorldtoNDCxForm()
{
	return Graphics::camera2d.world_to_ndc_xform;
}

GLApp::GLObject Graphics::Camera2D::getCameraObject()
{
	return *Graphics::Camera2D::pgo;
}
GLint Graphics::Camera2D::getHeight()
{
	return Graphics::Camera2D::height;
}

// Get Window Height
GLint Graphics::Camera2D::getWinHeight()
{
	return (int)GLHelper::height;
}
// Get Window Width
GLint Graphics::Camera2D::getWinWidth()
{
	return (int)GLHelper::width;
}