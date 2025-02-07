#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "fan.h"
#include "cylinders.h"
#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 800;

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;
bool fan_turn = false;
bool rotate_around = false;

// camera
Camera camera(glm::vec3(-3.0f, 2.5f, 4.3f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//Eye position
float eyeX = 0.0, eyeY = 1.0, eyeZ = 4.1;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);

// timing
float deltaTime = 0.0f;  
float lastFrame = 0.0f;

glm::mat4 transforamtion(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz) {
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
    translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
    model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
    return model;
}

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LAB FINAL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    
    //axis
    float cube_vertices2[] = {
        -0.25f, -0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        0.25f, -0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        0.25f, 0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        -0.25f, 0.25f, -0.25f, 0.0f, 0.0f, 0.0f,
        -0.25f, -0.25f, 0.25f, 0.3f, 0.8f, 0.5f,
        0.25f, -0.25f, 0.25f, 0.5f, 0.4f, 0.3f,
        0.25f, 0.25f, 0.25f, 0.2f, 0.7f, 0.3f,
        -0.25f, 0.25f, 0.25f, 0.6f, 0.2f, 0.8f,
    };
    float cube_vertices[] = {
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,

        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    float floor[] = {
        0.0f, 0.0f, 0.0f, 0.57f, 0.69f, 0.57f,
        0.5f, 0.0f, 0.0f, 0.57f, 0.69f, 0.57f,
        0.5f, 0.5f, 0.0f, 0.57f, 0.69f, 0.57f,
        0.0f, 0.5f, 0.0f, 0.57f, 0.69f, 0.57f,

        0.5f, 0.0f, 0.0f, 0.57f, 0.69f, 0.57f,
        0.5f, 0.5f, 0.0f, 0.57f, 0.69f, 0.57f,
        0.5f, 0.0f, 0.5f, 0.57f, 0.69f, 0.57f,
        0.5f, 0.5f, 0.5f, 0.57f, 0.69f, 0.57f,

        0.0f, 0.0f, 0.5f, 0.57f, 0.69f, 0.57f,
        0.5f, 0.0f, 0.5f, 0.57f, 0.69f, 0.57f,
        0.5f, 0.5f, 0.5f, 0.57f, 0.69f, 0.57f,
        0.0f, 0.5f, 0.5f, 0.57f, 0.69f, 0.57f,

        0.0f, 0.0f, 0.5f, 0.57f, 0.69f, 0.57f,
        0.0f, 0.5f, 0.5f, 0.57f, 0.69f, 0.57f,
        0.0f, 0.5f, 0.0f, 0.57f, 0.69f, 0.57f,
        0.0f, 0.0f, 0.0f, 0.57f, 0.69f, 0.57f,

        0.5f, 0.5f, 0.5f, 0.57f, 0.69f, 0.57f,
        0.5f, 0.5f, 0.0f, 0.57f, 0.69f, 0.57f,
        0.0f, 0.5f, 0.0f, 0.57f, 0.69f, 0.57f,
        0.0f, 0.5f, 0.5f, 0.57f, 0.69f, 0.57f,

        0.0f, 0.0f, 0.0f, 0.57f, 0.69f, 0.57f,
        0.5f, 0.0f, 0.0f, 0.57f, 0.69f, 0.57f,
        0.5f, 0.0f, 0.5f, 0.57f, 0.69f, 0.57f,
        0.0f, 0.0f, 0.5f, 0.57f, 0.69f, 0.57f
    };
    float floor2[] = {
       0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f,
       0.5f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f,
       0.5f, 0.5f, 0.0f, 0.75f, 0.75f, 0.75f,
       0.0f, 0.5f, 0.0f, 0.75f, 0.75f, 0.75f,

       0.5f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f,
       0.5f, 0.5f, 0.0f, 0.75f, 0.75f, 0.75f,
       0.5f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f,
       0.5f, 0.5f, 0.5f, 0.75f, 0.75f, 0.75f,

       0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f,
       0.5f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f,
       0.5f, 0.5f, 0.5f, 0.75f, 0.75f, 0.75f,
       0.0f, 0.5f, 0.5f, 0.75f, 0.75f, 0.75f,

       0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f,
       0.0f, 0.5f, 0.5f, 0.75f, 0.75f, 0.75f,
       0.0f, 0.5f, 0.0f, 0.75f, 0.75f, 0.75f,
       0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f,

       0.5f, 0.5f, 0.5f, 0.75f, 0.75f, 0.75f,
       0.5f, 0.5f, 0.0f, 0.75f, 0.75f, 0.75f,
       0.0f, 0.5f, 0.0f, 0.75f, 0.75f, 0.75f,
       0.0f, 0.5f, 0.5f, 0.75f, 0.75f, 0.75f,

       0.0f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f,
       0.5f, 0.0f, 0.0f, 0.75f, 0.75f, 0.75f,
       0.5f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f,
       0.0f, 0.0f, 0.5f, 0.75f, 0.75f, 0.75f
    };
    float wall1[] = {
        0.0f, 0.0f, 0.0f, 0.40f, 0.40f, 0.83f,
        0.5f, 0.0f, 0.0f, 0.40f, 0.40f, 0.83f,
        0.5f, 0.5f, 0.0f, 0.40f, 0.40f, 0.83f,
        0.0f, 0.5f, 0.0f, 0.40f, 0.40f, 0.83f,

        0.5f, 0.0f, 0.0f, 0.40f, 0.40f, 0.83f,
        0.5f, 0.5f, 0.0f, 0.40f, 0.40f, 0.83f,
        0.5f, 0.0f, 0.5f, 0.40f, 0.40f, 0.83f,
        0.5f, 0.5f, 0.5f, 0.40f, 0.40f, 0.83f,

        0.0f, 0.0f, 0.5f, 0.40f, 0.40f, 0.83f,
        0.5f, 0.0f, 0.5f, 0.40f, 0.40f, 0.83f,
        0.5f, 0.5f, 0.5f, 0.40f, 0.40f, 0.83f,
        0.0f, 0.5f, 0.5f, 0.40f, 0.40f, 0.83f,

        0.0f, 0.0f, 0.5f, 0.40f, 0.40f, 0.83f,
        0.0f, 0.5f, 0.5f, 0.40f, 0.40f, 0.83f,
        0.0f, 0.5f, 0.0f, 0.40f, 0.40f, 0.83f,
        0.0f, 0.0f, 0.0f, 0.40f, 0.40f, 0.83f,

        0.5f, 0.5f, 0.5f, 0.40f, 0.40f, 0.83f,
        0.5f, 0.5f, 0.0f, 0.40f, 0.40f, 0.83f,
        0.0f, 0.5f, 0.0f, 0.40f, 0.40f, 0.83f,
        0.0f, 0.5f, 0.5f, 0.40f, 0.40f, 0.83f,

        0.0f, 0.0f, 0.0f, 0.40f, 0.40f, 0.83f,
        0.5f, 0.0f, 0.0f, 0.40f, 0.40f, 0.83f,
        0.5f, 0.0f, 0.5f, 0.40f, 0.40f, 0.83f,
        0.0f, 0.0f, 0.5f, 0.40f, 0.40f, 0.83f
    };
    float wall2[] = {
        0.0f, 0.0f, 0.0f, 0.40f, 0.40f, 0.75,
        0.5f, 0.0f, 0.0f, 0.40f, 0.40f, 0.75,
        0.5f, 0.5f, 0.0f, 0.40f, 0.40f, 0.75,
        0.0f, 0.5f, 0.0f, 0.40f, 0.40f, 0.75,

        0.5f, 0.0f, 0.0f, 0.40f, 0.40f, 0.75,
        0.5f, 0.5f, 0.0f, 0.40f, 0.40f, 0.75,
        0.5f, 0.0f, 0.5f, 0.40f, 0.40f, 0.75,
        0.5f, 0.5f, 0.5f, 0.40f, 0.40f, 0.75,

        0.0f, 0.0f, 0.5f, 0.40f, 0.40f, 0.75,
        0.5f, 0.0f, 0.5f, 0.40f, 0.40f, 0.75,
        0.5f, 0.5f, 0.5f, 0.40f, 0.40f, 0.75,
        0.0f, 0.5f, 0.5f, 0.40f, 0.40f, 0.75,

        0.0f, 0.0f, 0.5f, 0.40f, 0.40f, 0.75,
        0.0f, 0.5f, 0.5f, 0.40f, 0.40f, 0.75,
        0.0f, 0.5f, 0.0f, 0.40f, 0.40f, 0.75,
        0.0f, 0.0f, 0.0f, 0.40f, 0.40f, 0.75,

        0.5f, 0.5f, 0.5f, 0.40f, 0.40f, 0.75,
        0.5f, 0.5f, 0.0f, 0.40f, 0.40f, 0.75,
        0.0f, 0.5f, 0.0f, 0.40f, 0.40f, 0.75,
        0.0f, 0.5f, 0.5f, 0.40f, 0.40f, 0.75,

        0.0f, 0.0f, 0.0f, 0.40f, 0.40f, 0.75,
        0.5f, 0.0f, 0.0f, 0.40f, 0.40f, 0.75,
        0.5f, 0.0f, 0.5f, 0.40f, 0.40f, 0.75,
        0.0f, 0.0f, 0.5f, 0.40f, 0.40f, 0.75,
    };
    float wall3[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.4f,
        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.4f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.4f,

        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.4f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.4f,
        0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.4f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.4f,

        0.0f, 0.0f, 0.5f, 0.0f, 1.0f, 0.4f,
        0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.4f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.4f,

        0.0f, 0.0f, 0.5f, 0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.4f,
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.4f,

        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.4f,
        0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.4f,

        0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.4f,
        0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.4f,
        0.5f, 0.0f, 0.5f, 0.0f, 1.0f, 0.4f,
        0.0f, 0.0f, 0.5f, 0.0f, 1.0f, 0.4f,
    };
    float wall4[] = {
        0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.4f,
        0.5f, 0.0f, 0.0f,  0.0f, 1.0f, 0.4f,
        0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 0.4f,

        0.5f, 0.0f, 0.0f,  0.0f, 1.0f, 0.4f,
        0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.4f,
        0.5f, 0.0f, 0.5f,  0.0f, 1.0f, 0.4f,
        0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.4f,

        0.0f, 0.0f, 0.5f,  0.0f, 1.0f, 0.4f,
        0.5f, 0.0f, 0.5f,  0.0f, 1.0f, 0.4f,
        0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.5f,  0.0f, 1.0f, 0.4f,

        0.0f, 0.0f, 0.5f,  0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.5f,  0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 0.4f,
        0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.4f,

        0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.4f,
        0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.0f,  0.0f, 1.0f, 0.4f,
        0.0f, 0.5f, 0.5f,  0.0f, 1.0f, 0.4f,

        0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.4f,
        0.5f, 0.0f, 0.0f,  0.0f, 1.0f, 0.4f,
        0.5f, 0.0f, 0.5f,  0.0f, 1.0f, 0.4f,
        0.0f, 0.0f, 0.5f,  0.0f, 1.0f, 0.4f,
    };
    float box[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.251f, 0.102f,
        0.5f, 0.0f, 0.0f, 0.0f, 0.251f, 0.102f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.251f, 0.102f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.251f, 0.102f,

        0.5f, 0.0f, 0.0f, 0.0f, 0.251f, 0.102f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.251f, 0.102f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.251f, 0.102f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.251f, 0.102f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.251f, 0.102f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.251f, 0.102f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.251f, 0.102f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.251f, 0.102f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.251f, 0.102f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.251f, 0.102f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.251f, 0.102f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.251f, 0.102f,

        0.5f, 0.5f, 0.5f, 0.0f, 0.251f, 0.102f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.251f, 0.102f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.251f, 0.102f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.251f, 0.102f,

        0.0f, 0.0f, 0.0f, 0.0f, 0.251f, 0.102f,
        0.5f, 0.0f, 0.0f, 0.0f, 0.251f, 0.102f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.251f, 0.102f,
        0.0f, 0.0f, 0.5f, 0.0f, 0.251f, 0.102f
    };
    float box2[] = {
        0.0f, 0.0f, 0.0f, 0,0,0,
        0.5f, 0.0f, 0.0f, 0,0,0,
        0.5f, 0.5f, 0.0f, 0,0,0,
        0.0f, 0.5f, 0.0f, 0,0,0,

        0.5f, 0.0f, 0.0f, 0,0,0,
        0.5f, 0.5f, 0.0f, 0,0,0,
        0.5f, 0.0f, 0.5f, 0,0,0,
        0.5f, 0.5f, 0.5f, 0,0,0,

        0.0f, 0.0f, 0.5f, 0,0,0,
        0.5f, 0.0f, 0.5f, 0,0,0,
        0.5f, 0.5f, 0.5f, 0,0,0,
        0.0f, 0.5f, 0.5f, 0,0,0,

        0.0f, 0.0f, 0.5f, .41, .24, .12,
        0.0f, 0.5f, 0.5f, .41, .24, .12,
        0.0f, 0.5f, 0.0f, .41, .24, .12,
        0.0f, 0.0f, 0.0f, .41, .24, .12,

        0.5f, 0.5f, 0.5f, .41, .24, .12,
        0.5f, 0.5f, 0.0f, .41, .24, .12,
        0.0f, 0.5f, 0.0f, .41, .24, .12,
        0.0f, 0.5f, 0.5f, .41, .24, .12,

        //0.0f, 0.0f, 0.0f, 0,0,0,
        //0.5f, 0.0f, 0.0f, 0,0,0,
        //0.5f, 0.0f, 0.5f, 0,0,0,
        //0.0f, 0.0f, 0.5f, 0,0,0
    };
    float ceiling[] = {
        0.0f, 0.0f, 0.0f, 0.466f, 0.631f, 0.827f,
        0.5f, 0.0f, 0.0f, 0.466f, 0.631f, 0.827f,
        0.5f, 0.5f, 0.0f, 0.466f, 0.631f, 0.827f,
        0.0f, 0.5f, 0.0f, 0.466f, 0.631f, 0.827f,

        0.5f, 0.0f, 0.0f, 0.466f, 0.631f, 0.827f,
        0.5f, 0.5f, 0.0f, 0.466f, 0.631f, 0.827f,
        0.5f, 0.0f, 0.5f, 0.466f, 0.631f, 0.827f,
        0.5f, 0.5f, 0.5f, 0.466f, 0.631f, 0.827f,

        0.0f, 0.0f, 0.5f, 0.466f, 0.631f, 0.827f,
        0.5f, 0.0f, 0.5f, 0.466f, 0.631f, 0.827f,
        0.5f, 0.5f, 0.5f, 0.466f, 0.631f, 0.827f,
        0.0f, 0.5f, 0.5f, 0.466f, 0.631f, 0.827f,

        0.0f, 0.0f, 0.5f, 0.466f, 0.631f, 0.827f,
        0.0f, 0.5f, 0.5f, 0.466f, 0.631f, 0.827f,
        0.0f, 0.5f, 0.0f, 0.466f, 0.631f, 0.827f,
        0.0f, 0.0f, 0.0f, 0.466f, 0.631f, 0.827f,

        0.5f, 0.5f, 0.5f, 0.466f, 0.631f, 0.827f,
        0.5f, 0.5f, 0.0f, 0.466f, 0.631f, 0.827f,
        0.0f, 0.5f, 0.0f, 0.466f, 0.631f, 0.827f,
        0.0f, 0.5f, 0.5f, 0.466f, 0.631f, 0.827f,

        0.0f, 0.0f, 0.0f, 0.466f, 0.631f, 0.827f,
        0.5f, 0.0f, 0.0f, 0.466f, 0.631f, 0.827f,
        0.5f, 0.0f, 0.5f, 0.466f, 0.631f, 0.827f,
        0.0f, 0.0f, 0.5f, 0.466f, 0.631f, 0.827f
    };
    float lamp_ver[] = {
       

       0.5f, 0.0f, 0.0f, 0.96, .89, .26,
       0.4f, 0.5f, 0.1f, 0.96, .89, .26,
       0.5f, 0.0f, 0.5f, 0.96, .52, .26,
       0.4f, 0.5f, 0.4f, 0.96, .52, .26,

       0.0f, 0.0f, 0.0f, 0.96, .52, .26,
       0.5f, 0.0f, 0.0f, 0.96, .52, .26,
       0.1f, 0.5f, 0.1f, 0.96, .89, .26,
       0.4f, 0.5f, 0.1f, 0.96, .89, .26,

       0.0f, 0.0f, 0.5f, 0.96, .52, .26,
       0.5f, 0.0f, 0.5f, 0.96, .52, .26,
       0.4f, 0.5f, 0.4f, 0.96, .89, .26,
       0.1f, 0.5f, 0.4f, 0.96, .89, .26,

       0.0f, 0.0f, 0.5f, 0.96, .52, .26,
       0.1f, 0.5f, 0.4f, 0.96, .52, .26,
       0.1f, 0.5f, 0.1f, 0.96, .89, .26,
       0.0f, 0.0f, 0.0f, 0.96, .89, .26,

       0.4f, 0.5f, 0.4f, 0,0,0,
       0.4f, 0.5f, 0.1f, 0,0,0,
       0.1f, 0.5f, 0.1f, 0,0,0,
       0.1f, 0.5f, 0.4f, 0,0,0,

       0.0f, 0.0f, 0.0f, 0.96, .89, .26,
       0.5f, 0.0f, 0.0f, 0.96, .89, .26,
       0.5f, 0.0f, 0.5f, 0.96, .89, .26,
       0.0f, 0.0f, 0.5f, 0.96, .89, .26,
    };
    float fan_holder[] = {
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,

        0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,

        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.0f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f
    };
    float fan_pivot[] = {
        0.0f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.0f, .44f, .22f, .05f,

        0.5f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.5f, .44f, .22f, .05f,

        0.0f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.5f, .44f, .22f, .05f,

        0.0f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.0f, 0.0f, .44f, .22f, .05f,

        0.5f, 0.5f, 0.5f, .44f, .22f, .05f,
        0.5f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.0f, .44f, .22f, .05f,
        0.0f, 0.5f, 0.5f, .44f, .22f, .05f,

        0.0f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.0f, .44f, .22f, .05f,
        0.5f, 0.0f, 0.5f, .44f, .22f, .05f,
        0.0f, 0.0f, 0.5f, .44f, .22f, .05f
    };
    float fan_blade[] = {
        0.0f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.0f, .0f, .0f, .42f,

        0.5f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.5f, .0f, .0f, .42f,

        0.0f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.5f, .0f, .0f, .42f,

        0.0f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.0f, 0.0f, .0f, .0f, .42f,

        0.5f, 0.5f, 0.5f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.5f, .0f, .0f, .42f,

        0.0f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.5f, .0f, .0f, .42f,
        0.0f, 0.0f, 0.5f, .0f, .0f, .42f
    };
    float glass[] = {//window
        0.0f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.0f, 0.0f, .0f, .0f, .42f,
        0.5f, 0.5f, 0.0f, .0f, .0f, .42f,
        0.0f, 0.5f, 0.0f, .0f, .0f, .42f,

    };
    float tv1[] = {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
    };
    /*******************AC********************/
    float ac[] = {
        0.0f, 0.0f, 0.0f,0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.25f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.0f, 0.2f, 0.2f, 0.2f,

        0.25f, 0.0f, 0.0f, 0.8f, 0.8f, 0.8f,
        0.25f, 0.25f, 0.0f, 0.8f, 0.8f, 0.8f,
        0.25f, 0.0f, 0.5f, 0.8f, 0.8f, 0.8f,
        0.25f, 0.25f, 0.5f, 0.8f, 0.8f, 0.8f,

        0.0f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.25f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.5f, 0.2f, 0.2f, 0.2f,

        0.0f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,

        0.25f, 0.25f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.25f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.5f, 0.5f, 0.2f, 0.2f, 0.2f,

        0.0f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.0f, 0.2f, 0.2f, 0.2f,
        0.25f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f,
        0.0f, 0.0f, 0.5f, 0.2f, 0.2f, 0.2f
    };


    unsigned int VBOCA, VAOCA, EBOCA;
    glGenVertexArrays(1, &VAOCA);
    glGenBuffers(1, &VBOCA);
    glGenBuffers(1, &EBOCA);
    glBindVertexArray(VAOCA);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCA);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(cabinate), cabinate, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOCA);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOT, VAOT, EBOT;
    glGenVertexArrays(1, &VAOT);
    glGenBuffers(1, &VBOT);
    glGenBuffers(1, &EBOT);
    glBindVertexArray(VAOT);
    glBindBuffer(GL_ARRAY_BUFFER, VBOT);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ceiling), ceiling, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOT);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    
    unsigned int VBOLMP, VAOLMP, EBOLMP;
    glGenVertexArrays(1, &VAOLMP);
    glGenBuffers(1, &VBOLMP);
    glGenBuffers(1, &EBOLMP);
    glBindVertexArray(VAOLMP);
    glBindBuffer(GL_ARRAY_BUFFER, VBOLMP);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lamp_ver), lamp_ver, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOLMP);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOG, VAOG, EBOG;
    glGenVertexArrays(1, &VAOG);
    glGenBuffers(1, &VBOG);
    glGenBuffers(1, &EBOG);
    glBindVertexArray(VAOG);
    glBindBuffer(GL_ARRAY_BUFFER, VBOG);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor), floor, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOG);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);
    /*----------------   floor2   -----------------*/
    unsigned int VBODD, VAODD, EBODD;
    glGenVertexArrays(1, &VAODD);
    glGenBuffers(1, &VBODD);
    glGenBuffers(1, &EBODD);
    glBindVertexArray(VAODD);
    glBindBuffer(GL_ARRAY_BUFFER, VBODD);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor2), floor2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBODD);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOW, VAOW, EBOW;
    glGenVertexArrays(1, &VAOW);
    glGenBuffers(1, &VBOW);
    glGenBuffers(1, &EBOW);
    glBindVertexArray(VAOW);
    glBindBuffer(GL_ARRAY_BUFFER, VBOW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall1), wall1, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    //*********************Wall3************************************
    unsigned int VBOQ, VAOQ, EBOQ;
    glGenVertexArrays(1, &VAOQ);
    glGenBuffers(1, &VBOQ);
    glGenBuffers(1, &EBOQ);
    glBindVertexArray(VAOQ);
    glBindBuffer(GL_ARRAY_BUFFER, VBOQ);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall3), wall3, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOQ);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);
    //*******************************************************************

    unsigned int VBOW1, VAOW1, EBOW1;
    glGenVertexArrays(1, &VAOW1);
    glGenBuffers(1, &VBOW1);
    glGenBuffers(1, &EBOW1);
    glBindVertexArray(VAOW1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOW1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall2), wall2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOW1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);
    //******************  Wall4 -------------------
    unsigned int VBOWY, VAOWY, EBOWY;
    glGenVertexArrays(1, &VAOWY);
    glGenBuffers(1, &VBOWY);
    glGenBuffers(1, &EBOWY);
    glBindVertexArray(VAOWY);
    glBindBuffer(GL_ARRAY_BUFFER, VBOWY);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wall4), wall4, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOWY);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    //*****************************  AC  ****************************************
    unsigned int VBOAC, VAOAC, EBOAC;
    glGenVertexArrays(1, &VAOAC);
    glGenBuffers(1, &VBOAC);
    glGenBuffers(1, &EBOAC);
    glBindVertexArray(VAOAC);
    glBindBuffer(GL_ARRAY_BUFFER, VBOAC);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ac), ac, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOAC);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);
    /*---------------------------------------------------------------------*/

    unsigned int VBOC, VAOC, EBOC;
    glGenVertexArrays(1, &VAOC);
    glGenBuffers(1, &VBOC);
    glGenBuffers(1, &EBOC);
    glBindVertexArray(VAOC);
    glBindBuffer(GL_ARRAY_BUFFER, VBOC);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOC);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOC2, VAOC2, EBOC2;
    glGenVertexArrays(1, &VAOC2);
    glGenBuffers(1, &VBOC2);
    glGenBuffers(1, &EBOC2);
    glBindVertexArray(VAOC2);
    glBindBuffer(GL_ARRAY_BUFFER, VBOC2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(box2), box2, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOC2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOTV, VAOTV, EBOTV;
    glGenVertexArrays(1, &VAOTV);
    glGenBuffers(1, &VBOTV);
    glGenBuffers(1, &EBOTV);
    glBindVertexArray(VAOTV);
    glBindBuffer(GL_ARRAY_BUFFER, VBOTV);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tv1), tv1, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOTV);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    //Fan
    unsigned int VBOF1, VAOF1, EBOF1;
    glGenVertexArrays(1, &VAOF1);
    glGenBuffers(1, &VBOF1);
    glGenBuffers(1, &EBOF1);
    glBindVertexArray(VAOF1);
    glBindBuffer(GL_ARRAY_BUFFER, VBOF1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fan_holder), fan_holder, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOF1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOF2, VAOF2, EBOF2;
    glGenVertexArrays(1, &VAOF2);
    glGenBuffers(1, &VBOF2);
    glGenBuffers(1, &EBOF2);
    glBindVertexArray(VAOF2);
    glBindBuffer(GL_ARRAY_BUFFER, VBOF2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fan_pivot), fan_pivot, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOF2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);


    unsigned int VBOF3, VAOF3, EBOF3;
    glGenVertexArrays(1, &VAOF3);
    glGenBuffers(1, &VBOF3);
    glGenBuffers(1, &EBOF3);
    glBindVertexArray(VAOF3);
    glBindBuffer(GL_ARRAY_BUFFER, VBOF3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fan_blade), fan_blade, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOF3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);
    

    unsigned int VBOS, VAOS, EBOS;
    glGenVertexArrays(1, &VAOS);
    glGenBuffers(1, &VBOS);
    glGenBuffers(1, &EBOS);
    glBindVertexArray(VAOS);
    glBindBuffer(GL_ARRAY_BUFFER, VBOS);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glass), glass, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOS);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // ---position attribute--
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    //---color attribute--
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    int i = 0;


    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();
        glm::mat4 model,rotate,scale;
        // pass projection matrix to shader (note that in this case it could change every frame)
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // ---camera/view transformation--
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        //***********************************************************************************************
        //------------------Floor------------------
        model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 0.1, 20);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOG);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //--------------Roof----------------------
        model = transforamtion(0, 5, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 0.1, 20);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOT);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //-----------------------Wall1 left --------------
        model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 10, 0.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //-----------------------Wall1 Right ---------------
        model = transforamtion(0, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 20, 10, 0.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //***************************   wall3  ***************************
         //-----------------------Wall3  --------------

        //2nd door er left side
        model = transforamtion(11.45, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 22.2, 10, 0.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOQ);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //2nd door er uporar part
        model = transforamtion(10, 3.34, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 4.8, 3.3, 0.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOQ);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //-----------------------2room er Wall3 Right ---------------
        model = transforamtion(10, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 25, 10, 0.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOQ);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //****************************************************************


        //------------------------door er sather Wall2--------------------
        //door er left side er door
        model = transforamtion(10, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, 10, 0.15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //Door er uporar wall
        model = transforamtion(10, 3.355, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, 3.3, 4.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //door er right side er wall
        model = transforamtion(10, 0, 2.1, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, 10, 15.8);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        ///*-----------------wall4 ------------------*/
        model = transforamtion(22.5, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, 10, 20);
        ourShader.setMat4("model", model);
        glBindVertexArray(VBOWY);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //------------------Floor2 for room2------------------
        model = transforamtion(10, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 25, 0.1, 20);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAODD);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        ////--------------Roof for room2----------------------
        model = transforamtion(10, 5, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 25, 0.1, 20);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOT);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //****************************************************************************************************
        
        //-----------Rak 1------------
        model = transforamtion(6.05, 1, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.2, .15, 2.12);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //-----------Rak 2------------
        model = transforamtion(6.05, .5, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.2, .15, 2.12);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //-----------Rak 3------------
        model = transforamtion(6.05, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.2, .15, 2.12);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //------------------tv---------------------
        model = transforamtion(5.60, 1.4, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -4.5, 2.75, 0.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //------------------Room2 tv---------------------
        model = transforamtion(10.1, 3, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .1, 4, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //TV stand
        model = transforamtion(10.1, 3, 5.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .1, -1, .1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //TV stand
        model = transforamtion(10.1, 3, 5.8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .1, -1, .1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //TV stand
        model = transforamtion(10.1, 3, 7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .1, -1, .1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //TV stand
        model = transforamtion(10.1, 3, 7.3, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .1, -1, .1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //-----------------TV Rakar Rak---------------
       model = transforamtion(10.1, 2.5, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .5, .2, 6);
       ourShader.setMat4("model", model);
       glBindVertexArray(VAOF2);
       glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //------------------DOOR----------------
        model = transforamtion(11.45, 0.1, 0.1, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 6.5, 4);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_LINE_LOOP, 12, GL_UNSIGNED_INT, 0);

        ////-----------DOOR er handle-----------------
        //model = transforamtion(9.9, 1.8, 2.0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -0.2, 0.7, -0.1);
        //ourShader.setMat4("model", model);
        //glBindVertexArray(VAOTV);
        //glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        ////-----------DOOR er handle-----------------
        //model = transforamtion(11.3, 1.8, 0.1, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -0.2, 0.7, -0.1);
        //ourShader.setMat4("model", model);
        //glBindVertexArray(VAOTV);
        //glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        //-----------------------sofa 1------------------
        //-----------------sofa 1 er boshar part---------------
        model = transforamtion(10, 0, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 1.5, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
 
        //-----------------sofa 1 er pichoner part---------------
        model = transforamtion(10, 0, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 4, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //-----------------sofa 1 er left er corner part---------------
        model = transforamtion(10, 0, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 3, -1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //-----------------sofa 1 er black cover part---------------
        model = transforamtion(10, 1.5, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 0.1, -1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(8.5, 0, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.1, 3.1, -1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //right er corner
        model = transforamtion(10, 0, 8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 3, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(10, 1.5, 8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 0.1, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(8.5, 0, 8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.1, 3.1, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //cover
        model = transforamtion(10, 0.75, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -3, 0.5, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        /*-----------------bed Room2*/
                //-----------------sofa 1 er boshar part---------------
        model = transforamtion(19.5, 0, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 6, 1.2, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //-----------------bed er pichoner part---------------
        model = transforamtion(22.5, 0, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 3, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //cover
        model = transforamtion(19.5, 0.60, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 5.2, 0.5, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //balish
        model = transforamtion(21.3, 0.60, 5.1, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1.5, 0.9, 2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOQ);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //balish
        model = transforamtion(21.3, 0.60, 6.8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1.5, 0.9, 2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOQ);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //manus
        model = transforamtion(19.5, 0.87, 5.8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .8, 1.3, 1.5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOT);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //gola
        model = transforamtion(19.7, 1.5, 6.1, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .2, 0.7, .2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //matha
        model = transforamtion(19.5, 1.7, 5.95, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .7, 0.5, .8);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //lag
        model = transforamtion(19.1, 0.0, 5.8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .2, 1.7, .2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //lag
        model = transforamtion(19.1, 0.0, 6.4, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .2, 1.7, .2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //hand
        model = transforamtion(19.1, 1.2, 6.4, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .8, .2, .2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //hand
        model = transforamtion(19.1, 1.2, 5.8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .8, .2, .2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //lag
        model = transforamtion(19.1, 0.84, 6.4, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .9, .2, .2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //lag
        model = transforamtion(19.1, 0.84, 5.8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .9, .2, .2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        /*------------------------- ROOM2 AC setup -------------------*/
        //AC
        model = transforamtion(22.5, 4, 6, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -5, 2, 6);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOAC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //------------------------paposh-----------------------   
        model = transforamtion(5.8, 0, 2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -8, 0.2, 8);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        

        //********************--Lamp--*******************
        model = transforamtion(8.5, 1.75,3.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1.2, 2.35, 1.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOLMP);
        glDrawElements(GL_TRIANGLES, 96, GL_UNSIGNED_INT, 0);

        //*------------------lamp stand---------------------*
        model = transforamtion(8.8, 0, 3.75, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .15, 4, .15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //*------------------lamp stand er nicar part---------------------*
        model = transforamtion(8.5, 0, 3.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1.2, 0.5, 1.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 96, GL_UNSIGNED_INT, 0);
        /*--------------Room2 Lamp-----------------*/
        //********************--Lamp--*******************
        model = transforamtion(21, 1.75, .5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1.2, 2, 1.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOLMP);
        glDrawElements(GL_TRIANGLES, 96, GL_UNSIGNED_INT, 0);

        //*------------------lamp stand---------------------*
        model = transforamtion(21.25, 1.0, .75, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .15, 2, .15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //*------------------lamp stand er nicar part---------------------*
        model = transforamtion(21, 1.0, .5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1.2, 0.2, 1.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 96, GL_UNSIGNED_INT, 0);
   
        
        
        //-------------sofa 2-----------------
        //-------------sofa2 boshar base-------
        model = transforamtion(6.6, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -9, 1.5, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //------------sofar black cover----------------
        model = transforamtion(6.6, 1.5, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 0.1, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        model = transforamtion(6.6, 0, 8.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 3.1, -.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //----------sofar right er corner--------------
        model = transforamtion(6.6, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 3, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //-------------sofar left er corner------------
        model = transforamtion(2.1, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 3, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //-------------sofar black cover------------
        model = transforamtion(2.1, 1.5, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 0.1, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(2.1, 0, 8.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -1, 3.1, -.1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //---------------sofar pichoner base--------------
        model = transforamtion(6.6, 0, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -9, 4, -1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //--------------1st sofa bed -----------------
        model = transforamtion(6.6, 0.75, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -9, 0.5, -3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        //*******************window*********************
        //----------------------pordar hanger--------------------------
        model = transforamtion(2.65, 4, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 8.4, 1, -.5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // -----------------window porson glass black--------------
        model = transforamtion(3, 1.5, 10, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 7, 5, -.15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        /*--------------ROOM2 ----------------*/
         //----------------------pordar hanger--------------------------
        model = transforamtion(15.65, 4, 0.3, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 8.4, 1, -.5);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // -----------------window porson glass black--------------
        model = transforamtion(16, 1.5, 0.1, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 7, 5, -.15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        /*--------------------------------------------------*/

        // ----------------**Table**--------------------
        model = transforamtion(5.8, 0.6, 6.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6, .8, 3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ----------------Tablem er samner left leg --------------------
        model = transforamtion(5.8, 0.0, 6.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.3, 1.2, .3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ----------------Tablem er samner right leg --------------------
        model = transforamtion(2.8, 0.0, 6.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, 1.2, .3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ----------------Table er pichoner left leg --------------------

        model = transforamtion(5.8, 0.0, 7.7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -.3, 1.2, -.3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ----------------Table er pichoner right leg --------------------
        model = transforamtion(2.8, 0.0, 7.7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, 1.2, -.3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ----------------Table er uporar part--------------------
        model = transforamtion(5.85, 1, 6.15, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.2, .4, 3.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ----------------**Room 2 Table**--------------------
        model = transforamtion(19.5, 0.6, 0.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 6, .6, 3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF2);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ----------------Tablem er samner left leg --------------------
        model = transforamtion(19.7, 0.0, 0.3, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, 1.2, .3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ----------------Tablem er samner right leg --------------------
        model = transforamtion(19.7, 0.0, 1.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, 1.2, .3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ----------------Table er pichoner left leg --------------------

        model = transforamtion(22.2, 0.0, 0.3, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, 1.2, -.3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ----------------Table er pichoner right leg --------------------
        model = transforamtion(22.2, 0.0, 1.5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, 1.2, -.3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // ----------------Table er uporar part--------------------
        model = transforamtion(19.5, .9, 0.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 6, .1, 3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        /*//////////////////////////////////////////////////////////////////////////////////////////*/

        //------------Font wallmat---------------------- 
        model = transforamtion(1.075, 1.65, 0.075, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2.2, 4.4, .05);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);



        // ----------------wallmat left--------------------
        model = transforamtion(1, 1.5, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2.5, 5,.15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        //------------------------------------------********************************------------------------------------
        // ----------------Fan--------------------
        // ----------------Fan er uporer part---------------
        model = transforamtion(5, 5, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, -.2, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        // ----------------Fan er hanger--------------------
        model = transforamtion(5.2125, 4.9, 5.2125, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .15, -1, .15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        // ----------------Fan er nicher part-----------------
        model = transforamtion(5, 4.4, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, -.33, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        /**********************Room3 Left**********************/
        model = transforamtion(10, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, 10, -10);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(10, 0, -5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 25, 10, 0.15);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(22.5, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, 10, -10);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOW1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //------------------Floor3 for room2------------------
        model = transforamtion(10, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 25, 0.1, -10);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAODD);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        ////--------------Roof for room3----------------------
        model = transforamtion(10, 5, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 25, 0.1, -10);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOT);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        // ----------------Table er uporar part room3--------------------
        model = transforamtion(17, 1, -4.9, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, -6.2, .4, 3.2);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(15, 1.2, -4.7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, 1.2, -.3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(15.5, 1.2, -4.7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 0.7, -.3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOC);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(15, 4.7, -4.7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 8, 1.2, -.3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(14.89, 4.7, -4.7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, 1.2, -.3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        model = transforamtion(19, 4.7, -4.7, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, .3, 1.2, -.3);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOTV);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);


        // ----------------Fan gurar Condation----------------
        Fan fan;
        ourShader = fan.local_rotation(ourShader, VAOF3, i);

        if (fan_turn)
            i -= 1;
        if (rotate_around)
           camera.ProcessKeyboard(Y_LEFT, deltaTime);
        //nicher 
        model = transforamtion(5, 4.225, 5, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, -.05, 1);
        ourShader.setMat4("model", model);
        glBindVertexArray(VAOF1);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // --------------------****************************************************------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    glDeleteVertexArrays(1, &VAOCA);
    glDeleteBuffers(1, &VBOCA);
    glDeleteBuffers(1, &EBOCA);

    glDeleteVertexArrays(1, &VAOT);
    glDeleteBuffers(1, &VBOT);
    glDeleteBuffers(1, &EBOT);

    glDeleteVertexArrays(1, &VAOG);
    glDeleteBuffers(1, &VBOG);
    glDeleteBuffers(1, &EBOG);

    glDeleteVertexArrays(1, &VAOW);
    glDeleteBuffers(1, &VBOW);
    glDeleteBuffers(1, &EBOW);

    glDeleteVertexArrays(1, &VAOW1);
    glDeleteBuffers(1, &VBOW1);
    glDeleteBuffers(1, &EBOW1);

    glDeleteVertexArrays(1, &VAOC);
    glDeleteBuffers(1, &VBOC);
    glDeleteBuffers(1, &EBOC);

    glDeleteVertexArrays(1, &VAOC2);
    glDeleteBuffers(1, &VBOC2);
    glDeleteBuffers(1, &EBOC2);

    glDeleteVertexArrays(1, &VAOTV);
    glDeleteBuffers(1, &VBOTV);
    glDeleteBuffers(1, &EBOTV);
    /************************************/
    glDeleteVertexArrays(1, &VAOQ);
    glDeleteBuffers(1, &VBOQ);
    glDeleteBuffers(1, &EBOQ);

    glDeleteVertexArrays(1, &VAOWY);
    glDeleteBuffers(1, &VBOWY);
    glDeleteBuffers(1, &EBOWY);

    glDeleteVertexArrays(1, &VAODD);
    glDeleteBuffers(1, &VBODD);
    glDeleteBuffers(1, &EBODD);

    glDeleteVertexArrays(1, &VAOAC);
    glDeleteBuffers(1, &VBOAC);
    glDeleteBuffers(1, &EBOAC);
    // -------------------------------

    glfwTerminate();
    return 0;
}

// ------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        if (!fan_turn) {
            fan_turn = true;
        }
        else {
            fan_turn = false;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!rotate_around) {
            rotate_around = true;
        }
        else {
            rotate_around = false;
        }
    
    }

}

// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}


// mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    // reversed  y-coordinates from bottom to top
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

//mouse scroll wheel scrolls
// ----------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
