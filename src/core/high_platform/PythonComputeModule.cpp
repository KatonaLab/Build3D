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
void pyDeclareMultiDimImageType(pybind11::module &m, const char* name)
{
    py::class_<MultiDimImage<T>, std::shared_ptr<MultiDimImage<T>>>(m, name)
    .def(py::init([](std::vector<std::size_t> dims)
    {
        return std::shared_ptr<MultiDimImage<T>>(new MultiDimImage<T>(dims));
    }))
    .def("__getitem__", &MultiDimImage<T>::at)
    .def("__setitem__", [](MultiDimImage<T>& obj, std::vector<std::size_t> coords, T value)
    {
        obj.at(coords) = value;
    })
    .def("shape", &MultiDimImage<T>::dimList)
    .def_readwrite("meta", &MultiDimImage<T>::meta);

    // TODO: type
    // TODO: planes
    // TODO: volumes
}

PYBIND11_EMBEDDED_MODULE(a3dc, m)
{
    py::bind_vector<std::vector<int8_t>>(m, "VectorInt8");
    py::bind_vector<std::vector<int16_t>>(m, "VectorInt16");
    py::bind_vector<std::vector<int32_t>>(m, "VectorInt32");
    py::bind_vector<std::vector<int64_t>>(m, "VectorInt64");

    py::bind_vector<std::vector<uint8_t>>(m, "VectorUInt8");
    py::bind_vector<std::vector<uint16_t>>(m, "VectorUInt16");
    py::bind_vector<std::vector<uint32_t>>(m, "VectorUInt32");
    py::bind_vector<std::vector<uint64_t>>(m, "VectorUInt64");

    py::bind_vector<std::vector<std::vector<int8_t>>>(m, "VectorVectorInt8");
    py::bind_vector<std::vector<std::vector<int16_t>>>(m, "VectorVectorInt16");
    py::bind_vector<std::vector<std::vector<int32_t>>>(m, "VectorVectorInt32");
    py::bind_vector<std::vector<std::vector<int64_t>>>(m, "VectorVectorInt64");

    py::bind_vector<std::vector<std::vector<uint8_t>>>(m, "VectorVectorUInt8");
    py::bind_vector<std::vector<std::vector<uint16_t>>>(m, "VectorVectorUInt16");
    py::bind_vector<std::vector<std::vector<uint32_t>>>(m, "VectorVectorUInt32");
    py::bind_vector<std::vector<std::vector<uint64_t>>>(m, "VectorVectorUInt64");

    py::bind_vector<std::vector<float>>(m, "VectorFloat");
    py::bind_vector<std::vector<double>>(m, "VectorDouble");
    py::bind_vector<std::vector<std::vector<float>>>(m, "VectorVectorFloat");
    py::bind_vector<std::vector<std::vector<double>>>(m, "VectorVectorDouble");

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
