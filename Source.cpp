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
    
    float vertices[] = {
        // positions         // colors         
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f, // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f  // top 
    };
    
    renderer::Basic triangle(vertices, 3, 6, &shaders->at("basicEX"));
    triangle.addToActive("basicEX1");

    renderer::Basic triangle2(vertices, 3, 6, &shaders->at("basicEX"));
    triangle2.addToActive("basicEX2");
    triangle2.scale(glm::vec3(0.5f, 0.5f, 0.5f));
    triangle2.translate(glm::vec3(0.0, -0.5f, 0.0f));
    
    
    float vertices2[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f,  // top left 

        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
       -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
       -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    
    renderer::texParam2D params = renderer::texParam2D(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
    renderer::texture2D texture = renderer::texture2D(&params, "container.jpg");

    renderer::BasicTextured crate(vertices2, 6, 8, &shaders->at("basicTexEX"), &texture);
    crate.addToActive("basicTexEX1");
    crate.translate(glm::vec3(-0.5f, 0.5f, 0.0f));
    crate.scale(glm::vec3(0.5f, 0.5f, 0.5f));

    glfwSwapInterval(0); //this is Vsync

    double time = glfwGetTime();
    double prevtime = time;
    int fps = 0;
    //doClear(false);
    while (!glfwWindowShouldClose(window)) //main loop of the program
    {
        time = glfwGetTime();
        processInput(window);
        
        if (time - prevtime >= 1)
        {
            std::cout << fps;
            fps = 0;
            prevtime++;
        }

        render(window);
        triangle2.rotate(-2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        triangle.rotate(2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        crate.rotate(2.0f, glm::vec3(0.0f, 0.0f, 1.0f));

        glfwSwapBuffers(window);
        fps++;

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}