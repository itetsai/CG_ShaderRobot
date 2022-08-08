#pragma once
#pragma once
#include "vgl.h"

#include <string.h>
#include <vector>

#include <opencv\cv.h>
#include <opencv\highgui.h>

namespace TextureApp {
	bool	LoadPngImage(char *name, int &outWidth, int &outHeight, bool &outHasAlpha, GLubyte **outData);
	GLubyte *LoadImage(char *name, int &outWidth, int &outHeight);
	GLubyte *LoadAlpha(char *name, int &outWidth, int &outHeight);
	GLuint	GenTexture(char* filepath);
	void OutputScreen(std::string& file);
}