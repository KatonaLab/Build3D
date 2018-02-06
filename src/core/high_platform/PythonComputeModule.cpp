#include "PythonComputeModule.h"
#include <pybind11/pybind11.h>

using namespace core::high_platform;
using namespace core::compute_platform;
using namespace std;

namespace py = pybind11;

PythonEnvironment& PythonEnvironment::instance()
{
    static PythonEnvironment instance;
    return instance;
}

void PythonEnvironment::setMain(py::function func)
{
    m_func = func;
}

void PythonEnvironment::setArgs(py::dict args)
{
    m_args = args;
}

void PythonEnvironment::run()
{
    if (m_func.is_none()) {
        throw std::runtime_error("py: no valid function was set with a3dc.set_module_main");
    }
}

PythonEnvironment::PythonEnvironment()
{}

PYBIND11_EMBEDDED_MODULE(a3dc, m) {
    m.def("set_module_args", [](py::dict args) {
        PythonEnvironment::instance().setArgs(args);
    });
    m.def("set_module_main", [](py::function func) {
        PythonEnvironment::instance().setMain(func);
    });
}

// --------------------------------------------------------

PythonComputeModule::PythonComputeModule(ComputePlatform& platform, std::string code)
    : ComputeModule(platform, m_inputPorts, m_outputPorts),
    m_inputPorts(*this),
    m_outputPorts(*this),
    m_code(code)
{}

void PythonComputeModule::execute()
{
    py::scoped_interpreter guard{};
    py::exec(m_code);
    PythonEnvironment::instance().run();
}
