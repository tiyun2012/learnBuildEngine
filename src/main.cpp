// Include standard input/output stream library for C++.
#include <iostream>

// Include glad, an OpenGL loader library.
#include "glad/glad.h"

// Include the custom Python interpreter header file.
#include "python_interpreter.h"

// Define a custom error callback function for GLFW.
static void glfw_error_callback(int error, const char* description) {
    // Print the GLFW error message to the standard error stream.
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// Program entry point - main function.
int main(int, char**) {
    // Set the custom error callback function for GLFW.
    glfwSetErrorCallback(glfw_error_callback);

    // Initialize GLFW library.
    if (!glfwInit()) {
        return 1; // Exit the program if GLFW initialization failed.
    }

    // Set the GLSL version string.
    const char* glsl_version = "#version 130";

    // Set the required OpenGL context version (major and minor) using GLFW window hints.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create a GLFW window with specified width, height, and title.
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Python Interpreter", NULL, NULL);
    if (window == NULL) {
        return 1; // Exit the program if window creation failed.
    }

    // Set the created GLFW window as the current OpenGL context.
    glfwMakeContextCurrent(window);

    // Enable vertical synchronization (vsync).
    glfwSwapInterval(1);

    // Initialize the OpenGL loader using the glad library.
    bool err = gladLoadGL() == 0;
    if (err) {
        // Print an error message and exit the program if OpenGL loader initialization failed.
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Check the ImGui library version.
    IMGUI_CHECKVERSION();

    // Create an ImGui context.
    ImGui::CreateContext();

    // Get a reference to the ImGui I/O structure.
    ImGuiIO& io = ImGui::GetIO();

    // Set ImGui style to a dark theme.
    ImGui::StyleColorsDark();

    // Initialize ImGui for GLFW and OpenGL.
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Create an instance of the custom Python interpreter class.
    PythonInterpreter python_interpreter;

    // Enter the main application loop.
    while (!glfwWindowShouldClose(window)) {
        // Poll for window events, such as keyboard input, mouse movement, and window resizing.
        glfwPollEvents();

        // Begin a new ImGui frame for OpenGL3.
        ImGui_ImplOpenGL3_NewFrame();

        // Begin a new ImGui frame for GLFW.
        ImGui_ImplGlfw_NewFrame();

        // Begin a new ImGui frame.
        ImGui::NewFrame();

        // Draw the Python interpreter UI using ImGui.
        python_interpreter.draw();

        // Render ImGui commands.
        ImGui::Render();

        // Get the framebuffer size of the GLFW window.
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);

        // Set the OpenGL viewport to match the framebuffer size.
        glViewport(0, 0, display_w, display_h);

        // Set the OpenGL clear color (background color).
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);

        // Clear the color buffer.
        glClear(GL_COLOR_BUFFER_BIT);

        // Render ImGui
