#include <graphics.h>
#define _USE_MATH_DEFINES //for pi
#include <math.h>
#include <buffer.h>
#include <texture.h>
#include <iostream>

extern bool modulate, alphablend, textures;
extern Graphics::Texture texobj;

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

void Graphics::BatchRenderer::BatchRender()
{
	//shd_ref->second.Use();
	//glBindVertexArray(mdl_ref->second.getVAOid()); // Rebind VAO
	//glNamedBufferSubData(mdl_ref->second.getVBOid(), 0, sizeof(Graphics::vertexData) * vertexData.size(), vertexData.data()); // Set new buffer index with subdata
	//GLuint tex_loc = glGetUniformLocation(shd_ref->second.GetHandle(), "ourTexture");
	//glUniform1i(tex_loc, 0);

	//GLboolean UniformModulate = glGetUniformLocation(shd_ref->second.GetHandle(), "modulatebool");
	//glUniform1i(UniformModulate, modulate);

	//GLboolean UniformTextures = glGetUniformLocation(shd_ref->second.GetHandle(), "texturebool");
	//glUniform1i(UniformTextures, textures);
	//// call glDrawElements with appropriate arguments
	//glDrawElements(mdl_ref->second.getPrimitiveType(), mdl_ref->second.getDrawCnt(), GL_UNSIGNED_SHORT, NULL);

	//// unbind VAO and unload shader program
	//glBindVertexArray(0);

	//shd_ref->second.UnUse();

	batchshader.Use();
	glBindVertexArray(vaoid);

	for (int i = 0; i < batchdata.size(); i++)
	{
		//std::cout << "This is I " << i << std::endl;
		//std::cout << "Coords " << batchdata[i].posVtx.x << ", " << batchdata[i].posVtx.y << std::endl;
	}


	vboid = Graphics::VBO::init();
	//std::cout << "Size " << sizeof(Graphics::vertexData) * totalsize << std::endl;
	Graphics::VBO::store(vboid, sizeof(Graphics::vertexData) * totalsize, batchdata);

	Graphics::VAO::enableattrib(vaoid, 0); // Attrib 0
	Graphics::VBO::bind(vaoid, 0, vboid, 0, sizeof(float) * 7); // Set buffer binding point 0
	Graphics::VAO::setattrib(vboid, 0, 2); // Attrib format
	Graphics::VAO::bindattrib(vaoid, 0, 0); // Bind attrib

	Graphics::VAO::enableattrib(vaoid, 1); // Attrib 0
	Graphics::VBO::bind(vaoid, 1, vboid, sizeof(float) * 2, sizeof(float) * 7); // Set buffer binding point 0
	Graphics::VAO::setattrib(vaoid, 1, 3); // Attrib format
	Graphics::VAO::bindattrib(vaoid, 1, 1); // Bind attrib


	Graphics::VAO::enableattrib(vaoid, 2); // Attrib 1
	Graphics::VBO::bind(vaoid, 2, vboid, sizeof(float) * 5, sizeof(float) * 7); // Set buffer binding point 1
	Graphics::VAO::setattrib(vaoid, 2, 2); // Attrib format 1
	Graphics::VAO::bindattrib(vaoid, 2, 2); // Bind attrib 1

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

	//glNamedBufferSubData(vboid, 0, sizeof(Graphics::vertexData) * totalsize, batchdata.data());
	//std::cout << "Inside batch render batch shader " << batchshader.GetHandle() << std::endl;
	GLuint tex_loc = glGetUniformLocation(batchshader.GetHandle(), "ourTexture");
	glUniform1i(tex_loc, 0);

	GLboolean UniformModulate = glGetUniformLocation(batchshader.GetHandle(), "modulatebool");
	glUniform1i(UniformModulate, modulate); // Modulate bool temp

	GLboolean UniformTextures = glGetUniformLocation(batchshader.GetHandle(), "texturebool");
	glUniform1i(UniformTextures, textures); // Texture bool temp

	glDrawElements(primtype, totaldrawcnt, GL_UNSIGNED_SHORT, NULL);

	Graphics::VAO::unbind();

	batchshader.UnUse();


	//std::cout << "Cleared\n";
	totalindicesize = 0;
	totaldrawcnt = 0;
	totalsize = 0;
	ebodata.clear();
	batchdata.clear();
	glDeleteBuffers(1, &vboid);
	glDeleteBuffers(1, &eboid);
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