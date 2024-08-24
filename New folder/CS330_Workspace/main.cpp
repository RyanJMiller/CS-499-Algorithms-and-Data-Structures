// main inclusions
#include <iostream>         // for input/output
#include <cstdlib>          // for exit failure and success macros
#include <GL/glew.h>        // GLEW library
#include <GLFW/glfw3.h>     // GLFW library

// GLM inclusions
#include <glm/glm.hpp>      // OpenGL Math library
#include <glm/gtx/transform.hpp>    // used for translations, rotations, scaling, and perspective
#include <glm/gtc/type_ptr.hpp>     // used to send matrices to shaders
#include <glm/gtc/matrix_transform.hpp> // Include for glm::ortho and glm::perspective
#include <vector>

// image inclusions
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>      // used for funtions that can handle images

#include "mesh.h"
#include "shader.h"
#include "texture.h"

using namespace std; // using the standard namespace

// shader program macro
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version " core \n" #Source
#endif

// unnamed namespace to avoid conflicts
namespace
{
    const char* const WINDOW_TITLE = "Module Six Milestone"; // window title

    // constant defs for window width and height
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    // declaration of main GLFW window handle
    GLFWwindow* gWindow = nullptr;
    // declaration of GLMesh objects to hold VAO, VBOs
    GLMesh gMeshCylinder;
    GLMesh gMeshCube;
    GLMesh gMeshSphere;
    GLMesh gMeshPlane;
    // declaration of the texture ID
    GLuint gTexture1;
    GLuint gTexture2;
    GLuint gTexture3;
    GLuint gTexture4;
    GLuint gTexture5;
    // declaration of the shader program ID
    GLuint gProgramId;

    // camera parameters  
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 4.0f);   // position vector for the camera
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // forward vector for the camera
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);   // vertical vector for the camera

    // initialize with perspective view as default
    bool isOrthoView = false;

    // mouse input parameters
    bool firstMouse = true;         // checks for the first time mouse input
    float yaw = -90.0f;	            // initially rotate a bit to the left to avoid a jump.
    float pitch = 0.0f;             // levels the pitch with the xz plane
    float lastX = WINDOW_WIDTH / 2.0; // ensures that the initial previous x position is centered
    float lastY = WINDOW_HEIGHT / 2.0; // this serves the same function as lastX for the y position

    // timing parameters
    float gDeltaTime = 0.0f; // time between frames
    float gLastFrame = 0.0f; // time of last frame

    float cameraSpeed = 2.5f; // initial camera movement speed
}


/* User-defined Function prototypes to:
 * initialize the program, set the window size,
 * redraw graphics on the window when resized,
 * and render graphics on the screen
 */
bool UInitialize(int, char* [], GLFWwindow** window);
void UResizeWindow(GLFWwindow* window, int width, int height);
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos);
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void UProcessInput(GLFWwindow* window);
void URender();


// vertex shader source code
const GLchar* vertexShaderSource = GLSL(440,
    layout(location = 0) in vec3 position; // vertex data
    layout(location = 1) in vec3 normal; //normal data
    layout(location = 2) in vec2 textureCoordinate;  // texture coordinate data

    out vec2 vertexTextureCoordinate; // variable to transfer texture data to the fragment shader
    out vec3 FragPos;
    out vec3 Normal;

    // variables for transform matrices
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertices to clip coordinates
        vertexTextureCoordinate = textureCoordinate; // passes incoming texture data
        FragPos = vec3(model * vec4(position, 1.0f)); // transformed fragment position
        Normal = mat3(transpose(inverse(model))) * normal; // transformed normal
    }
);


// fragment shader source code
const GLchar* fragmentShaderSource = GLSL(440,
    in vec2 vertexTextureCoordinate; // holds incoming texture data from vertex shader
    in vec3 FragPos;
    in vec3 Normal;

    out vec4 fragmentColor; // output color

    uniform sampler2D uTexture; // sampler for texture

    // light uniforms for main and secondary lights
    uniform vec3 u_LightPos;    // position of light
    uniform vec3 u_LightColor;  // color of light

    uniform vec3 u_SpotLightPos;
    uniform vec3 u_SpotLightDirection;
    uniform float u_SpotLightCutOff;
    uniform float u_SpotLightOuterCutOff;
    uniform vec3 u_SpotLightColor;

    uniform vec3 u_CameraPos;   // position of camera for reflection

    void main()
    {
        // diffuse calculations
        float diffuseStrength = 0.6f; // intensity modifier
        vec3 norm = normalize(Normal);
        vec3 toLight = normalize(u_LightPos - FragPos);
        float diffuse = max(dot(norm, toLight), 0.0);
        vec3 light = diffuseStrength * u_LightColor * diffuse;

        // ambient calculations
        float ambientStrength = 0.25f; // Set ambient or global lighting strength
        vec3 ambient = ambientStrength * u_LightColor; // Generate ambient light color


        // Calculate the reflection direction for main light
        float specIntensity = 0.3f; // intensity modifier
        float highlightSize = 8.0f;
        vec3 viewDir = normalize(u_CameraPos - FragPos);
        vec3 reflectDir = reflect(-toLight, norm);

        float specComp = pow(max(dot(viewDir, reflectDir), 0.0), highlightSize);
        vec3 specular = u_LightColor * specComp * specIntensity;

        float spotSpecIntensity = 0.5f; // intensity modifier
        vec3 lightToFragment = normalize(FragPos - u_SpotLightPos);
        float theta = dot(lightToFragment, normalize(-u_SpotLightDirection));

        float epsilon = u_SpotLightCutOff - u_SpotLightOuterCutOff;
        float intensity = clamp((theta - u_SpotLightOuterCutOff) / epsilon, 0.0, 0.8);

        vec3 toSpotlight = normalize(u_SpotLightPos - FragPos);
        float spotlightDiffuseValue = max(dot(norm, toSpotlight), 0.0);
        vec3 reflectDirSpotlight = reflect(-toSpotlight, norm);
        float specCompSpotlight = pow(max(dot(viewDir, reflectDirSpotlight), 0.0), highlightSize);

        vec3 spotlightDiffuse = u_SpotLightColor * spotlightDiffuseValue * intensity;
        vec3 spotlightSpecular = u_SpotLightColor * specCompSpotlight * specIntensity * intensity;


        // combine results 
        vec3 texColor = texture(uTexture, vertexTextureCoordinate).rgb; // texture color
        vec3 finalColor = (ambient + light + specular + spotlightDiffuse + spotlightSpecular) * texColor;

        fragmentColor = vec4(finalColor, 1.0);
    }
);


// Entry Point
int main(int argc, char* argv[])
{
    // Initialize the application and create a window
    if (!UInitialize(argc, argv, &gWindow))
        return EXIT_FAILURE; // terminates program if initialization fails

    // Create meshes for the scene
    UCreateCylinder(gMeshCylinder);
    UCreateCube(gMeshCube);
    UCreateSphere(gMeshSphere);
    UCreatePlane(gMeshPlane);

    // Creates shader program
    if (!UCreateShaderProgram(vertexShaderSource, fragmentShaderSource, gProgramId))
        return EXIT_FAILURE; // terminates program if shader program fails

    // Load texture (relative to project's directory)
    const char* texFilename = "textures/metal.jpg";
    if (!UCreateTexture(texFilename, gTexture1))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // Tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gProgramId);
    // We set the texture as texture unit 0
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);

    // Load texture (relative to project's directory)
    texFilename = "textures/leather.jpg";
    if (!UCreateTexture(texFilename, gTexture2))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // Tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gProgramId);
    // We set the texture as texture unit 1
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 1);

    // Load texture (relative to project's directory)
    texFilename = "textures/paper.jpg";
    if (!UCreateTexture(texFilename, gTexture3))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // Tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gProgramId);
    // We set the texture as texture unit 2
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 2);

    // Load texture (relative to project's directory)
    texFilename = "textures/peel.jpg";
    if (!UCreateTexture(texFilename, gTexture4))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // Tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gProgramId);
    // We set the texture as texture unit 3
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 3);

    // Load texture (relative to project's directory)
    texFilename = "textures/plastic.jpg";
    if (!UCreateTexture(texFilename, gTexture5))
    {
        cout << "Failed to load texture " << texFilename << endl;
        return EXIT_FAILURE;
    }
    // Tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    glUseProgram(gProgramId);
    // We set the texture as texture unit 4
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 4);

    // sets the color to be used when clearing color buffers to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Main render loop
    while (!glfwWindowShouldClose(gWindow))
    {
        // per-frame timing logic
        float currentFrame = glfwGetTime();      // get current time
        gDeltaTime = currentFrame - gLastFrame;  // compute change in time
        gLastFrame = currentFrame;               // update last frame time

        // Handle input
        UProcessInput(gWindow);

        // Render the current frame
        URender();

        // Handle events
        glfwPollEvents();
    }

    // Cleanup resources
    UDestroyMesh(gMeshCylinder); // destroy cylinder mesh data
    UDestroyMesh(gMeshCube); // destroy cube mesh data
    UDestroyMesh(gMeshSphere); // destroy sphere mesh data
    UDestroyMesh(gMeshPlane); // destroy plane mesh data
    UDestroyTexture(gTexture1);
    UDestroyTexture(gTexture2);
    UDestroyTexture(gTexture3);
    UDestroyTexture(gTexture4);
    UDestroyTexture(gTexture5);
    UDestroyShaderProgram(gProgramId); // destroy shader program

    exit(EXIT_SUCCESS); // terminates the program successfully
}


/**
 * @brief Initializes GLFW, GLEW, and creates a window.
 *
 * This function initializes the GLFW library, sets the OpenGL context version and profile,
 * creates a window, sets various GLFW callbacks, and initializes the GLEW library.
 */
bool UInitialize(int argc, char* argv[], GLFWwindow** window)
{
    // GLFW: initialization
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // for MacOS compatibility
#endif

// GLFW: window creation
    * window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (*window == NULL) // if window could not be created
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return false;
    }

   /**
    * Makes the window's context current
    * and registers callbacks for:
    * window resizing,
    * mouse position,
    * mouse scrolling
    */
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, UResizeWindow);
    glfwSetCursorPosCallback(*window, UMousePositionCallback);
    glfwSetScrollCallback(*window, UMouseScrollCallback);

    // Tell GLFW to capture mouse input and disable cursor
    glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // GLEW: initialization
    glewExperimental = GL_TRUE;
    GLenum GlewInitResult = glewInit();

    if (GLEW_OK != GlewInitResult) // if initialization fails
    {
        cerr << glewGetErrorString(GlewInitResult) << endl;
        return false;
    }

    // Displays OpenGL version
    cout << "INFO: OpenGL Version: " << glGetString(GL_VERSION) << endl;

    return true;
}


/**
 * @brief Processes all relevant keyboard input and responds accordingly.
 *
 * This function handles keyboard input to move the camera and close the window.
 * It checks the state of specific keys and updates the camera position and view mode.
 *
 * @param window A pointer to the GLFW window.
 */
void UProcessInput(GLFWwindow* window)
{
    // Closes the window if escape is pressed
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Offset movement to account for frame timing
    float cameraOffset = cameraSpeed * gDeltaTime;

   /**
    * Moves the camera based on key input:
    *
    * if 'W' is pressed, move camera forward
    * if 'S' is pressed, move camera backward
    * if 'A' is pressed, move camera left
    * if 'D' is pressed, move camera right
    * if 'Q' is pressed, move camera up
    * if 'E' is pressed, move camera down
    * if 'P' is pressed, toggle between views
    */
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraOffset * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraOffset * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraOffset;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraOffset;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        cameraPos += cameraOffset * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        cameraPos -= cameraOffset * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        isOrthoView = !isOrthoView;  // toggle between ortho and perspective view
    }
}


/**
 * @brief Callback function executed whenever the window size changes.
 *
 * This function is registered as a callback with GLFW and is called whenever
 * the window is resized. It updates the viewport dimensions to match the new
 * window size.
 *
 * @param window A pointer to the GLFW window.
 * @param width The new width of the window.
 * @param height The new height of the window.
 */
void UResizeWindow(GLFWwindow* window, int width, int height)
{
    // Set the viewport to cover the new window dimensions
    glViewport(0, 0, width, height);
}


/**
 * @brief Callback function executed whenever the mouse moves.
 *
 * This function is registered as a callback with GLFW and is called whenever
 * the mouse moves. It updates the camera direction based on the new mouse
 * position, allowing for interactive camera control.
 *
 * @param window A pointer to the GLFW window.
 * @param xpos The new x-coordinate of the mouse.
 * @param ypos The new y-coordinate of the mouse.
 */
void UMousePositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    // If this is the first time the mouse moved (after the window opened)
    if (firstMouse)
    {
        lastX = xpos;       // Update last x-coordinate
        lastY = ypos;       // Update last y-coordinate
        firstMouse = false; // first mouse condition no longer triggers
    }

    // Calculate the offset from the last mouse position
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // This is inverted because y-coordinate range is reversed
    lastX = xpos; // Update last x-coordinate
    lastY = ypos; // Update last y-coordinate

    // Sets and applies mouse sensitivity
    float sensitivity = 0.2f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // Update yaw and pitch based on the mouse offset
    yaw += xoffset;
    pitch += yoffset;

    // Constrain the pitch to avoid screen flip
    // Makes sure that pitch is corrected if out of bounds (upper)
    if (pitch > 89.0f)
    {
        pitch = 89.0f;
    }

    // Makes sure that pitch is corrected if out of bounds (lower)
    if (pitch < -89.0f)
    {
        pitch = -89.0f;
    }

    // Euler angles formula to convert yaw and pitch values to 3D direction vectors
    // Calculate the new front vector based on updated yaw and pitch
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}


/**
 * @brief Callback function executed whenever the mouse scroll wheel scrolls.
 *
 * This function is registered as a callback with GLFW and is called whenever
 * the mouse scroll wheel is used. It adjusts the camera speed based on the
 * scroll input, ensuring the speed remains within defined bounds.
 *
 * @param window A pointer to the GLFW window.
 * @param xoffset The scroll offset along the x-axis.
 * @param yoffset The scroll offset along the y-axis.
 */
void UMouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Adjusts camera speed based of scroll input
    cameraSpeed += (float)yoffset;

    // Checks if speed would be out of set bounds (1-10) and corrects it
    if (cameraSpeed < 1.0f)
        cameraSpeed = 1.0f;
    if (cameraSpeed > 10.0f)
        cameraSpeed = 10.0f;
}


/**
 * @brief Renders the frame.
 *
 * This function is called to render each frame. It clears the frame and depth buffers,
 * sets up transformation matrices, updates shader uniforms, and draws the objects
 * (two cylinders, two cubes, a sphere, and a plane) in the scene.
 */
void URender()
{
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    // Clear the frame and depth buffers
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Define transformation matrices for each object
    glm::mat4 modelCylinder = glm::translate(glm::vec3(-1.0f, 0.0f, -0.5f)) *
        glm::rotate(1.5708f, glm::vec3(-1.5708f, 0.0f, 1.0f)) * 
        glm::scale(glm::vec3(0.3f, 0.1f, 0.3f));

    glm::mat4 modelCube = glm::translate(glm::vec3(-1.7f, 0.0f, -0.35f)) *
        glm::rotate(1.0f, glm::vec3(0.0f, -0.5f, 0.0f)) *
        glm::scale(glm::vec3(0.3f, 0.8f, 1.2f));

    glm::mat4 modelSphere = glm::translate(glm::vec3(-0.5f, -0.1501f, 1.0f)) *
        glm::rotate(0.0f, glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));

    glm::mat4 modelCylinder02 = glm::translate(glm::vec3(0.0f, -0.2f, 0.0f)) *
        glm::rotate(1.0f, glm::vec3(0.0f, -0.5f, 0.0f)) * 
        glm::scale(glm::vec3(0.3f, 0.201f, 0.3f));

    glm::mat4 modelCube02 = glm::translate(glm::vec3(1.0f, -0.025f, 0.0f)) *
        glm::rotate(1.5708f, glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::vec3(0.3f, 0.05f, 0.8f));

    glm::mat4 modelPlane = glm::translate(glm::vec3(0.0f, -0.4f, 0.0f)) *
        glm::rotate(1.5708f, glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::vec3(3.0f, 3.0f, 3.0f));

    // Create view matrix with previously defined lookAt parameters
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Create perspective and orthographic projection matrices
    glm::mat4 perspectiveProjection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    glm::mat4 orthoProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);


    // Sets the shader to be used
    glUseProgram(gProgramId);

    // Set camera position uniform
    GLint cameraPosLoc = glGetUniformLocation(gProgramId, "u_CameraPos");
    glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));

    // Set the lighting uniforms
    GLint lightPosLoc = glGetUniformLocation(gProgramId, "u_LightPos");
    GLint lightColorLoc = glGetUniformLocation(gProgramId, "u_LightColor");

    glUniform3f(lightPosLoc, 0.0f, 1.0f, 0.0f); // light position
    glUniform3f(lightColorLoc, 1.0f, 1.0f, 0.8f); // light color

    // Set the spotlight uniforms
    GLint spotLightPosLoc = glGetUniformLocation(gProgramId, "u_SpotLightPos");
    GLint spotLightColorLoc = glGetUniformLocation(gProgramId, "u_SpotLightColor");
    GLint spotLightDirLoc = glGetUniformLocation(gProgramId, "u_SpotLightDirection");
    GLint spotLightCutOffLoc = glGetUniformLocation(gProgramId, "u_SpotLightCutOff");
    GLint spotLightOuterCutOffLoc = glGetUniformLocation(gProgramId, "u_SpotLightOuterCutOff");

    glUniform3f(spotLightPosLoc, 3.0f, 3.0f, 1.0f);
    glUniform3f(spotLightColorLoc, 1.0f, 0.6f, 0.06f);
    glUniform3f(spotLightDirLoc, 3.0f, 3.0f, 1.0f);
    glUniform1f(spotLightCutOffLoc, cos(glm::radians(12.5f)));
    glUniform1f(spotLightOuterCutOffLoc, cos(glm::radians(17.5f)));

    // Retrieve and pass transform matrices to the shader program
    GLint modelLoc = glGetUniformLocation(gProgramId, "model");
    GLint viewLoc = glGetUniformLocation(gProgramId, "view");
    GLint projLoc = glGetUniformLocation(gProgramId, "projection");

    // Draw the first cylinder
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCylinder));
    glBindVertexArray(gMeshCylinder.vao);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 0);
    glBindTexture(GL_TEXTURE_2D, gTexture1);
    glDrawElements(GL_TRIANGLES, 12 * 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Draw the first cube
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCube));
    glBindVertexArray(gMeshCube.vao);
    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 1);
    glBindTexture(GL_TEXTURE_2D, gTexture2);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Draw the sphere
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelSphere));
    glBindVertexArray(gMeshSphere.vao);
    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 3);
    glBindTexture(GL_TEXTURE_2D, gTexture4);
    glDrawElements(GL_TRIANGLES, 6 * 16 * 16, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Draw the second cylinder
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCylinder02));
    glBindVertexArray(gMeshCylinder.vao);
    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 4);
    glBindTexture(GL_TEXTURE_2D, gTexture5);
    glDrawElements(GL_TRIANGLES, 12 * 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Draw the second cube
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelCube02));
    glBindVertexArray(gMeshCube.vao);
    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 4);
    glBindTexture(GL_TEXTURE_2D, gTexture5);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Draw the plane
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelPlane));
    glBindVertexArray(gMeshPlane.vao);
    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(gProgramId, "uTexture"), 2);
    glBindTexture(GL_TEXTURE_2D, gTexture3);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);


    // Pass view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // Pass projection matrix to shader based on view mode
    if (isOrthoView)
    {
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(orthoProjection));
    }
    else
    {
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(perspectiveProjection));
    }

    // Swap buffers and poll for IO events
    glfwSwapBuffers(gWindow);
}