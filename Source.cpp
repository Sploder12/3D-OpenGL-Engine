#include "Render.h"

#define SWIDTH 800
#define SHEIGHT 600
#define TITLE "SploderEngine"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) //this is for window resizing (gets called when window is resized)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) //Processes user input
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

std::map<std::string, Shader>* shaders;

//This is our entry point and initialization of GLFW3 & openGL
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SWIDTH, SHEIGHT, TITLE, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SWIDTH, SHEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    compileShaders();
    shaders = getShaders();

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    
    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
    };
    
    renderobject triangle(vertices, 3, 6, VAO, VBO, &shaders->at("example"));
    triangle.addToRendering("example");

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glfwSwapInterval(0); //this is Vsync

    double time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) //main loop of the program
    {
        time = glfwGetTime();
        processInput(window);
        
        render(window, VAO, VBO);
        triangle.rotate(0.01f, glm::vec3(0.0f, 1.0f, 0.0f));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}