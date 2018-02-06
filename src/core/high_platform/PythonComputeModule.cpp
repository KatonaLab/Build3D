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

// --------------------------------------------------------

PythonOutputPort::PythonOutputPort(ComputeModule& parent)
    : OutputPort(parent)
{}

size_t PythonOutputPort::typeHash() const
{

}

PythonOutputPort::~PythonOutputPort()
{}

bool PythonOutputPort::compatible(std::weak_ptr<cp::InputPort> input) const
{

}

void PythonOutputPort::cleanOnReset()
{

}

// --------------------------------------------------------

PythonInputPort::PythonInputPort(cp::ComputeModule& parent)
    : InputPort(parent)
{

}

void PythonInputPort::fetch()
{

}

size_t PythonInputPort::typeHash() const
{

}

PythonInputPort::~PythonInputPort()
{

}

// --------------------------------------------------------

PythonInputPortCollection::PythonInputPortCollection(cp::ComputeModule& parent)
    : InputPortCollection(parent)
{

}

void PythonInputPortCollection::fetch()
{

}

std::weak_ptr<cp::InputPort> PythonInputPortCollection::get(size_t portId)
{

}

size_t PythonInputPortCollection::size() const
{

}

PythonInputPortCollection::~PythonInputPortCollection()
{

}

// --------------------------------------------------------


PythonOutputPortCollection::PythonOutputPortCollection(cp::ComputeModule& parent)
    : OutputPortCollection(parent)
{

}

std::weak_ptr<cp::OutputPort> PythonOutputPortCollection::get(size_t portId)
{

}

size_t PythonOutputPortCollection::size() const
{

}

PythonOutputPortCollection::~PythonOutputPortCollection()
{

}