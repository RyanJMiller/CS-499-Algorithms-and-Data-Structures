#pragma once

#include <GL/glew.h>  // Include OpenGL types

// Struct to hold mesh data
struct GLMesh {
    GLuint vao;
    GLuint vbos[2];
    GLuint nIndices;
};

// Function declarations
void UCreateCylinder(GLMesh& mesh);
void UCreateCube(GLMesh& mesh);
void UCreateSphere(GLMesh& mesh);
void UCreatePlane(GLMesh& mesh);
void UDestroyMesh(GLMesh& mesh);