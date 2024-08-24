#include "texture.h"
#include <stb_image.h>  // For image loading
#include <iostream>
using namespace std;


/**
 * @brief Flips an image vertically for Y axis orientation compatibility with OpenGL.
 *
 * This function takes an image and flips it vertically. This is needed because
 * stb stores images with the origin (0, 0) at the top-left corner, while
 * OpenGL expects the origin at the bottom-left corner.
 *
 * @param image A pointer to the image data.
 * @param width The width of the image in pixels.
 * @param height The height of the image in pixels.
 * @param channels The number of color channels in the image (e.g., 3 for RGB, 4 for RGBA).
 */
void flipImageVertically(unsigned char* image, int width, int height, int channels)
{
    // Iterate over each row until the middle of the image
    for (int j = 0; j < height / 2; ++j)
    {
        // Calculate the starting index of the current row and corresponding row
        int index1 = j * width * channels;
        int index2 = (height - 1 - j) * width * channels;

        // Swap each pixel in the current row with the corresponding pixel in the bottom row
        for (int i = width * channels; i > 0; --i)
        {
            unsigned char tmp = image[index1];
            image[index1] = image[index2];
            image[index2] = tmp;
            ++index1;
            ++index2;
        }
    }
}


/**
 * @brief Generates and loads a texture from an image file.
 *
 * This function uses the stb_image library to load an image file, then creates and configures an OpenGL texture.
 * The texture is flipped vertically to match OpenGL's expected orientation. Texture parameters such as wrapping
 * and filtering are set, and mipmaps are generated for the texture.
 *
 * @param filename The path to the image file to be loaded.
 * @param textureId The GLuint reference where the texture ID will be stored.
 * @return Returns true if the texture was successfully created and loaded, false otherwise.
 */
bool UCreateTexture(const char* filename, GLuint& textureId)
{
    int width, height, channels;

    // Load the image from file using stb_image library
    unsigned char* image = stbi_load(filename, &width, &height, &channels, 0);
    if (image)
    {
        // Flip the image vertically to match OpenGL's Y-axis orientation
        flipImageVertically(image, width, height, channels);

        // Generate and bind the texture
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        // Set the texture wrapping parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set texture filtering parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Determine the format based on the number of channels
        if (channels == 3)

            // Create a texture with RGB format
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

        else if (channels == 4)

            // Create a texture with RGBA format
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

        else
        {
            // Create a texture with RGBA format
            cout << "Not implemented to handle image with " << channels << " channels" << endl;
            return false;
        }

        // Generate mipmaps for the texture
        glGenerateMipmap(GL_TEXTURE_2D);

        // Free the image memory
        stbi_image_free(image);

        // Unbind the texture
        glBindTexture(GL_TEXTURE_2D, 0);

        return true;
    }

    // Error loading the image
    return false;
}


/**
 * @brief Deletes an OpenGL texture.
 *
 * This function deletes an OpenGL texture identified by the given texture ID.
 *
 * @param textureId The ID of the texture to be deleted.
 */
void UDestroyTexture(GLuint textureId)
{
    glDeleteTextures(1, &textureId);
}