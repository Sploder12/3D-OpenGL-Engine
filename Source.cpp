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
    camera* cam = renderer::getCamera();
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

    xoffset *= cam->sensitivity;
    yoffset *= cam->sensitivity;

    cam->moveYaw(xoffset);
    cam->movePitch(yoffset);

    glm::vec3 direction;
    direction.x = cos(float(glm::radians(cam->getYaw()))) * cos(float(glm::radians(cam->getPitch())));
    direction.y = sin(float(glm::radians(cam->getPitch())));
    direction.z = sin(float(glm::radians(cam->getYaw()))) * cos(float(glm::radians(cam->getPitch())));
    cam->front = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    renderer::getCamera()->moveFov(-yoffset);
}

void processInput(GLFWwindow* window) //Processes user input
{
    camera* cam = renderer::getCamera();
    float cameraSpeed = cam->speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam->pos += cameraSpeed * cam->front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam->pos -= cameraSpeed * cam->front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam->pos -= glm::normalize(glm::cross(cam->front, cam->up)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam->pos += glm::normalize(glm::cross(cam->front, cam->up)) * cameraSpeed;
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

    camera cam = camera(glm::vec3(0.0, 0.0, 0.0));
    renderer::setCamera(&cam);
    
    float vertices[] = {
        // positions        
         0.5f, -0.5f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f    // top 
    };

    float colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    };

    renderer::Basic triangle(vertices, colors, 3, &shaders->at("basicEX"));
    triangle.addToActive("basicEX1");

    renderer::Basic triangle2(vertices, colors, 3, &shaders->at("basicEX"));
    triangle2.addToActive("basicEX2");
    triangle2.scale(glm::vec3(0.5f, 0.5f, 0.5f));
    triangle2.translate(glm::vec3(0.0, -0.5f, 0.0f));
    
    
    float vertices2[] = {
        // positions      
        0.5f,  0.5f, 0.0f,    // top right
        0.5f, -0.5f, 0.0f,    // bottom right
       -0.5f,  0.5f, 0.0f,    // top left 

        0.5f, -0.5f, 0.0f,    // bottom right
       -0.5f, -0.5f, 0.0f,    // bottom left
       -0.5f,  0.5f, 0.0f     // top left
    };

    float colors2[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,

        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 0.0f
    };

    float texcord[] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,

        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    
    renderer::texParam2D params = renderer::texParam2D(GL_REPEAT, GL_REPEAT, GL_LINEAR, GL_LINEAR);
    renderer::texture2D texture = renderer::texture2D(&params, "container.jpg");

    renderer::BasicTextured crate(vertices2, colors2, texcord, 6, &shaders->at("basicTexEX"), &texture);
    crate.addToActive("basicTexEX1");
    crate.translate(glm::vec3(-0.5f, 0.5f, 0.0f));
    crate.scale(glm::vec3(0.5f, 0.5f, 0.5f));
    
    float vertices3[] = {
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f, 
         0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f,  

        -0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f
    };

    float vertices3col[] = {
        0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f,
         0.5f,  0.5f, 0.5f,
         0.5f,  0.5f, 0.5f,
        0.5f,  0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        0.5f, 0.5f,  0.5f,
         0.75f, 0.75f,  0.5f,
         0.5f,  0.75f,  0.5f,
         0.5f,  0.5f,  0.75f,
        0.5f,  0.5f,  0.75f,
        0.5f, 0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

         0.75f,  0.5f,  0.5f,
         0.5f,  0.5f, 0.75f,
         0.5f, 0.75f, 0.5f,
         0.5f, 0.75f, 0.5f,
         0.5f, 0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.75f,
         0.75f, 0.75f,  0.5f,
         0.5f, 0.5f,  0.5f,
        0.5f, 0.5f,  0.5f,
        0.5f, 0.75f, 0.5f,

        0.75f,  0.5f, 0.5f,
         0.75f,  0.5f, 0.5f,
         0.75f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        0.5f,  0.75f,  0.5f,
        0.5f,  0.5f, 0.75f
    };

    renderer::PointLight light(vertices3, 36, &shaders->at("pointLight"), glm::vec3(1.0f, 0.9f, 0.0f), glm::vec3(0.0f,0.0f,0.0f));
    light.addToActive("light");
    light.translate(glm::vec3(1.1f, 0.0f, 1.1f));
    light.scale(glm::vec3(0.1f,0.1f,0.1f));
    

    renderer::Basic cube(vertices3, vertices3, 36, &shaders->at("basicEX"));
    cube.addToActive("tube");
    cube.scale(glm::vec3(0.5f, 0.5f, 0.5f));
    cube.translate(glm::vec3(0.0f, 0.0f, 1.0f));

    //glfwSwapInterval(1); //this is Vsync
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

        cube.rotate(0.02f, glm::vec3(0.0f, 1.0f, 0.0f));


        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}