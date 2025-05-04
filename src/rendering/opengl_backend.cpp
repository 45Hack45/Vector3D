
#include <plog/Log.h>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl_backend.h"
#include "engine.h"

#include "shader.h"

#include "camera.hpp"

#include "OBJ-Loader-master/Source/OBJ_Loader.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    // PLOGV << "Window resized to " << width << "x" << height << std::endl;
}
//Camera
Camera cam = Camera(glm::vec3(0, 0, 3));
bool moveCam = false;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void processInput(GLFWwindow* window)
{
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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	static bool firstMouse = true;
    //TODO: not hardcoded
	static float lastX = 800 / 2.0f;
	static float lastY = 600 / 2.0f;

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	if (moveCam)
		cam.ProcessMouseMovement(xoffset, yoffset);

	//std::cout << xpos << ",	" << ypos << std::endl;
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//Hide cursor
		moveCam = true;
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);//Show cursor
		moveCam = false;
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (moveCam) cam.ProcessMouseScroll(yoffset);
}

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

// const char *vertexShaderSource = "#version 330 core\n"
//     "layout (location = 0) in vec3 aPos;\n"
//     "void main()\n"
//     "{\n"
//     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//     "}\0";

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
    "}\0\n";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

unsigned int VBO, VAO, EBO;
unsigned int shaderProgram;

class Model
{
private:
    
public:
    Model();
    ~Model();
};

class Mesh
{
private:
unsigned int m_VBO, m_VAO, m_EBO;
size_t m_numIndices;

public:
    Mesh(objl::Mesh mesh){
        // Send vertex data to GPU

        // Generate buffers
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindVertexArray(m_VAO);

        // Load data into vertex/index buffers
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh.Vertices.size() * sizeof(objl::Vertex), mesh.Vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        m_numIndices = mesh.Indices.size();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numIndices * sizeof(unsigned int), mesh.Indices.data(), GL_STATIC_DRAW);


        // Set the vertex attribute pointers
        
        // Vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(objl::Vertex), (void*)0);

        glBindVertexArray(0);//unbind VAO

        PLOGI << "Buffer handles: " << m_VBO << "; " << m_VAO << "; " << m_EBO << "\n";
    };
    ~Mesh(){
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    };

    void draw(){
        // draw mesh
        glBindVertexArray(m_VAO);
        glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};

Mesh *bunny_mesh;
bool bunny_loaded = false;

glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f)); 
glm::mat4 view;
glm::mat4 projection;

Shader *shader;

void v3d::rendering::OpenGlBackend::init() {
    if (m_initialized) {
        return;
    }
    PLOGI << "Initializing OpenGL" << std::endl;
    
    PLOGD << "Initializing GLAD" << std::endl;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
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

    PLOGI << "OpenGL initialized" << std::endl;
    m_initialized = true;

    
    // Test load and rander obj
    // Initialize Loader
	objl::Loader obj_loader;

    // Load .obj File
    std::string filepath = "EngineProject/stanford-bunny.obj";
    PLOGD << filepath << "\n";
	bool loadout = obj_loader.LoadFile(filepath);

    if (loadout){
        bunny_mesh = new Mesh(obj_loader.LoadedMeshes[0]);
        bunny_loaded = true;
    } else {
        // Output Error
		PLOGD << "Failed to Load File. May have failed to find it or it was not an .obj file.\n";
    }
}

void v3d::rendering::OpenGlBackend::frame_update() {

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // // draw our first triangle
    // glUseProgram(shaderProgram);
    // glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    // //glDrawArrays(GL_TRIANGLES, 0, 6);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(m_window->getWindow());

    //Rotate over time
    const float radius = 5.f;
    const float frequency = 1.f;
    float camX = sin(glfwGetTime() * frequency) * radius;
    float camZ = cos(glfwGetTime() * frequency) * radius;


    view = cam.GetViewMatrix();
    projection = glm::perspective(glm::radians(cam.Zoom), 1.f*m_window->getWidth()/m_window->getHeight(), 0.1f, 100.0f);

    shader->bind();
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setVector("dye_color", glm::vec4(0, camX, camZ, 1));


    if (bunny_loaded) bunny_mesh->draw();

    glfwSwapBuffers(m_window->getWindow());
}

void v3d::rendering::OpenGlBackend::cleanup() {

    if (bunny_loaded) delete bunny_mesh;

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
}
