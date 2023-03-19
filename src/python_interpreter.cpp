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

    ImGui::InputText("##input", input_buffer, IM_ARRAYSIZE(input_buffer));

    // Add a separate button for executing Python code
    if (ImGui::Button("Execute")) {
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
