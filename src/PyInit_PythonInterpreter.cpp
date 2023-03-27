#include "python_interpreter.h"

std::string PythonInterpreter::python_output;

PyMethodDef PythonInterpreter::custom_methods[] = {
    {"write", py_write, METH_VARARGS, "Custom write function."},
    {NULL, NULL, 0, NULL}
};

PyModuleDef PythonInterpreter::custom_module = {
    PyModuleDef_HEAD_INIT,
    "custom_module",
    NULL,
    -1,
    custom_methods
};

// ... rest of the file

static PyObject* PyInit_PythonInterpreter() {
    return PyModule_Create(&PythonInterpreter::custom_module);
}

PythonInterpreter::PythonInterpreter() {
    PyImport_AppendInittab("python_interpreter", &PyInit_PythonInterpreter);
    Py_Initialize();
    const wchar_t* program_name = L"Python Interpreter";
    Py_SetProgramName(const_cast<wchar_t*>(program_name));
    memset(input_buffer, 0, sizeof(input_buffer));

    PyObject* sys = PyImport_ImportModule("sys");
    PyObject* python_interpreter = PyImport_ImportModule("python_interpreter");
    PyObject* sys_stdout = PyObject_GetAttrString(sys, "stdout");
    PyObject_SetAttrString(sys, "stdout", python_interpreter);
    Py_DECREF(sys);
    Py_DECREF(sys_stdout);
    Py_DECREF(python_interpreter);

    // Initialize globals and locals
    globals = PyDict_New();
    locals = PyDict_New();
}

PythonInterpreter::~PythonInterpreter() {
    // Clean up globals and locals
    Py_DECREF(globals);
    Py_DECREF(locals);

    Py_Finalize();
}


PyObject* PythonInterpreter::py_write(PyObject* self, PyObject* args) {
    const char* str;
    if (!PyArg_ParseTuple(args, "s", &str)) {
        return nullptr;
    }
    python_output.append(str);
    Py_RETURN_NONE;
}
void PythonInterpreter::draw() {
    ImGui::Begin("Python Interpreter");

    bool enter_pressed = ImGui::InputText("##input", input_buffer, IM_ARRAYSIZE(input_buffer), ImGuiInputTextFlags_EnterReturnsTrue);

    ImGui::SameLine();

    // Add a separate button for executing Python code
    bool execute_button_pressed = ImGui::Button("Execute");

    if (enter_pressed || execute_button_pressed) {
        // Execute Python code
        PyObject* result = PyRun_String(input_buffer, Py_single_input, globals, locals);

        if (result) {
            PyObject* result_str = PyObject_Repr(result);
            const char* result_cstr = PyUnicode_AsUTF8(result_str);
            output_buffer.append(result_cstr);
            output_buffer.append("\n");
            Py_DECREF(result);
            Py_DECREF(result_str);
        } else {
            if (PyErr_Occurred()) {
                PyObject* exc_type;
                PyObject* exc_value;
                PyObject* exc_traceback;
                PyErr_Fetch(&exc_type, &exc_value, &exc_traceback);
                PyErr_NormalizeException(&exc_type, &exc_value, &exc_traceback);

                PyObject* exc_str = PyObject_Repr(exc_value);
                const char* exc_cstr = PyUnicode_AsUTF8(exc_str);
                output_buffer.append("Error: ");
                output_buffer.append(exc_cstr);
                output_buffer.append("\n");
                Py_DECREF(exc_type);
                Py_DECREF(exc_value);
                Py_DECREF(exc_traceback);
                Py_DECREF(exc_str);
            }
        }

        PyErr_Clear();
        memset(input_buffer, 0, sizeof(input_buffer));
    }

    ImGui::SameLine();
    if (ImGui::Button("Clear")) {
        output_buffer.clear();
    }

    if (!python_output.empty()) {
        output_buffer.append(python_output.c_str());
        python_output.clear();
    }

    ImGui::BeginChild("##output", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::TextUnformatted(output_buffer.begin(), output_buffer.end());
    ImGui::EndChild();

    ImGui::End();
}

and main.cpp

#include <iostream>
#include "glad/glad.h"
#include "python_interpreter.h"

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char**) {
    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) {
        return 1;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Python Interpreter", NULL, NULL);
    if (window == NULL) {
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    bool err = gladLoadGL() == 0;
    if (err) {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    PythonInterpreter python_interpreter;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        python_interpreter.draw();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}