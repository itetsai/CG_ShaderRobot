#include "TexBox.h"

GLuint TexBox::Program = 0;
TexBox::TexBox(float x, float y, float w, float h, bool mirror /*= true*/)
{
	if (mirror)
	{
		const GLfloat data[] =
		{
			x + w	, y,
			x	, y,
			x	, y + h,
			x + w	, y + h,

			1.0f, 0.0f,
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f
		};
		Initialize(sizeof(data), data);
	}
	else {
		const GLfloat data[] =
		{
			x + w	, y,
			x	, y,
			x	, y + h,
			x + w	, y + h,

			1.0f, 1.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f
		};
		Initialize(sizeof(data), data);
	}
}

TexBox::~TexBox(void)
{
	ReleaseTexture();
}

void TexBox::Initialize(unsigned int size, const GLfloat* data)
{
	glGenBuffers(1, &m_Buffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffer);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glUseProgram(Program);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(8 * sizeof(float)));

	glEnableVertexAttribArray(3);
	glEnableVertexAttribArray(4);

	glLinkProgram(Program);
	glUniform1i(glGetUniformLocation(Program, "tex"), 0);
}

void TexBox::Render()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureIndex);
	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void TexBox::ReleaseTexture()
{
	glDeleteTextures(1, &m_TextureIndex);
}




