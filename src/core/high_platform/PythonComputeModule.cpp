#include "PythonComputeModule.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <core/multidim_image_platform/MultiDimImage.hpp>
#include <memory>

using namespace core::high_platform;
using namespace core::compute_platform;
using namespace core::multidim_image_platform;
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

template <typename T>
void pyDeclareMultiDimImageType(pybind11::module &m, const char* name)
{
    py::class_<PyMultiDimImage<T>>(m, name)
    .def(py::init([](std::vector<std::size_t> dims)
        {
            return std::unique_ptr<PyMultiDimImage<T>>(new PyMultiDimImage<T>(dims));
        }))
    .def("getMeta", &PyMultiDimImage<T>::getMeta)
    .def("getData", &PyMultiDimImage<T>::getData)
    .def("setData", &PyMultiDimImage<T>::setData);
}

PYBIND11_EMBEDDED_MODULE(a3dc, m)
{
    py::class_<Meta>(m, "Meta")
    .def(py::init<>())
    .def("add", &Meta::add)
    .def("has", &Meta::has)
    .def("remove", &Meta::remove)
    .def("clear", &Meta::clear)
    .def("__repr__",
        [](const Meta &) {
            return "<a3dc.Meta>";
        }
    );

    pyDeclareMultiDimImageType<int8_t>(m, "ImageS8");
    pyDeclareMultiDimImageType<uint8_t>(m, "ImageU8");
    pyDeclareMultiDimImageType<int32_t>(m, "ImageS32");
    pyDeclareMultiDimImageType<uint32_t>(m, "ImageU32");
    pyDeclareMultiDimImageType<int64_t>(m, "ImageS64");
    pyDeclareMultiDimImageType<uint64_t>(m, "ImageU64");
    pyDeclareMultiDimImageType<float>(m, "ImageF");
    pyDeclareMultiDimImageType<double>(m, "ImageD");

    // m.def("set_module_args", [](py::dict args) {
    //     PythonEnvironment::instance().setArgs(args);
    // });
    // m.def("set_module_main", [](py::function func) {
    //     PythonEnvironment::instance().setMain(func);
    // });
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
    m_inputPorts.size()
    
    py::scoped_interpreter guard{};
    py::exec(m_code);
    PythonEnvironment::instance().run();
}
