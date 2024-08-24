#pragma once

#include <GL/glew.h>

bool UCreateTexture(const char* filename, GLuint& textureId);
void UDestroyTexture(GLuint textureId);