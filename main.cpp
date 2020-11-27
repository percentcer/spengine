#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "shader.h"
#include "texture.h"

#define CGLTF_IMPLEMENTATION
#include "stb/cgltf.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

constexpr unsigned int NUM_VERT_ELEMENTS = 5;

// camera stuff
glm::vec3 cameraPos = glm::vec3(0.f, 0.f, 3.f);
glm::vec3 cameraFront = glm::vec3(0.f, 0.f, -1.f);
glm::vec3 cameraUp = glm::vec3(0.f, 1.f, 0.f);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Spengine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create window " << glfwGetVersionString() << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD, sad!" << std::endl;
        return -1;
    }

    glClearColor(.2f, .6f, .8f, 1.f);
    glEnable(GL_DEPTH_TEST);

    //                     // position           color             uv
    // float vertices[] = { .5f,  .5f, 0.f,    1.f, 0.f, 0.f,    1.f, 1.f,
    //                      .5f, -.5f, 0.f,    0.f, 1.f, 0.f,    1.f, 0.f,
    //                     -.5f, -.5f, 0.f,    0.f, 0.f, 1.f,    0.f, 0.f,
    //                     -.5f,  .5f, 0.f,    1.f, 0.f, 1.f,    0.f, 1.f,
    //                     };

    // box verts
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, 2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, 3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, 2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, 1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)};

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3};
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, NUM_VERT_ELEMENTS * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // (old, was vert colors)
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, NUM_VERT_ELEMENTS * sizeof(float), (void*)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, NUM_VERT_ELEMENTS * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // create the wisp gltf
    unsigned int VAOWisp;
    glGenVertexArrays(1, &VAOWisp);
    glBindVertexArray(VAOWisp);

    unsigned int VBOWisp;
    glGenBuffers(1, &VBOWisp);
    glBindBuffer(GL_ARRAY_BUFFER, VBOWisp);

    unsigned int EBOWisp;
    glGenBuffers(1, &EBOWisp);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOWisp);

    cgltf_options options = {};
    cgltf_data* data = NULL;
    cgltf_result result = cgltf_parse_file(&options, "res/wisp.gltf", &data);
    if (result == cgltf_result_success) {
        cgltf_result result = cgltf_load_buffers(&options, data, "res/wisp.gltf");
        if (result != cgltf_result_success) {
            std::cout << "failed to load gltf buffers\n" << std::endl;
        }
        cgltf_mesh& mesh = data->meshes[0];
        cgltf_primitive& prim = mesh.primitives[0];

        cgltf_accessor positions = data->accessors[0];
        cgltf_accessor uvs = data->accessors[2];

        glBufferData(GL_ARRAY_BUFFER, data->buffers[0].size, data->buffers[0].data, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, prim.indices->buffer_view->size, (void*)(((char*)data->buffers[0].data) + prim.indices->buffer_view->offset), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, positions.stride, (void *)positions.buffer_view->offset);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, uvs.stride, (void *)uvs.buffer_view->offset);
        glEnableVertexAttribArray(2);
    } else {
        std::cout << "failed to load wisp.gltf" << std::endl;
    }

    textures::init();
    unsigned int tex0 = textures::genTex("res/face.jpg");
    unsigned int tex1 = textures::genTex("res/debug.jpg");

    Shader shader{"res/shaders/main.vert", "res/shaders/main.frag"};

    float res[] = {SCR_WIDTH, SCR_HEIGHT};
    shader.activate();
    shader.setInt("tex0", 0);
    shader.setInt("tex1", 1);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float timeValue = glfwGetTime();
        float scalt = (sin(timeValue) * 0.5f) + .5f;
        shader.activate();
        shader.setFloat("scaling", scalt);
        shader.setFloat("iTime", timeValue);
        shader.setFloat("xOffset", 0.f); //sin(timeValue));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1);

        // glBindVertexArray(VAO);
        glBindVertexArray(VAOWisp);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("view", glm::value_ptr(view));

        glm::mat4 proj;
        proj = glm::perspective(glm::radians(45.f), 800.f / 600.f, 0.1f, 1000.f);
        shader.setMat4("proj", glm::value_ptr(proj));

        for (int i = 0; i < 10; i++)
        {
            glm::mat4 local = glm::mat4(1.0f);
            local = glm::rotate(local, timeValue * float(i+.1f) * .2f, glm::vec3(0.45f, 0.45f, .0f));
            shader.setMat4("local", glm::value_ptr(local));

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            shader.setMat4("model", glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, 62424, GL_UNSIGNED_SHORT, NULL);
            // glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    
    const float camSpeed = .1f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += camSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
    {
        cameraPos += -glm::cross(cameraFront, cameraUp) * camSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
    {
        cameraPos += -camSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
    {
        cameraPos += glm::cross(cameraFront, cameraUp) * camSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) 
    {
        cameraPos += camSpeed * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) 
    {
        cameraPos += -camSpeed * cameraUp;
    }
}