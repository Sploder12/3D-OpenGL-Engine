#include "Render.h"

#define TITLE "SploderEngine"

bool firstMouse = true;
double lastX = 800.0 / 2.0;
double lastY = 600.0 / 2.0;

double currentTime = glfwGetTime();
double deltaTime = 0.0;	// Time between current frame and last frame
double lastFrame = 0.0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) //this is for window resizing (gets called when window is resized)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    double sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    renderer::setYaw(renderer::getYaw() + xoffset);
    renderer::setPitch(renderer::getPitch() + yoffset);

    if (renderer::getPitch() > 89.0f)
        renderer::setPitch(89.0f);
    if (renderer::getPitch() < -89.0f)
        renderer::setPitch(-89.0f);

    glm::vec3 direction;
    direction.x = cos(glm::radians(renderer::getYaw())) * cos(glm::radians(renderer::getPitch()));
    direction.y = sin(glm::radians(renderer::getPitch()));
    direction.z = sin(glm::radians(renderer::getYaw())) * cos(glm::radians(renderer::getPitch()));
    renderer::setCameraFront(glm::normalize(direction));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    renderer::setFov(renderer::getFov() - (float)yoffset);
    if (renderer::getFov() < 1.0f)
        renderer::setFov(1.0f);
    if (renderer::getFov() > 45.0f)
        renderer::setFov(45.0f);
}

void processInput(GLFWwindow* window) //Processes user input
{
    float cameraSpeed = 2.5 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        renderer::setCameraPos(renderer::getCameraPos() + cameraSpeed * renderer::getCameraFront());
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        renderer::setCameraPos(renderer::getCameraPos() - cameraSpeed * renderer::getCameraFront());
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        renderer::setCameraPos(renderer::getCameraPos() - glm::normalize(glm::cross(renderer::getCameraFront(), renderer::getCameraUp())) * cameraSpeed);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        renderer::setCameraPos(renderer::getCameraPos() + glm::normalize(glm::cross(renderer::getCameraFront(), renderer::getCameraUp())) * cameraSpeed);
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
    glEnable(GL_DEPTH_TEST);

    //doClear(false);
    while (!glfwWindowShouldClose(window)) //main loop of the program
    {
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
        processInput(window);

        render(window);
        triangle2.rotate(-0.02f, glm::vec3(1.0f, 0.0f, 0.0f));
        triangle.rotate(0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
        crate.rotate(0.02f, glm::vec3(0.0f, 0.0f, 1.0f));

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}