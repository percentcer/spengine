#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

constexpr unsigned int NUM_VERT_ELEMENTS = 8;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create window " << glfwGetVersionString() << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD, sad!" << std::endl;
        return -1;
    }

    glClearColor(.2f, .6f, .8f, 1.f);

                        // position           color             uv
    float vertices[] = { .5f,  .5f, 0.f,    1.f, 0.f, 0.f,    1.f, 1.f,
                         .5f, -.5f, 0.f,    0.f, 1.f, 0.f,    1.f, 0.f,
                        -.5f, -.5f, 0.f,    0.f, 0.f, 1.f,    0.f, 0.f,
                        -.5f,  .5f, 0.f,    1.f, 0.f, 1.f,    0.f, 1.f,
                        };
    unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // set up a texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int width, height, channelCount;
    unsigned char *data = stbi_load("res/wall.jpg", &width, &height, &channelCount, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }    
    stbi_image_free(data);

    Shader shader{"res/shaders/main.vert", "res/shaders/main.frag"};

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NUM_VERT_ELEMENTS * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, NUM_VERT_ELEMENTS * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, NUM_VERT_ELEMENTS * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
        
    float res[] = {SCR_WIDTH, SCR_HEIGHT};
    shader.activate();

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        float timeValue = glfwGetTime();
        float scalt = (sin(timeValue) / 2.0f) + 1.f;
        shader.activate();
        shader.setFloat("scaling", scalt);
        shader.setFloat("iTime", timeValue);
        shader.setFloat("xOffset", sin(timeValue));
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}