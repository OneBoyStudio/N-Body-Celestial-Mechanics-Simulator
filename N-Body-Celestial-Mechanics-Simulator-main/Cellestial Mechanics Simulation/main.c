#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "simulation.h"
#include "frames.h"

#define numBodies 1000

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char* fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
    "}\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
    glViewport(0, 0, width, height);
}

void inputProcess(GLFWwindow* window)
{
    //handle inputs and that
}

int main(void)
{
    stellarBody bodies[numBodies] = { {0} };
    float vertices[numBodies * 3] = { {0} };

    for (int i = 0; i < numBodies; i++)
    {
        instantiateBodies(bodies, numBodies);
        vertices[(i * 3)] = bodies[i].posx;
        vertices[(i * 3) + 1] = bodies[i].posy;
        //printf("x: %f, y: %f\n", vertices[i * 3], vertices[(i * 3) + 1]);
    }

    unsigned int VBO;

    unsigned int VAO;

    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int WINDOW_WIDTH = 1000;
    int WINDOW_HEIGHT = 1000;

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "CellMechanics", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(2);

    float xpos = 0.0f;

    int duration = 5;
    int i = 0;

    while (!glfwWindowShouldClose(window))
    {
        i++;

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glDrawArrays(GL_POINTS, 0, numBodies);

        printf(".\n");
        planetarySimulation(bodies, numBodies, 6.67408E-11, 25000.0f, 5.0f, 0.5, i - 1);

        for (int k = 0; k < numBodies; k++)
        {
            vertices[(k * 3)] = bodies[k].posx;
            vertices[(k * 3) + 1] = bodies[k].posy;
            //printf("x: %f, y: %f\n", bodies[k].posx * 1000, bodies[k].posy * 1000);
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glfwSwapBuffers(window);

        glfwPollEvents();

        printf("%d", i);

        if (i == duration * 30) {

            makeVideo(30, "out");
            exit(0);
        }
    }

    glfwTerminate();
    return 0;
}