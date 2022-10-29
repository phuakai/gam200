#include "Font.h"

namespace Font
{
	std::map<char, Character> Characters;
	unsigned int VAO, VBO;
	unsigned int Uniform;

	void init()
	{
		FT_Library library;
		FT_Face face;
		FT_Error ft_error_code;

		ft_error_code = FT_Init_FreeType(&library);
		if (ft_error_code)
		{
			assert("An error occurred during library initialization");
		}

		else if (ft_error_code)
		{
			assert("Another error code means that the font file could not be opened or read, or that it is broken.");
		}

		/*ft_error_code = FT_Load_Char(face, 'X', FT_LOAD_RENDER);
		if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
		{
			assert("Failed to load glyph");
		}*/

		ft_error_code = FT_New_Face(library, "../asset/fonts/arial.ttf", 0, &face);
		if (ft_error_code == FT_Err_Unknown_File_Format)
		{
			assert("The font file could be opened and read, but it appears the font file could be opened and read, but it appears");
		}

		else
		{
			FT_Set_Pixel_Sizes(face, 0, 48); // Disable byte-alignment restriction 
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			for (unsigned char c = 0; c < 128; c++)
			{
				// load character glyph
				if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				{
					std::cout << "Error::Freetype: Failed to load glyph";
					continue;
				}
				// generate texture
				unsigned int texture;
				glGenTextures(1, &texture);
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

				// set texture options
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				// now store character for later use
				Character character = {
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<unsigned int>(face->glyph->advance.x)
				};
				Characters.insert(std::pair<char, Character>(c, character));
				//std::cout << character.TextureID << std::endl;
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		}
		FT_Done_Face(face);
		FT_Done_FreeType(library);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	void RenderFont(GLSLShader& s, std::string text, float x, float y, float scale, glm::vec3 color)
	{
		glEnable(GL_BLEND);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		s.Use();
		//Uniform = glGetUniformLocation(s.GetHandle(), "text");
		glm::mat4x4 proj(
			2.0f / (float)Graphics::camera2d.getWinWidth(), 0.f, 0.f, 0.f,
			0.f, 2.0f / (float)Graphics::camera2d.getWinHeight(), 0.f, 0.f,
			0.f, 0.f, -1.f, 0.f,
			-2.f, -2.f, 0.f, 1.f);
		glUniformMatrix4fv(glGetUniformLocation(s.GetHandle(), "projection"), 1, GL_FALSE, glm::value_ptr(proj));

		glUniform3f(glGetUniformLocation(s.GetHandle(), "textColor"), color.x, color.y, color.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(VAO);

		// Iterate through the characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];

			float xpos = x + ch.Bearing.x * scale;
			float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

			float width = ch.Size.x * scale;
			float height = ch.Size.y * scale;

			float vertices[6][4] = {
				{xpos,  ypos + height,			0.0f, 0.0f, },
				{xpos,  ypos,					0.0f, 1.0f, },
				{xpos + width, ypos,			1.0f, 1.0f, },
				{xpos,  ypos + height,			0.0f, 0.0f, },
				{xpos + width,  ypos,			1.0f, 1.0f, },
				{xpos + width, ypos + height,   1.0f, 0.0f, }
			};

			glBindTexture(GL_TEXTURE_2D, ch.TextureID);
			//glUniform1i(Uniform, 0);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glDrawArrays(GL_TRIANGLES, 0, 6);

			x += (ch.Advance >> 6) * scale;

			//std::cout << ch.TextureID << std::endl;
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
		//std::cout << text << " been rendered!" << std::endl;
		//std::cout << "Text Coords, x: " << x << " y: " << y << std::endl;
	}
}
