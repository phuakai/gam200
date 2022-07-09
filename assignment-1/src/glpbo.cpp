/*!
@file    glpbo.cpp
@author  a.weiren@digipen.edu
@date    09/07/2022

This file implements functionality useful and necessary to build OpenGL
applications including use of external APIs such as GLFW to create a
window and start up an OpenGL context and to extract function pointers
to OpenGL implementations.


*//*__________________________________________________________________________*/
#include "glpbo.h"
#include "glhelper.h"
#include "dpml.h"
#include <algorithm>
#include <iomanip>
#include <chrono> // Time
#include <random>

#define M_PI 3.1415926535897932384626433832795 // Pi macro def

GLsizei GLPbo::width, GLPbo::height;
GLsizei GLPbo::pixel_cnt, GLPbo::byte_cnt;
GLPbo::Color* GLPbo::ptr_to_pbo;
GLuint GLPbo::vaoid;
GLuint GLPbo::elem_cnt;
GLuint GLPbo::pboid;
GLuint GLPbo::texid;       
GLSLShader GLPbo::shdr_pgm;
GLPbo::Color GLPbo::clear_clr;

std::vector<std::pair<std::string, GLPbo::Model>> GLPbo::modelstorage;

std::default_random_engine dre;
int modelnum;
bool rotflag;
int colorcounter;

/*  _________________________________________________________________________*/
/*! GLPbo::init

@param GLsizei w
Width of window

@param GLsizei h
Height of window

@return none

This function is called once on initialization to initialize values and call setup functions
*/
void GLPbo::init(GLsizei w, GLsizei h)
{

	// create default engine as source of randomness
	dre.seed((int)std::chrono::high_resolution_clock::now().time_since_epoch().count());
	// get numbers in range [-1, 1) - notice that interval is half-
	// open on the right. When you submit the tutorial, specify a
	// closed range [-1, 1]
	std::uniform_real_distribution<float> urdf(0.0f, 1.0f);

	colorcounter = 0;
	modelnum = 0;
	rotflag = false;

	GLPbo::width = w;
	GLPbo::height = h;
	GLPbo::pixel_cnt = GLPbo::width * GLPbo::height;
	GLPbo::byte_cnt = GLPbo::pixel_cnt * 4;

	GLPbo::set_clear_color(255, 0, 255, 255);
	glCreateBuffers(1, &pboid);

	glNamedBufferStorage(pboid, byte_cnt, nullptr, GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT);

	// binds the pbo buffer
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboid);
	GLPbo::ptr_to_pbo = reinterpret_cast<GLPbo::Color*>(glMapNamedBuffer(pboid, GL_WRITE_ONLY));
	GLPbo::clear_color_buffer();

	std::ifstream ifs{ "../scenes/ass-1.scn", std::ios::in };
	if (!ifs)
	{
		std::cout << "ERROR: Unable to open scene file: "
			<<  "../scenes/ass-1.scn" << "\n";
		exit(EXIT_FAILURE);
	}
	std::vector<std::string> sceneitems;
	std::string line;
	while (getline(ifs, line))
	{
		sceneitems.emplace_back(line);
	}

	for (std::string modelname : sceneitems)
	{
		GLPbo::Model modelinfo;

		if (!DPML::parse_obj_mesh("../meshes/" + modelname + ".obj", modelinfo.pm, modelinfo.nml, modelinfo.tex, modelinfo.tri, false, true, true))
		{
			std::cout << "ERROR: Unable to parse mesh file: " << modelname << "\n";
			exit(EXIT_FAILURE);
		}
		modelinfo.name = modelname;
		modelinfo.name[0] = toupper(modelinfo.name[0]);
		//Init starting values
		modelinfo.angle = 0.f;
		modelinfo.rendertype = 0;

		for (size_t i = 0; i < modelinfo.pm.size(); i++)
		{
			modelinfo.nml[i] *= 0.5f;
			modelinfo.nml[i] += glm::vec3(0.5f, 0.5f, 0.5f);
			modelinfo.pd.emplace_back();
		}
		viewport_transformation(modelinfo);

		for (size_t i = 0, j = 0; i < modelinfo.tri.size() / 3; i++, j+= 3)
		{
			float signedarea = ((modelinfo.pd[modelinfo.tri[j + 1]].x - modelinfo.pd[modelinfo.tri[j]].x) 
				* (modelinfo.pd[modelinfo.tri[j + 2]].y - modelinfo.pd[modelinfo.tri[j]].y)
				- (modelinfo.pd[modelinfo.tri[j + 2]].x - modelinfo.pd[modelinfo.tri[j]].x)
				* (modelinfo.pd[modelinfo.tri[j + 1]].y - modelinfo.pd[modelinfo.tri[j]].y));
			modelinfo.signedarea.emplace_back(signedarea);
			modelinfo.colors.emplace_back(glm::vec3{ urdf(dre) * 230, urdf(dre) * 230, urdf(dre) * 230 });

		}
		GLPbo::modelstorage.emplace_back(std::make_pair(".. / meshes / " + modelname + ".obj", modelinfo));
	}
	
	set_clear_color(255, 255, 255, 255);


	glCreateTextures(GL_TEXTURE_2D, 1, &texid);

	glTextureStorage2D(texid, 1, GL_RGBA8, width, height);

	glCreateBuffers(1, &pboid);
	glNamedBufferStorage
	(	
		pboid,
		byte_cnt,
		nullptr,
		GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT
	);

	setup_quad_vao();

	setup_shdrpgm();

}

/*  _________________________________________________________________________*/
/*! GLPbo::viewport_transformation

@param Model& mdltransform
Model to be used to calculate transformations

@return none

This function is called to initialize and update the viewport transformation
*/
void GLPbo::viewport_transformation(Model& mdltransform)
{

	glm::mat3 test(	GLPbo::width / 2.f, 0, 0,
					0, GLPbo::height / 2.f, 0,
					GLPbo::width / 2.f, GLPbo::height / 2.f, 1);
	glm::mat3 rot(cos(mdltransform.angle / 180.f * M_PI), sin(mdltransform.angle / 180.f * M_PI), 0,
		-sin(mdltransform.angle / 180.f * M_PI), cos(mdltransform.angle / 180.f * M_PI), 0,
		0, 0, 1);
	for (size_t i = 0; i < mdltransform.pm.size(); i++)
	{
		mdltransform.pm[i].z = 1.f;
		glm::vec3 tmp = rot * mdltransform.pm[i];
		glm::vec3 tmp2 = test * tmp;

		mdltransform.pd[i] = glm::vec3{tmp2.x, tmp2.y, 0};
	}

}


/*  _________________________________________________________________________*/
/*! GLPbo::set_pixel

@param GLint x
X value of pixel to be set

@param GLint y
Y value of pixel to be set

@param GLPbo::Color draw_clr
Color of line to be drawn

@return none

This function is called to set pixels
*/
void GLPbo::set_pixel(int x, int y, GLPbo::Color clr)
{
	if (x < 0 || x >= GLHelper::width || y < 0 || y >= GLHelper::height)
	{
		return;
	}
	std::fill_n(ptr_to_pbo + y * GLHelper::width + x, 1, clr);
}
/*  _________________________________________________________________________*/
/*! GLPbo::set_clear_color

@param GLPbo::Color color
Input color to be set

@return none

This function is called to set the clear color
*/
void GLPbo::set_clear_color(GLPbo::Color color)
{
	GLPbo::clear_clr = color;
}

/*  _________________________________________________________________________*/
/*! GLPbo::set_clear_color

@param GLubyte r
Red color to be set

@param GLubyte g
Green color to be set

@param GLubyte b
Blue color to be set

@param GLubyte a
Alpha color to be set

@return none

This function is called to set the clear color
*/
void GLPbo::set_clear_color(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
	GLPbo::clear_clr = { r,g,b,a };
}

/*  _________________________________________________________________________*/
/*! GLPbo::clear_color_buffer

@param none

@return none

This function is called to clear the color buffer
*/
void GLPbo::clear_color_buffer()
{
	std::fill_n(GLPbo::ptr_to_pbo, width * height, GLPbo::clear_clr);
}

/*  _________________________________________________________________________*/
/*! GLPbo::setup_quad_vao

@param none

@return none

This function is called to setup the quad vao
*/
void GLPbo::setup_quad_vao()
{
	std::vector<glm::vec2> pos_vtx
	{
		glm::vec2(1.f, -1.f), glm::vec2(1.f, 1.f),
		glm::vec2(-1.f, 1.f), glm::vec2(-1.f, -1.f)
	};
	std::vector<glm::vec2> tex_coord
	{
		glm::vec2(1.f, 0.f), glm::vec2(1.f, 1.f),
		glm::vec2(0.f, 1.f), glm::vec2(0.f, 0.f)
	};
	// compute and store endpoints for (slices+1) set of lines
	// for each x from -1 to 1
	// start endpoint is (x, -1) and end endpoint is (x, 1)
	// set up VAO as in GLApp::points_model
	GLuint vbo_hdl;
	glCreateBuffers(1, &vbo_hdl);
	glNamedBufferStorage(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size() + sizeof(glm::vec2) * tex_coord.size(),
		nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData(vbo_hdl, 0, sizeof(glm::vec2) * pos_vtx.size(), pos_vtx.data());
	glNamedBufferSubData(vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec2) * tex_coord.size(), tex_coord.data());

	glCreateVertexArrays(1, &vaoid);
	glEnableVertexArrayAttrib(vaoid, 0); // Attrib 0
	glVertexArrayVertexBuffer(vaoid, 0, vbo_hdl, 0, sizeof(glm::vec2)); // buffer binding point 0
	glVertexArrayAttribFormat(vaoid, 0, 2, GL_FLOAT, GL_FALSE, 0); // attrib index and size
	glVertexArrayAttribBinding(vaoid, 0, 0); // Attrib index, buffer binding point

	glEnableVertexArrayAttrib(vaoid, 1); // Attrib 1
	glVertexArrayVertexBuffer(vaoid, 1, vbo_hdl, sizeof(glm::vec2) * pos_vtx.size(), sizeof(glm::vec2)); // buffer binding point 1
	glVertexArrayAttribFormat(vaoid, 1, 2, GL_FLOAT, GL_FALSE, 0); // attrib index and size
	glVertexArrayAttribBinding(vaoid, 1, 1); // Attrib index, buffer binding point


	std::vector<GLushort> idx_vtx{
		0, 1, 2, // 1st triangle with counterclockwise winding is specified by
		// vertices in VBOs with indices 0, 1, 2
		2, 3, 0 // 2nd triangle with counterclockwise winding
	};

	GLuint ebo_hdl;
	glCreateBuffers(1, &ebo_hdl);
	glNamedBufferStorage(ebo_hdl, sizeof(GLushort) * idx_vtx.size(), reinterpret_cast<GLvoid*>(idx_vtx.data()), GL_DYNAMIC_STORAGE_BIT);
	glVertexArrayElementBuffer(vaoid, ebo_hdl);
	glBindVertexArray(0);

	GLPbo::elem_cnt = idx_vtx.size();
}

/*  _________________________________________________________________________*/
/*! GLPbo::setup_shdrpgm

@param none

@return none

This function is called to setup the shdrpgm
*/
void GLPbo::setup_shdrpgm()
{
	std::string vtx_shdr
	{
		"#version 450 core\n"
		"layout(location = 0) in vec2 aVertexPosition;\n"
		"layout(location = 1) in vec2 aTextureCoord;\n"
		"layout(location = 0) out vec2 vTextureCoord;\n"
		"void main() {\n"
		"	gl_Position = vec4(aVertexPosition, 0.0, 1.0);\n"
		"	vTextureCoord = aTextureCoord;\n"
		"}\n"
	};
	std::string frg_shdr
	{
		"#version 450 core\n"
		"layout (location=0) in vec2 vTexturecoord;\n"
		"layout (location=0) out vec4 fFragColor;\n"
		"uniform sampler2D ourTexture;\n"
		"void main() {\n"
		"	fFragColor = texture(ourTexture, vTexturecoord);\n"
		"}\n"
	};	
	std::vector<std::pair<GLenum, std::string>> shdr_files;
	shdr_files.emplace_back(std::make_pair(GL_VERTEX_SHADER, vtx_shdr));
	shdr_files.emplace_back(std::make_pair(GL_FRAGMENT_SHADER, frg_shdr));
	for (auto& elem : shdr_files) {
		if (GL_FALSE == shdr_pgm.CompileShaderFromString(elem.first, elem.second)) {
			std::cout << "Unable to compile/link/validate shader programs\n";
			std::cout << shdr_pgm.GetLog() << "\n";
			std::exit(EXIT_FAILURE);
		}
	}
	if (GL_FALSE == shdr_pgm.Link()) {
		std::cout << "Unable to compile/link/validate shader programs\n";
	}
	if (GL_FALSE == shdr_pgm.Validate()) {
		std::cout << "Unable to compile/link/validate shader programs\n";
	}
	shdr_pgm.PrintActiveAttribs();
	shdr_pgm.PrintActiveUniforms();
}

/*  _________________________________________________________________________*/
/*! GLPbo::emulate

@param none

@return none

This function is called to emulate the program
*/
void GLPbo::emulate()
{
	set_clear_color(210, 210, 210);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboid);

	GLPbo::ptr_to_pbo = reinterpret_cast<GLPbo::Color*>(glMapNamedBuffer(pboid, GL_WRITE_ONLY));
	clear_color_buffer();

	if (GLHelper::keystateM == GL_TRUE)
	{
		GLHelper::keystateM = GL_FALSE;
		rotflag = false;
		modelnum >= 4 ? modelnum = 0 : modelnum++;
	}
	if (GLHelper::keystateR == GL_TRUE)
	{
		GLHelper::keystateR = GL_FALSE;
		rotflag = !rotflag;
	}
	if (rotflag)
	{
		modelstorage[modelnum].second.angle += (float)(GLHelper::delta_time * 30.0);
	}
	if (GLHelper::keystateW == GL_TRUE)
	{
		GLHelper::keystateW = GL_FALSE;
		modelstorage[modelnum].second.rendertype >= 3 ? modelstorage[modelnum].second.rendertype = 0 : modelstorage[modelnum].second.rendertype++;

	}

	viewport_transformation(GLPbo::modelstorage[modelnum].second);
	
	colorcounter = 0;

	modelstorage[modelnum].second.culled = 0;

	for (size_t i = 0; i < modelstorage[modelnum].second.tri.size(); i += 3)
	{
		if (modelstorage[modelnum].second.signedarea[colorcounter] > 0.f)
		{
			switch (modelstorage[modelnum].second.rendertype)
			{
			case 0:
				GLPbo::render_wireframe(modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i]],
					modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i + 1]],
					modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i + 2]],
					{ 0, 0, 0, 255 });
				break;
			case 1:
				GLPbo::render_wireframe(modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i]],
					modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i + 1]],
					modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i + 2]],
					{ GLubyte(modelstorage[modelnum].second.colors[modelstorage[modelnum].second.tri[colorcounter]].x), GLubyte(modelstorage[modelnum].second.colors[modelstorage[modelnum].second.tri[colorcounter]].y),GLubyte(modelstorage[modelnum].second.colors[modelstorage[modelnum].second.tri[colorcounter]].z), 255 });
				break;
			case 2:
				GLPbo::render_triangle(modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i]],
					modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i + 1]],
					modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i + 2]],
					{ GLubyte(modelstorage[modelnum].second.colors[modelstorage[modelnum].second.tri[colorcounter]].x), GLubyte(modelstorage[modelnum].second.colors[modelstorage[modelnum].second.tri[colorcounter]].y),GLubyte(modelstorage[modelnum].second.colors[modelstorage[modelnum].second.tri[colorcounter]].z), 255 });
				break;
			case 3:
				GLPbo::render_triangle(modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i]],
					modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i + 1]],
					modelstorage[modelnum].second.pd[modelstorage[modelnum].second.tri[i + 2]],
					modelstorage[modelnum].second.nml[modelstorage[modelnum].second.tri[i]],
					modelstorage[modelnum].second.nml[modelstorage[modelnum].second.tri[i + 1]],
					modelstorage[modelnum].second.nml[modelstorage[modelnum].second.tri[i + 2]]);
				break;
			}
		}
		else
		{
			modelstorage[modelnum].second.culled++;
		}
		colorcounter++;
	}

	glUnmapNamedBuffer(pboid);

	
	glBindTexture(GL_TEXTURE_2D, texid);

	glTextureSubImage2D(texid, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

/*  _________________________________________________________________________*/
/*! GLPbo::render_linebresenham

@param GLint px0
X value of starting point

@param GLint py0
Y value of starting point

@param GLint px1
X value of ending point

@param GLint py1
Y value of ending point

@param GLPbo::Color draw_clr
Color of line to be drawn

@return none

This function is called to render the line bresenham
*/
void GLPbo::render_linebresenham(GLint px0, GLint py0,
	GLint px1, GLint py1, GLPbo::Color draw_clr)
{
	int x1 = px0;
	int y1 = py0;
	int dx = px1 - x1, dy = py1 - y1;
	int xstep = (dx < 0) ? -1 : 1;
	int ystep = (dy < 0) ? -1 : 1;
	dx = (dx < 0) ? -dx : dx;
	dy = (dy < 0) ? -dy : dy;

	if (dy <= dx)
	{
		int d = 2 * dy - dx, dmin = 2 * dy, dmaj = 2 * dy - 2 * dx;
		set_pixel(x1, y1, draw_clr);
		while (--dx > 0)
		{
			y1 += (d > 0) ? ystep : 0;
			d += (d > 0) ? dmaj : dmin;
			x1 += xstep;
			set_pixel(x1, y1, draw_clr);
		}
	}
	else
	{
		int d = 2 * dx - dy, dmin = 2 * dx, dmaj = 2 * dx - 2 * dy;
		set_pixel(x1, y1, draw_clr);
		while (--dy > 0)
		{
			x1 += (d > 0) ? xstep : 0;
			d += (d > 0) ? dmaj : dmin;
			y1 += ystep;
			set_pixel(x1, y1, draw_clr);
		}
	}
}

/*  _________________________________________________________________________*/
/*! GLPbo::render_wireframe

@param glm::vec3 p0
Point 0 of triangle wireframe

@param glm::vec3 p1
Point 1 of triangle wireframe

@param glm::vec3 p2
Point 2 of triangle wireframe

@param GLPbo::Color clr
Color of lines to be drawn

@return none

This function is called to render the line bresenham as a triangle wireframe
*/
void GLPbo::render_wireframe(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, GLPbo::Color clr)
{

		render_linebresenham
		(
			(GLint)p0.x,
			(GLint)p0.y,
			(GLint)p1.x,
			(GLint)p1.y,
			clr
		);
		render_linebresenham
		(
			(GLint)p1.x,
			(GLint)p1.y,
			(GLint)p2.x,
			(GLint)p2.y,
			clr
		);
		render_linebresenham
		(
			(GLint)p0.x,
			(GLint)p0.y,
			(GLint)p2.x,
			(GLint)p2.y,
			clr
		);
	
}

/*  _________________________________________________________________________*/
/*! GLPbo::render_triangle

@param glm::vec3 p0
Point 0 of triangle

@param glm::vec3 p1
Point 1 of triangle

@param glm::vec3 p2
Point 2 of triangle

@param GLPbo::Color clr
Color of lines to be drawn

@return none

This function is called to render a flat shaded triangle
*/
bool GLPbo::render_triangle(glm::vec3 const& p0, glm::vec3 const& p1,
	glm::vec3 const& p2, GLPbo::Color clr)
{

	glm::vec4 L0 = EdgeEquationTopLeft(p1, p2);
	glm::vec4 L1 = EdgeEquationTopLeft(p2, p0);
	glm::vec4 L2 = EdgeEquationTopLeft(p0, p1);

	glm::vec2 min{ (int)(std::min({ p0.x, p1.x, p2.x })), (int)(std::min({ p0.y, p1.y, p2.y })) };
	glm::vec2 max{ (std::max({ p0.x, p1.x, p2.x })), (std::max({ p0.y, p1.y, p2.y })) };


	for (int y = (int)min.y; y <= (int)max.y; y++)
	{
		for (int x = (int)min.x; x <= (int)max.x; x++)
		{
			if (PointInEdgeTopLeftOptimized(L0, { (float)x + 0.5f, (float)y + 0.5f, 0 }) && 
				PointInEdgeTopLeftOptimized(L1, { (float)x + 0.5f, (float)y + 0.5f, 0 }) &&
				PointInEdgeTopLeftOptimized(L2, { (float)x + 0.5f, (float)y + 0.5f, 0 }))
			{
				set_pixel(x, y, clr);
			}
		}
	}
	return true;
}

/*  _________________________________________________________________________*/
/*! GLPbo::render_triangle

@param glm::vec3 p0
Point 0 of triangle

@param glm::vec3 p1
Point 1 of triangle

@param glm::vec3 p2
Point 2 of triangle

@param glm::vec3 c0
Color of Point 0

@param glm::vec3 c1
Color of Point 1

@param glm::vec3 c2
Color of Point 2

@return none

This function is called to render a smooth shaded triangle
*/
bool GLPbo::render_triangle(glm::vec3 const& p0, glm::vec3 const& p1,
	glm::vec3 const& p2, glm::vec3 const& c0,
	glm::vec3 const& c1, glm::vec3 const& c2)
{

	glm::vec4 L0 = EdgeEquationTopLeft(p1, p2);
	glm::vec4 L1 = EdgeEquationTopLeft(p2, p0);
	glm::vec4 L2 = EdgeEquationTopLeft(p0, p1);

	glm::vec2 min{ (int)(std::min({ p0.x, p1.x, p2.x })), (int)(std::min({ p0.y, p1.y, p2.y })) };
	glm::vec2 max{ (std::max({ p0.x, p1.x, p2.x })), (std::max({ p0.y, p1.y, p2.y })) };


	for (int y = (int)min.y; y <= (int)max.y; y++)
	{
		for (int x = (int)min.x; x <= (int)max.x; x++)
		{
			if (PointInEdgeTopLeftOptimized(L0, { (float)x + 0.5f, (float)y + 0.5f, 0 }) &&
				PointInEdgeTopLeftOptimized(L1, { (float)x + 0.5f, (float)y + 0.5f, 0 }) &&
				PointInEdgeTopLeftOptimized(L2, { (float)x + 0.5f, (float)y + 0.5f, 0 }))
			{
				glm::vec3 clr{ (L0.w * c0.x + L1.w * c1.x + L2.w * c2.x) , (L0.w * c0.y + L1.w * c1.y + L2.w * c2.y), (L0.w * c0.z + L1.w * c1.z + L2.w * c2.z) };
				Color combinedClr{ static_cast<GLubyte>(clr.x * 255 / modelstorage[modelnum].second.signedarea[colorcounter]), static_cast<GLubyte>(clr.y * 255 / modelstorage[modelnum].second.signedarea[colorcounter]), static_cast<GLubyte>(clr.z * 255 / modelstorage[modelnum].second.signedarea[colorcounter]), 255 };
				set_pixel(x, y, combinedClr);
			}
		}
	}
	return true;
}

/*  _________________________________________________________________________*/
/*! GLPbo::render_triangle

@param glm::vec3 p0
Point 0 of triangle

@param glm::vec3 p1
Point 1 of triangle

@return glm::vec4
Vector 4 of edge equation

This function computes the edge equation given points
*/
glm::vec4 GLPbo::EdgeEquationTopLeft(glm::vec3 const& p0, glm::vec3 const& p1)
{
	return { p0.y - p1.y, p1.x - p0.x, (p1.y - p0.y) * p0.x - (p1.x - p0.x) * p0.y, 0 };
}

/*  _________________________________________________________________________*/
/*! GLPbo::render_triangle

@param glm::vec3 E
Edge equation

@param glm::vec3 P
Arbitrary point

@return bool

This function checks if point is in within boundaries
*/
bool GLPbo::PointInEdgeTopLeftOptimized(glm::vec4& E, glm::vec3 P)
{
	E.w = E.x * P.x + E.y * P.y + E.z;

	return (E.w > -0.005f || (E.w == 0.f && ((E.x != 0.f) ? (E.x > 0.f) : (E.y < 0.f)))) ? true : false;
}
/*  _________________________________________________________________________*/
/*! GLPbo::draw_fullwindow_quad

@param none

@return none

This function is called to draw the window
*/
void GLPbo::draw_fullwindow_quad()
{
	glBindTextureUnit(6, texid);

	// use shader
	shdr_pgm.Use();

	// set texture location
	GLuint tex_loc = glGetUniformLocation(shdr_pgm.GetHandle(), "ourTexture");

	glUniform1i(tex_loc, 6);

	// GLPbo::vaoid must be initialized with vertex buffers containing 2D position and texture coordinates for a full - window quad
	glBindVertexArray(vaoid);

	// draw
	glDrawElements(GL_TRIANGLE_STRIP, elem_cnt, GL_UNSIGNED_SHORT, nullptr);

	std::stringstream title;
	title << std::fixed;
	title << std::setprecision(2);
	title << "A1 | Ang Wei Ren ";
	title << " | Model: ";
	title << modelstorage[modelnum].second.name;
	title << " | Mode: ";
	switch (modelstorage[modelnum].second.rendertype)
	{
	case 0:
		title << "Wireframe";
		break;
	case 1:
		title << "Wireframe Color";
		break;
	case 2:
		title << "Faceted";
		break;
	case 3:
		title << "Shaded";
		break;
	}
	title << " | Vtx: " << modelstorage[modelnum].second.pm.size();
	title << " | Tri: " << modelstorage[modelnum].second.tri.size() / 3;
	title << " | Culled: " << modelstorage[modelnum].second.culled;
	title << std::setprecision(2) << " | FPS " << int(GLHelper::fps * 100) / 100.0;

	glfwSetWindowTitle(GLHelper::ptr_window, title.str().c_str());
}

/*  _________________________________________________________________________*/
/*! GLPbo::cleanup

@param none

@return none

This function is called to cleanup the created objects	
*/
void GLPbo::cleanup()
{
	glDeleteVertexArrays(1, &vaoid);
	glDeleteBuffers(1, &pboid);
	glDeleteTextures(1, &texid);
}