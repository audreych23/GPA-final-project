#include "src/RenderWidgets/RenderingOrderExp.h"

// the include of "glad" must be before the "glfw"
#include <GLFW\glfw3.h>
#include <iostream>
#include <map>

#define IMGUI_ENABLED
#define VSYNC_DISABLED

#ifdef IMGUI_ENABLED
#include <imgui\imgui.h>
#include <imgui\imgui_impl_glfw.h>
#include <imgui\imgui_impl_opengl3.h>
#endif // IMGUI_ENABLED


#pragma comment (lib, "lib-vc2015\\glfw3.lib")
#pragma comment(lib, "assimp-vc141-mt.lib")

const int FRAME_WIDTH = 1344;
const int FRAME_HEIGHT = 756;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void cursorPosCallback(GLFWwindow* window, double x, double y);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

bool initializeGL();
void resizeGL(GLFWwindow* window, int w, int h);
void paintGL();
void updateState();

INANOA::RenderingOrderExp* m_renderWidget = nullptr;

double PROGRAM_FPS = 0.0;
double FRAME_MS = 0.0;

std::map<int, int> key_mapping;

bool first_mouse = true;
bool is_mouse_pressed = false;

float last_x = FRAME_WIDTH / 2.0f;
float last_y = FRAME_HEIGHT / 2.0f;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(FRAME_WIDTH, FRAME_HEIGHT, "rendering", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "failed to create GLFW window\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// load OpenGL function pointer
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetKeyCallback(window, keyCallback);
	glfwSetScrollCallback(window, mouseScrollCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetFramebufferSizeCallback(window, resizeGL);

	if (initializeGL() == false) {
		glfwTerminate();
		return 0;
	}

#ifdef IMGUI_ENABLED
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");
#endif // IMGUI_ENABLED


#ifdef VSYNC_DISABLED
	glfwSwapInterval(0);
#endif // VSYNC_DISABLED

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	double previousTimeStamp = glfwGetTime();
	int frameCounter = 0;
	while (!glfwWindowShouldClose(window)) {
		// ===============================
		const double timeStamp = glfwGetTime();
		const double deltaTime = timeStamp - previousTimeStamp;
		if (deltaTime >= 1.0) {
			PROGRAM_FPS = frameCounter / deltaTime;

			// reset
			frameCounter = 0;
			previousTimeStamp = timeStamp;
		}
		frameCounter = frameCounter + 1;
		// ===============================

		glfwPollEvents();

		updateState();
		paintGL();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}



bool initializeGL() {

	// initialize render widget
	stbi_set_flip_vertically_on_load(true);
	m_renderWidget = new INANOA::RenderingOrderExp();
	if (m_renderWidget->init(FRAME_WIDTH, FRAME_HEIGHT) == false) {
		return false;
	}

	return true;
}
void resizeGL(GLFWwindow* window, int w, int h) {
	m_renderWidget->resize(w, h);
}
void updateState() {
	m_renderWidget->update();
}
void paintGL() {

#ifdef IMGUI_ENABLED
	//ImGui_ImplOpenGL3_NewFrame();
	//ImGui_ImplGlfw_NewFrame();
	//ImGui::NewFrame();

#endif // IMGUI_ENABLED

	m_renderWidget->render();

	static char fpsBuf[] = "fps: 000000000.000000000";
	static char msBuf[] = "ms: 000000000.000000000";
	sprintf_s(fpsBuf + 5, 16, "%.5f", PROGRAM_FPS);
	sprintf_s(msBuf + 4, 16, "%.5f", (1000.0 / PROGRAM_FPS));

//#ifdef IMGUI_ENABLED	
//	ImGui::Begin("Information");
//	ImGui::Text(fpsBuf);
//	ImGui::Text(msBuf);
//	ImGui::End();
//	ImGui::Render();
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//#endif // IMGUI_ENABLED
}

////////////////////////////////////////////////
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	double x, y;
	glfwGetCursorPos(window, &x, &y);  // Get current cursor position
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		//is_first_mouse = true;
		//is_mouse_pressed = true;
		is_mouse_pressed = true;
	}

	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		/*is_first_mouse = false;
		is_mouse_pressed = false;
		if (m_renderWidget != nullptr) m_renderWidget->setMouseOffset(0.0f, 0.0f, false);*/
		first_mouse = true;
		is_mouse_pressed = false;
		if (m_renderWidget != nullptr) m_renderWidget->setMouseOffset(0.0f, 0.0f);
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {}

	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {}
}
void cursorPosCallback(GLFWwindow* window, double x, double y) {
	float xpos = static_cast<float>(x);
	float ypos = static_cast<float>(y);

	if (first_mouse)
	{
		last_x = xpos;
		last_y = ypos;
		first_mouse = false;
	}
	else {

		float xoffset = xpos - last_x;
		float yoffset = last_y - ypos; // reversed since y-coordinates go from bottom to top

		last_x = xpos;
		last_y = ypos;
		//processMouseMovement(xoffset, yoffset);
		if (m_renderWidget != nullptr && is_mouse_pressed) m_renderWidget->setMouseOffset(xoffset, yoffset);

	}
	//if (!is_mouse_pressed) return;
	//if (is_first_mouse)
	//{
	//	last_x = x;
	//	last_y = y;
	//	is_first_mouse = false;
	//}

	//float x_offset = x - last_x;
	//float y_offset = last_y - y;  // Reversed for typical mouse look
	//
	//last_x = x;
	//last_y = y;

	//if (m_renderWidget != nullptr) m_renderWidget->setMouseOffset(x_offset, y_offset, true);
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		key_mapping[GLFW_KEY_W] = 1;
	}
	else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
		key_mapping[GLFW_KEY_W] = 0;
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
		key_mapping[GLFW_KEY_A] = 1;
	}
	else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
		key_mapping[GLFW_KEY_A] = 0;
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
		key_mapping[GLFW_KEY_S] = 1;
	}
	else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
		key_mapping[GLFW_KEY_S] = 0;
	}
	else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
		key_mapping[GLFW_KEY_D] = 1;
	}
	else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
		key_mapping[GLFW_KEY_D] = 0;
	}
	else if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		key_mapping[GLFW_KEY_Z] = 1;
	}
	else if (key == GLFW_KEY_Z && action == GLFW_RELEASE) {
		key_mapping[GLFW_KEY_Z] = 0;
	}
	else if (key == GLFW_KEY_X && action == GLFW_PRESS) {
		key_mapping[GLFW_KEY_X] = 1;
	}
	else if (key == GLFW_KEY_X && action == GLFW_RELEASE) {
		key_mapping[GLFW_KEY_X] = 0;
	}

	if (m_renderWidget != nullptr) m_renderWidget->setKeyMap(key_mapping);
}
void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {}