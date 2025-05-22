
#include "opengl_backend.h"

#include <plog/Log.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OBJ-Loader-master/Source/OBJ_Loader.h"
#include "camera.hpp"
#include "engine.h"
#include "glm/glm.hpp"
#include "mesh.h"
#include "rendering/shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    // PLOGV << "Window resized to " << width << "x" << height << std::endl;
}
// Camera
Camera cam = Camera(glm::vec3(0, 0, 3));
bool moveCam = false;
// timing
float deltaTime = 0.0f;  // time between current frame and last frame
float lastFrame = 0.0f;

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (moveCam) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cam.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cam.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cam.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cam.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            cam.ProcessKeyboard(Camera_Movement::UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            cam.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;
    // TODO: not hardcoded
    static float lastX = 800 / 2.0f;
    static float lastY = 600 / 2.0f;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;  // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    if (moveCam)
        cam.ProcessMouseMovement(xoffset, yoffset);

    // std::cout << xpos << ",	" << ypos << std::endl;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  // Hide cursor
        moveCam = true;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  // Show cursor
        moveCam = false;
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (moveCam) cam.ProcessMouseScroll(yoffset);
}

bool bunny_loaded = false;

glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
glm::mat4 view;
glm::mat4 projection;

Shader* shader;

void v3d::rendering::OpenGlBackend::init() {
    if (m_initialized) {
        return;
    }
    PLOGI << "Initializing OpenGL" << std::endl;

    PLOGD << "Initializing GLAD" << std::endl;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        PLOGE << "Failed to initialize GLAD" << std::endl;
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glViewport(0, 0, m_window->getWidth(), m_window->getHeight());

    glfwSetFramebufferSizeCallback(m_window->getWindow(), framebuffer_size_callback);

    glfwSetCursorPosCallback(m_window->getWindow(), mouse_callback);
    glfwSetScrollCallback(m_window->getWindow(), scroll_callback);
    glfwSetMouseButtonCallback(m_window->getWindow(), mouse_button_callback);

    shader = new Shader("rcs/shaders/SimpleShader.glsl");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    PLOGI << "OpenGL initialized" << std::endl;
    m_initialized = true;
}

void v3d::rendering::OpenGlBackend::frame_update() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(m_window->getWindow());

    // Rotate over time
    const float radius = 5.f;
    const float frequency = 1.f;
    float camX = sin(glfwGetTime() * frequency) * radius;
    float camZ = cos(glfwGetTime() * frequency) * radius;

    view = cam.GetViewMatrix();
    projection = glm::perspective(glm::radians(cam.Zoom), 1.f * m_window->getWidth() / m_window->getHeight(), 0.1f, 100.0f);

    shader->bind();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVector("dye_color", glm::vec4(1, 1, 1, 1));

    for (auto renderTarget : m_renderTargets) {
        renderTarget->setUniforms(shader);
        renderTarget->renderElement();
    }

    glfwSwapBuffers(m_window->getWindow());
}

void v3d::rendering::OpenGlBackend::cleanup() {}

v3d::Mesh* v3d::rendering::OpenGlBackend::createMesh(std::string filePath) {
    // Initialize Loader
    objl::Loader obj_loader;

    // Load .obj File
    PLOGD << "Loading model " << filePath << "...\n";
    bool loadout = obj_loader.LoadFile(filePath);

    if (loadout) {
        MeshOpenGL* mesh = new v3d::MeshOpenGL(obj_loader.LoadedMeshes[0]);
        m_meshList.push_back(mesh);
        return mesh;
    } else {
        // Output Error
        PLOGD << "Failed to Load File " << filePath << ". May have failed to find it or it was not an .obj file.\n";
        return nullptr;
    }
}