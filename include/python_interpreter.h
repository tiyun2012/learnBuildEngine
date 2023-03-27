#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>

#include <Python.h>
#include <string>

class PythonInterpreter {
public:
    PythonInterpreter();
    ~PythonInterpreter();
    void draw();

    static PyModuleDef custom_module;

private:
    static PyObject* py_write(PyObject* self, PyObject* args);
    static std::string python_output;
    static PyMethodDef custom_methods[];

    PyObject* globals; // Add this line
    PyObject* locals;  // Add this line

    // Add input_buffer and output_buffer as private members
    char input_buffer[1024];
    ImGuiTextBuffer output_buffer;

    // Other private members and methods

}; 
