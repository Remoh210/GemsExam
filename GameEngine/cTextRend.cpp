#include "cTextRend.h"





cTextRend::cTextRend()
{

	// Vertex shader text
	mvs_text =
		"#version 400\n"
		"attribute vec4 coord;"
		"varying vec2 texpos;"
		"void main () {"
		"	gl_Position = vec4(coord.xy, 0, 1);"
		"	texpos = coord.zw;"
		"}";

	// Fragmente shader text
	mfs_text =
		"#version 400\n"
		"varying vec2 texpos;"
		"uniform sampler2D tex;"
		"uniform vec4 color;"
		"void main () {"
		"	gl_FragColor = vec4(1, 1, 1, texture2D(tex, texpos).r) * color;"
		"}";


	init();
	mStart = true;
	mTimeWaitedSoFar = 0.0f;
}

GLboolean cTextRend::initfreetype() {

	if (FT_Init_FreeType(&mft))
	{
		fprintf(stderr, "unable to init free type\n");
		return GL_FALSE;
	}

	if (FT_New_Face(mft, "assets/fonts/ARIALUNI.TTF", 0, &mface))
	{
		fprintf(stderr, "unable to open font\n");
		return GL_FALSE;
	}

	//set font size
	FT_Set_Pixel_Sizes(mface, 0, 48);


	if (FT_Load_Char(mface, 'X', FT_LOAD_RENDER))
	{
		fprintf(stderr, "unable to load character\n");
		return GL_FALSE;
	}


	return GL_TRUE;
}

GLboolean cTextRend::init_gl()
{
	//create shaders
	mvertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(mvertex_shader, 1, &mvs_text, NULL);
	glCompileShader(mvertex_shader);

	mfragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(mfragment_shader, 1, &mfs_text, NULL);
	glCompileShader(mfragment_shader);

	mprogramm = glCreateProgram();
	glAttachShader(mprogramm, mvertex_shader);
	glAttachShader(mprogramm, mfragment_shader);

	glLinkProgram(mprogramm);

	//get vertex attribute/s id/s
	attribute_coord = glGetAttribLocation(mprogramm, "coord");
	uniform_tex = glGetUniformLocation(mprogramm, "tex");
	uniform_color = glGetUniformLocation(mprogramm, "color");

	if (attribute_coord == -1 || uniform_tex == -1 || uniform_color == -1)
	{
		fprintf(stderr, "unable to get attribute or uniform from shader\n");
		return GL_FALSE;
	}

	//generate and bind vbo 
	glGenBuffers(1, &mdp_vbo);

	//generate and bind vao
	glGenVertexArrays(1, &mvao);

	return GL_TRUE;
}

bool cTextRend::init()
{
	//init_gl();
	//initfreetype();
	if (init_gl() != GL_TRUE)
		return false;

	if (initfreetype() != GL_TRUE)
		return false;

	return true;
}

void cTextRend::drawText(unsigned int width, unsigned int height, const char *text)
{


	GLfloat white[4] = { 1, 1, 1, 1 };
	//GLfloat red[4] = { 1, 0, 0, 1 };

	glUseProgram(mprogramm);


	glUniform4fv(uniform_color, 1, white);


	glBindVertexArray(mvao);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	float sx = 2.0f / width;
	float sy = 2.0f / height;
	GLfloat yoffset = 50.0f;
	GLfloat xoffset = 8 * sx;

	renderText(text, -1 + xoffset, 1 - yoffset * sy, sx, sy);

	glBindVertexArray(0);

}



void cTextRend::drawText(unsigned int width, unsigned int height, const char *text, float yoffset)
{


	//GLfloat white[4] = { 1, 1, 1, 1 };
	GLfloat color[4] = { 1, 0, 0, 1 };

	glUseProgram(mprogramm);


	glUniform4fv(uniform_color, 1, color);


	glBindVertexArray(mvao);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	float sx = 2.0f / width;
	float sy = 2.0f / height;
	//GLfloat yoffset = 50.0f;
	GLfloat xoffset = 8 * sx;

	renderText(text, -1 + xoffset, 1 - yoffset * sy, sx, sy);

	glBindVertexArray(0);

}



//void cTextRend::drawText(unsigned int width, unsigned int height, const char *text, float drawTime, float dt)
//{
//
//
//
//		if (this->mStart) 
//		{
//			mTimeWaitedSoFar = 0.0f;
//			mStart = false;
//		}
//		//mAgent->velocity = glm::vec3(0.0f);
//		//mAgent->accel = glm::vec3(0.0f);
//		mTimeWaitedSoFar += (float)dt;
//		if (mTimeWaitedSoFar < drawTime)
//		{
//			//draw
//			GLfloat black[4] = { 1, 1, 1, 1 };
//			GLfloat red[4] = { 1, 0, 0, 1 };
//
//			glUseProgram(mprogramm);
//
//
//			glUniform4fv(uniform_color, 1, black);
//
//
//			glBindVertexArray(mvao);
//
//			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//			glDisable(GL_CULL_FACE);
//			float sx = 2.0f / width;
//			float sy = 2.0f / height;
//			GLfloat yoffset = 50.0f;
//			GLfloat xoffset = 8 * sx;
//
//			renderText(text, -1 + xoffset, 1 - yoffset * sy, sx, sy);
//
//			glBindVertexArray(0);
//		}
//	
//}

void cTextRend::renderText(const char *text, float x, float y, float sx, float sy) {

	const char *p;
	FT_GlyphSlot g = mface->glyph;

	GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniform1i(uniform_tex, 0);

	/* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/* Set up the VBO for our vertex data */
	glEnableVertexAttribArray(attribute_coord);
	glBindBuffer(GL_ARRAY_BUFFER, mdp_vbo);
	glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);


	/* Loop through all characters */
	for (p = text; *p; p++) {
		/* Try to load and render the character */
		if (FT_Load_Char(mface, *p, FT_LOAD_RENDER))
			continue;

		/* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, g->bitmap.width, g->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			g->bitmap.width,
			g->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			g->bitmap.buffer
		);

		/* Calculate the vertex and texture coordinates */
		float x2 = x + g->bitmap_left * sx;
		float y2 = -y - g->bitmap_top * sy;
		float w = g->bitmap.width * sx;
		float h = g->bitmap.rows * sy;

		point box[4] = {
			{ x2, -y2, 0, 0 },
		{ x2 + w, -y2, 1, 0 },
		{ x2, -y2 - h, 0, 1 },
		{ x2 + w, -y2 - h, 1, 1 },
		};

		//glBindVertexArray(mvao);
		/* Draw the character on the screen */
		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		/* Advance the cursor to the start of the next character */
		x += (g->advance.x >> 6) * sx;
		y += (g->advance.y >> 6) * sy;
	}

	glDisableVertexAttribArray(attribute_coord);
	glDeleteTextures(1, &tex);


}