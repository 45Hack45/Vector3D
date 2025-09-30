
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

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    // PLOGV << "Window resized to " << width << "x" << height << std::endl;
}
// Camera
Camera cam = Camera(glm::vec3(0, 2.5, 10));
bool moveCam = false;
// timing
float deltaTime = 0.0f;  // time between current frame and last frame
float lastFrame = 0.0f;

void processInput(GLFWwindow *window) {
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
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            cam.ProcessKeyboard(Camera_Movement::UP, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            cam.ProcessKeyboard(Camera_Movement::DOWN, deltaTime);
    }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
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
    float yoffset =
        lastY - ypos;  // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    if (moveCam) cam.ProcessMouseMovement(xoffset, yoffset);

    // std::cout << xpos << ",	" << ypos << std::endl;
}

void mouse_button_callback(GLFWwindow *window, int button, int action,
                           int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR,
                         GLFW_CURSOR_DISABLED);  // Hide cursor
        moveCam = true;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        glfwSetInputMode(window, GLFW_CURSOR,
                         GLFW_CURSOR_NORMAL);  // Show cursor
        moveCam = false;
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    if (moveCam) cam.ProcessMouseScroll(yoffset);
}

bool bunny_loaded = false;

glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f),
                              glm::vec3(1.0f, 0.0f, 0.0f));
glm::mat4 view;
glm::mat4 projection;

Shader *shader;
Shader *shaderGrid;

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

    glfwSetFramebufferSizeCallback(m_window->getWindow(),
                                   framebuffer_size_callback);

    glfwSetCursorPosCallback(m_window->getWindow(), mouse_callback);
    glfwSetScrollCallback(m_window->getWindow(), scroll_callback);
    glfwSetMouseButtonCallback(m_window->getWindow(), mouse_button_callback);

    shader = new Shader("resources/shaders/SimpleShader.glsl");
    shaderGrid = new Shader("resources/shaders/GridShader.glsl");

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    PLOGI << "OpenGL initialized" << std::endl;
    m_initialized = true;

    initPrimitives();
}

namespace v3d {
namespace rendering {
GLuint createGridVAO(int halfSize, float spacing, GLsizei &vertexCount) {
    std::vector<float> vertices;

    for (int i = -halfSize; i <= halfSize; i++) {
        float coord = i * spacing;

        // line parallel to X axis
        vertices.push_back(-halfSize * spacing);
        vertices.push_back(0.0f);
        vertices.push_back(coord);

        vertices.push_back(halfSize * spacing);
        vertices.push_back(0.0f);
        vertices.push_back(coord);

        // line parallel to Z axis
        vertices.push_back(coord);
        vertices.push_back(0.0f);
        vertices.push_back(-halfSize * spacing);

        vertices.push_back(coord);
        vertices.push_back(0.0f);
        vertices.push_back(halfSize * spacing);
    }

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // number of vertices = total floats / 3
    vertexCount = static_cast<GLsizei>(vertices.size() / 3);

    return VAO;
}

GLsizei gridVertexCount;
GLuint gridVAO;

void drawGrid() {
    float size = 1;
    glm::mat4 pmodel = glm::translate(glm::mat4(1.0f), glm::vec3());
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(pmodel)));
    pmodel = glm::scale(pmodel, size * glm::vec3(1, 1, 1));

    shader->setMat4("model", pmodel);
    shader->setMat3("normalMatrix", normalMatrix);

    // Draw grid
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, gridVertexCount);
}
}  // namespace rendering

}  // namespace v3d

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
    projection = glm::perspective(
        glm::radians(cam.Zoom),
        1.f * m_window->getWidth() / m_window->getHeight(), 0.1f, 100.0f);

    shader->bind();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVector("dye_color", glm::vec4(1, 1, 1, 1));

    drawGrid();

    for (auto renderTarget : m_renderTargets) {
        renderTarget->setUniforms(shader);
        renderTarget->renderElement();
    }
}

void v3d::rendering::OpenGlBackend::present_frame() {
    glfwSwapBuffers(m_window->getWindow());
}
void v3d::rendering::OpenGlBackend::pre_draw_gizmos_hook() {
    glDisable(GL_DEPTH_TEST);
}
void v3d::rendering::OpenGlBackend::post_draw_gizmos_hook() {
    glEnable(GL_DEPTH_TEST);
}

void v3d::rendering::OpenGlBackend::draw_primitive_cube(glm::vec3 position,
                                                        glm::vec3 scale,
                                                        glm::vec4 color,
                                                        bool wireframe) {
    view = cam.GetViewMatrix();
    projection = glm::perspective(
        glm::radians(cam.Zoom),
        1.f * m_window->getWidth() / m_window->getHeight(), 0.1f, 100.0f);

    shader->bind();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVector("dye_color", color);

    glm::mat4 pmodel = glm::translate(glm::mat4(1.0f), position);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(pmodel)));
    pmodel = glm::scale(pmodel, scale);

    shader->setMat4("model", pmodel);
    shader->setMat3("normalMatrix", normalMatrix);

    if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_primitives.m_cube->draw();
    if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void v3d::rendering::OpenGlBackend::draw_primitive_sphere(glm::vec3 position,
                                                          glm::vec3 scale,
                                                          glm::vec4 color,
                                                          bool wireframe) {
    view = cam.GetViewMatrix();
    projection = glm::perspective(
        glm::radians(cam.Zoom),
        1.f * m_window->getWidth() / m_window->getHeight(), 0.1f, 100.0f);

    shader->bind();
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVector("dye_color", color);

    glm::mat4 pmodel = glm::translate(glm::mat4(1.0f), position);
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(pmodel)));
    pmodel = glm::scale(pmodel, scale);

    shader->setMat4("model", pmodel);
    shader->setMat3("normalMatrix", normalMatrix);

    if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    m_primitives.m_sphere->draw();
    if (wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void v3d::rendering::OpenGlBackend::cleanup() {}

v3d::Mesh *v3d::rendering::OpenGlBackend::createMesh(std::string filePath) {
    // Initialize Loader
    objl::Loader obj_loader;

    // Load .obj File
    PLOGD << "Loading model " << filePath << "...\n";
    bool loadout = obj_loader.LoadFile(filePath);

    if (loadout) {
        // FIXME: use smart ptr? improve lifetime handeling
        MeshOpenGL *mesh = new v3d::MeshOpenGL(obj_loader.LoadedMeshes[0]);
        m_meshList.push_back(mesh);
        return mesh;
    } else {
        // Output Error
        PLOGD << "Failed to Load File " << filePath
              << ". May have failed to find it or it was not an .obj file.\n";
        return nullptr;
    }
}

void v3d::rendering::OpenGlBackend::initPrimitives() {
    PLOGD << "Loading primitives\n";
    m_primitives.m_cube = createMesh("resources/primitives/3D/cube.obj");
    m_primitives.m_sphere = createMesh("resources/primitives/3D/sphere.obj");
    gridVAO = createGridVAO(100, 1.f, gridVertexCount);
    PLOGD << "\n";
}