#include "PythonComputeModule.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <core/multidim_image_platform/MultiDimImage.hpp>
#include <memory>
#include <stdexcept>

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

PythonEnvironment::PythonEnvironment()
{
    py::initialize_interpreter();
    py::module::import("a3dc");
}

PythonEnvironment::~PythonEnvironment()
{
    py::finalize_interpreter();
}

void pyDeclareMetaType(pybind11::module &m)
{
    py::class_<Meta>(m, "Meta")
    .def(py::init<>())
    .def("add", &Meta::add)
    .def("has", &Meta::has)
    .def("remove", &Meta::remove)
    .def("clear", &Meta::clear);
}

template <typename T>
void pyDeclareMultiDimImageType(pybind11::module &m, string name)
{
    py::class_<MultiDimImage<T>, std::shared_ptr<MultiDimImage<T>>>(m, name.c_str())
    .def(py::init([](std::vector<std::size_t> dims)
    {
        return std::shared_ptr<MultiDimImage<T>>(new MultiDimImage<T>(dims));
    }))
    .def("__getitem__", &MultiDimImage<T>::at)
    .def("__setitem__", [](MultiDimImage<T>& obj, std::vector<std::size_t> coords, T value)
    {
        obj.at(coords) = value;
    })
    .def("plane", [](MultiDimImage<T>& obj, std::vector<std::size_t> coords) -> py::array_t<T>
    {
        PyImageView<T> helper(obj, coords);
        // prevent copy on return
        py::capsule onDestroy(helper.data(), [](void*) {});
        return py::array_t<T>(
            {helper.rows(), helper.cols()},
            {sizeof(T) * helper.rows(), sizeof(T)},
            helper.data(),
            onDestroy);
    }, py::keep_alive<0, 1>())
    .def("dims", &MultiDimImage<T>::dimList)
    .def_readwrite("meta", &MultiDimImage<T>::meta);

    // TODO: type
    // TODO: planes
    // TODO: volumes
}

PYBIND11_EMBEDDED_MODULE(a3dc, m)
{
    pyDeclareMetaType(m);

    pyDeclareMultiDimImageType<int8_t>(m, "MultiDimImageInt8");
    pyDeclareMultiDimImageType<int16_t>(m, "MultiDimImageInt16");
    pyDeclareMultiDimImageType<int32_t>(m, "MultiDimImageInt32");
    pyDeclareMultiDimImageType<int64_t>(m, "MultiDimImageInt64");

    pyDeclareMultiDimImageType<uint8_t>(m, "MultiDimImageUInt8");
    pyDeclareMultiDimImageType<uint16_t>(m, "MultiDimImageUInt16");
    pyDeclareMultiDimImageType<uint32_t>(m, "MultiDimImageUInt32");
    pyDeclareMultiDimImageType<uint64_t>(m, "MultiDimImageUInt64");

    pyDeclareMultiDimImageType<float>(m, "MultiDimImageFloat");
    pyDeclareMultiDimImageType<double>(m, "MultiDimImageDouble");
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
    m_inputPorts.size();
    
    py::scoped_interpreter guard{};
    py::exec(m_code);
    // PythonEnvironment::instance().run();
}
