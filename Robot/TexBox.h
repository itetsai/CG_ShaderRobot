#pragma once
#include "vgl.h"

class TexBox
{
protected:
	static GLuint Program;

	GLuint  m_TextureIndex;
	GLuint	m_Buffer;
	GLuint  m_VAO;

	virtual void Initialize(unsigned int size, const GLfloat* data);
public:
	TexBox(void) {};
	TexBox(float x, float y, float w, float h, bool mirror = true);
	~TexBox(void);

	static void inline LoadProgram(GLuint p)
	{
		Program = p;
	};

	static void inline UnloadProgram() {
#ifndef _UNLOADED_SHADER_TEXBOX
#define _UNlOADED_SHADER_TEXTBOX
		glDeleteShader(Program);
#endif
	};

	static void inline BeginProgram() { glUseProgram(Program); };
	static void inline EndProgram() {};

	virtual void Render();

	void SetTexture(GLuint index) { m_TextureIndex = index; }
	void ReleaseTexture();
};

