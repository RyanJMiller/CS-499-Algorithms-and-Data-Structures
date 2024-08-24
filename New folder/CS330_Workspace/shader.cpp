#include "shader.h"
#include <iostream>
using namespace std;

/**
 * @brief Creates a shader program from vertex and fragment shader source code.
 *
 * This function compiles the vertex and fragment shaders from the provided source code,
 * attaches them to a shader program, and links the program. It also checks for any
 * compilation or linkage errors and reports them.
 *
 * @param vtxShaderSource The source code of the vertex shader.
 * @param fragShaderSource The source code of the fragment shader.
 * @param programId A reference to the shader program ID that will be created.
 * @return True if the shader program is successfully created, otherwise false.
 */
bool UCreateShaderProgram(const char* vtxShaderSource, const char* fragShaderSource, GLuint& programId)
{
    // Linkage error reporting
    int success = 0;
    char infoLog[512];

    // Creates a shader program object.
    programId = glCreateProgram();

    // Creates the vertex and fragment shader objects
    GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    // Gets source code for both shaders
    glShaderSource(vertexShaderId, 1, &vtxShaderSource, NULL);
    glShaderSource(fragmentShaderId, 1, &fragShaderSource, NULL);

    // Compiles the vertex shader and checks for errors
    glCompileShader(vertexShaderId); // compile the vertex shader
    glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &success);


    if (!success)
    {
        glGetShaderInfoLog(vertexShaderId, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;

        return false;
    }

    // Compiles the fragment shader and checks for errors
    glCompileShader(fragmentShaderId);
    glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderId, sizeof(infoLog), NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;

        return false;
    }

    // Attaches compiled shaders to the program
    glAttachShader(programId, vertexShaderId);
    glAttachShader(programId, fragmentShaderId);

    // Links the shader program and checks for errors
    glLinkProgram(programId);
    glGetProgramiv(programId, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(programId, sizeof(infoLog), NULL, infoLog);
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;

        return false;
    }

    // Uses the shader program
    glUseProgram(programId);

    return true;
}

/**
 * @brief Deletes the shader program when it is no longer needed.
 *
 * This function deletes the specified shader program, freeing up the resources
 * associated with it. It should be called when the shader program is no longer needed.
 *
 * @param programId The ID of the shader program to be deleted.
 */
void UDestroyShaderProgram(GLuint programId)
{
    glDeleteProgram(programId);
}