#include "mesh.h"
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/constants.hpp>
using namespace std;

/**
 * @brief Creates a 3D cylinder mesh.
 *
 * This function generates the vertices, normals, texture coordinates, and indices
 * for a 3D cylinder mesh and uploads them to the GPU. The cylinder is centered at
 * the origin, with its height extending along the y-axis.
 *
 * @param mesh The GLMesh structure to hold the mesh data.
 */
void UCreateCylinder(GLMesh& mesh) {
    const int segments = 36;
    const float radius = 1.0f;
    const float height = 2.0f;
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;

    float angleStep = 2.0f * glm::pi<float>() / segments;

    // Generate vertices and normals
    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        float u = (float)i / segments;

        // Top vertex
        vertices.push_back(x);                // x
        vertices.push_back(height / 2.0f);    // y
        vertices.push_back(z);                // z
        vertices.push_back(x);                // normal x (for smooth shading, use x and z)
        vertices.push_back(0.0f);             // normal y
        vertices.push_back(z);                // normal z
        vertices.push_back(u);                // texture u
        vertices.push_back(1.0f);             // texture v

        // Bottom vertex
        vertices.push_back(x);                // x
        vertices.push_back(-height / 2.0f);   // y
        vertices.push_back(z);                // z
        vertices.push_back(x);                // normal x (for smooth shading, use x and z)
        vertices.push_back(0.0f);             // normal y
        vertices.push_back(z);                // normal z
        vertices.push_back(u);                // texture u
        vertices.push_back(0.0f);             // texture v
    }

    // Center top vertex (for caps)
    vertices.push_back(0.0f);                // x
    vertices.push_back(height / 2.0f);       // y
    vertices.push_back(0.0f);                // z
    vertices.push_back(0.0f);                // normal x
    vertices.push_back(1.0f);                // normal y
    vertices.push_back(0.0f);                // normal z
    vertices.push_back(0.5f);                // texture u
    vertices.push_back(0.5f);                // texture v

    // Center bottom vertex (for caps)
    vertices.push_back(0.0f);                // x
    vertices.push_back(-height / 2.0f);      // y
    vertices.push_back(0.0f);                // z
    vertices.push_back(0.0f);                // normal x
    vertices.push_back(-1.0f);               // normal y
    vertices.push_back(0.0f);                // normal z
    vertices.push_back(0.5f);                // texture u
    vertices.push_back(0.5f);                // texture v

    // Generate indices for the sides
    for (int i = 0; i < segments; ++i) {
        int top1 = i * 2;
        int top2 = (i + 1) * 2;
        int bottom1 = i * 2 + 1;
        int bottom2 = (i + 1) * 2 + 1;

        // Side triangles
        indices.push_back(top1);
        indices.push_back(bottom1);
        indices.push_back(bottom2);

        indices.push_back(top1);
        indices.push_back(bottom2);
        indices.push_back(top2);
    }

    // Generate indices for the top and bottom circles
    int centerTopIndex = (segments + 1) * 2;
    int centerBottomIndex = centerTopIndex + 1;
    for (int i = 0; i < segments; ++i) {
        int top1 = i * 2;
        int top2 = (i + 1) * 2;
        int bottom1 = i * 2 + 1;
        int bottom2 = (i + 1) * 2 + 1;

        // Top circle
        indices.push_back(top1);
        indices.push_back(top2);
        indices.push_back(centerTopIndex);

        // Bottom circle
        indices.push_back(bottom1);
        indices.push_back(centerBottomIndex);
        indices.push_back(bottom2);
    }

    // sets the number of positional, normal, and texture coordinate data points
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // creates 2 buffers: first one for vertex data; second one for indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // 8 strides between vertex coordinates (x, y, z, nx, ny, nz, u, v)
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

    // creates vertex attribute pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

/**
 * @brief Creates a 3D cube mesh.
 *
 * This function generates the vertices, normals, and texture coordinates for a 3D cube mesh
 * and uploads them to the GPU. The cube is centered at the origin.
 *
 * @param mesh The GLMesh structure to hold the mesh data.
 */
void UCreateCube(GLMesh& mesh)
{
    // Vertex data for a cube, including positions, normals, and texture coordinates
    GLfloat verts[] = {
        // vertex positions  // normals        // texture coordinates
         0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f, // front right (bottom face) Vertex 0
        -0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f,  0.0f, 1.0f, // front left (bottom face) Vertex 1
        -0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, // back left (bottom face) Vertex 3
         0.5f, -0.5f, 0.0f,  0.0f, -1.0f, 0.0f,  1.0f, 1.0f, // front right (bottom face) Vertex 0
         0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f, // back right (bottom face) Vertex 2
        -0.5f, -0.5f, -1.0f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f, // back left (bottom face) Vertex 3

       -0.5f,  0.5f, 0.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // front top (left face) Vertex 0
       -0.5f,  0.5f, -1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // back top (left face) Vertex 1
       -0.5f, -0.5f, -1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // back bottom (left face) Vertex 3
       -0.5f,  0.5f, 0.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // front top (left face) Vertex 0
       -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // front bottom (left face) Vertex 2
       -0.5f, -0.5f, -1.0f,  -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // back bottom (left face) Vertex 3

        0.5f,  0.5f, -1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // back top (right face) Vertex 0
        0.5f,  0.5f,  0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f, // front top (right face) Vertex 1
        0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // front bottom (right face) Vertex 3
        0.5f,  0.5f, -1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 1.0f, // back top (right face) Vertex 0
        0.5f, -0.5f, -1.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f, // back bottom (right face) Vertex 2
        0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, // front bottom (right face) Vertex 3

        0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 4.0f, // front right (top face) Vertex 0
       -0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 4.0f, // front left (top face) Vertex 1
       -0.5f,  0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // back left (top face) Vertex 3
        0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 4.0f, // front right (top face) Vertex 0
        0.5f,  0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // back right (top face) Vertex 2
       -0.5f,  0.5f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // back left (top face) Vertex 3

        0.5f,  0.5f, -1.0f,  0.0f, 0.0f, -1.0f,  1.0f, 3.0f, // top right (back face) Vertex 0
       -0.5f,  0.5f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 3.0f, // top left (back face) Vertex 1
       -0.5f, -0.5f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, // bottom left (back face) Vertex 3
        0.5f,  0.5f, -1.0f,  0.0f, 0.0f, -1.0f,  1.0f, 3.0f, // top right (back face) Vertex 0
        0.5f, -0.5f, -1.0f,  0.0f, 0.0f, -1.0f,  1.0f, 0.0f, // bottom right (back face) Vertex 2
       -0.5f, -0.5f, -1.0f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, // bottom left (back face) Vertex 3

        0.5f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 3.0f, // top right (front face) Vertex 0
       -0.5f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 3.0f, // top left (front face) Vertex 1
       -0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, // bottom left (front face) Vertex 3
        0.5f,  0.5f,  0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 3.0f, // top right (front face) Vertex 0
        0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, // bottom right (front face) Vertex 2
       -0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f // bottom left (front face) Vertex 3
    };

    // Sets the number of positional, normal, and texture coordinate data points
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    // Generate and bind the vertex array object (VAO)
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Creates 2 buffers: first one for vertex data; second one for indices
    glGenBuffers(1, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // 8 strides between vertex coordinates (x, y, z, nx, ny, nz, u, v)
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

    // Creates vertex attribute pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    // Unbind the VAO
    glBindVertexArray(0);
}

/**
 * @brief Creates a 3D sphere mesh.
 *
 * This function generates the vertices, normals, and texture coordinates for a 3D sphere mesh
 * and uploads them to the GPU. The sphere is centered at the origin.
 *
 * @param mesh The GLMesh structure to hold the mesh data.
 */
void UCreateSphere(GLMesh& mesh)
{
    unsigned int numSegments = 16; // Number of segments for the sphere

    vector<GLfloat> verts; // Vector to store vertex data

    float radius = 0.5f; // Radius of the sphere

    // Iterate through and generate the sphere
    for (unsigned int i = 0; i <= numSegments; ++i)
    {
        for (unsigned int j = 0; j <= numSegments; ++j)
        {
            // Calculate the position of each vertex
            float y = cos(glm::radians(180.0f - (i * 180.0f / numSegments)));
            float x = cos(glm::radians(j * 360.0f / numSegments)) * sin(glm::radians(180.0f - (i * 180.0f / numSegments)));
            float z = sin(glm::radians(j * 360.0f / numSegments)) * sin(glm::radians(180.0f - (i * 180.0f / numSegments)));

            // Position
            verts.push_back(radius * x);
            verts.push_back(radius * y);
            verts.push_back(radius * z);

            // Normal
            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            verts.push_back(normal.x);
            verts.push_back(normal.y);
            verts.push_back(normal.z);

            // UV
            verts.push_back((float)j / (float)numSegments);
            verts.push_back((float)i / (float)numSegments);
        }
    }

    vector<GLuint> indices; // Vector to store indices

    // Generate indices for the sphere
    for (unsigned int i = 0; i < numSegments; ++i)
    {
        for (unsigned int j = 0; j < numSegments; ++j)
        {
            int first = (i * (numSegments + 1)) + j;
            int second = first + numSegments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    // Sets the number of positional, normal, and texture coordinate data points
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    // Generate and bind the vertex array object (VAO)
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Creates 2 buffers: first one for vertex data; second one for indices
    glGenBuffers(2, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * verts.size(), &verts[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbos[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    // 8 strides between vertex coordinates (x, y, z, nx, ny, nz, u, v)
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

    // Creates vertex attribute pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    // Unbind the VAO
    glBindVertexArray(0);
}


/**
 * @brief Creates a 3D plane mesh.
 *
 * This function generates the vertices, normals, and texture coordinates for a 3D plane mesh
 * and uploads them to the GPU. The plane lies flat on the XZ plane.
 *
 * @param mesh The GLMesh structure to hold the mesh data.
 */
void UCreatePlane(GLMesh& mesh)
{
    // Define vertices for the plane, including positions, normals, and texture coordinates
    GLfloat verts[] = {
        // vertex positions   // normals        // texture coordinates
         1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // front right Vertex 0
        -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, // front left Vertex 1
        -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // back left Vertex 3
         1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, // front right Vertex 0
         1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f, // back right Vertex 2
        -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,  0.0f, 1.0f, // back left Vertex 3
    };

    // Sets the number of positional, normal, and texture coordinate data points
    const GLuint floatsPerVertex = 3;
    const GLuint floatsPerNormal = 3;
    const GLuint floatsPerUV = 2;

    // Generate and bind the vertex array object (VAO)
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    // Creates 2 buffers: first one for vertex data; second one for indices
    glGenBuffers(1, mesh.vbos);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbos[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    // 8 strides between vertex coordinates (x, y, z, nx, ny, nz, u, v)
    GLint stride = sizeof(float) * (floatsPerVertex + floatsPerNormal + floatsPerUV);

    // creates vertex attribute pointers
    glVertexAttribPointer(0, floatsPerVertex, GL_FLOAT, GL_FALSE, stride, 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, floatsPerNormal, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * floatsPerVertex));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, floatsPerUV, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * (floatsPerVertex + floatsPerNormal)));
    glEnableVertexAttribArray(2);

    // Unbind the VAO
    glBindVertexArray(0);
}


/**
 * @brief Cleans up the buffers and vertex arrays used by a mesh.
 *
 * This function deletes the vertex array object (VAO) and the vertex buffer objects (VBOs)
 * associated with the provided mesh, freeing up GPU resources.
 *
 * @param mesh The GLMesh structure containing the VAO and VBOs to be deleted.
 */
void UDestroyMesh(GLMesh& mesh)
{
    // Delete the vertex array object (VAO)
    glDeleteVertexArrays(1, &mesh.vao);

    // Delete the vertex buffer objects (VBOs)
    glDeleteBuffers(2, mesh.vbos);
}