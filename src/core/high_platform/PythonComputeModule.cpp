#include "PythonComputeModule.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pytypes.h>

#include <core/multidim_image_platform/MultiDimImage.hpp>
#include <core/compute_platform/port_utils.hpp>

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

void PythonEnvironment::reset()
{

}

void PythonEnvironment::exec(std::string code)
{
    py::exec(code);
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
    py::enum_<PyTypes>(m, "types", py::arithmetic())
    .value("int8", PyTypes::TYPE_int8_t)
    .value("int16", PyTypes::TYPE_int16_t)
    .value("int32", PyTypes::TYPE_int32_t)
    .value("int64", PyTypes::TYPE_int64_t)
    .value("uint8", PyTypes::TYPE_uint8_t)
    .value("uint16", PyTypes::TYPE_uint16_t)
    .value("uint32", PyTypes::TYPE_uint32_t)
    .value("uint64", PyTypes::TYPE_uint64_t)
    .value("float", PyTypes::TYPE_float)
    .value("double", PyTypes::TYPE_double)
    .value("ImageInt8", PyTypes::TYPE_MultiDimImageInt8)
    .value("ImageInt16", PyTypes::TYPE_MultiDimImageInt16)
    .value("ImageInt32", PyTypes::TYPE_MultiDimImageInt32)
    .value("ImageInt64", PyTypes::TYPE_MultiDimImageInt64)
    .value("ImageUInt8", PyTypes::TYPE_MultiDimImageUInt8)
    .value("ImageUInt16", PyTypes::TYPE_MultiDimImageUInt16)
    .value("ImageUInt32", PyTypes::TYPE_MultiDimImageUInt32)
    .value("ImageUInt64", PyTypes::TYPE_MultiDimImageUInt64)
    .value("ImageFloat", PyTypes::TYPE_MultiDimImageFloat)
    .value("ImageDouble", PyTypes::TYPE_MultiDimImageDouble);

    m.def("def_process_module",
        [](ProcessArg inputs, ProcessArg outputs, const ProcessFunc& func)
        {
            auto& env = PythonEnvironment::instance();
            env.inputs = inputs;
            env.outputs = outputs;
            env.func = func;
        });

    // m.attr("")

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
{
    buildPorts();
}

shared_ptr<PyInputPortWrapper> PythonComputeModule::createInputPortWrapper(PyTypes t)
{
    #define CASE(E, T) \
        case PyTypes::E: \
            {auto tp = shared_ptr<TypedInputPort<T>>(new TypedInputPort<T>(*this)); \
            return make_shared<PyTypedInputPortWrapper<T>>(tp);}

    switch (t) {
        CASE(TYPE_int8_t, int8_t)
        CASE(TYPE_int16_t, int16_t)
        CASE(TYPE_int32_t, int32_t)
        CASE(TYPE_int64_t, int64_t)
        CASE(TYPE_uint8_t, uint8_t)
        CASE(TYPE_uint16_t, uint16_t)
        CASE(TYPE_uint32_t, uint32_t)
        CASE(TYPE_uint64_t, uint64_t)
        CASE(TYPE_float, float)
        CASE(TYPE_double, double)
        CASE(TYPE_MultiDimImageInt8, MultiDimImage<int8_t>)
        CASE(TYPE_MultiDimImageInt16, MultiDimImage<int16_t>)
        CASE(TYPE_MultiDimImageInt32, MultiDimImage<int32_t>)
        CASE(TYPE_MultiDimImageInt64, MultiDimImage<int64_t>)
        CASE(TYPE_MultiDimImageUInt8, MultiDimImage<uint8_t>)
        CASE(TYPE_MultiDimImageUInt16, MultiDimImage<uint16_t>)
        CASE(TYPE_MultiDimImageUInt32, MultiDimImage<uint32_t>)
        CASE(TYPE_MultiDimImageUInt64, MultiDimImage<uint64_t>)
        CASE(TYPE_MultiDimImageFloat, MultiDimImage<float>)
        CASE(TYPE_MultiDimImageDouble, MultiDimImage<double>)
        default: throw std::runtime_error("unknown input port type");
    }
    #undef CASE
}

shared_ptr<PyOutputPortWrapper> PythonComputeModule::createOutputPortWrapper(PyTypes t)
{
    #define CASE(E, T) \
        case PyTypes::E: \
            {auto tp = shared_ptr<TypedOutputPort<T>>(new TypedOutputPort<T>(*this)); \
            return make_shared<PyTypedOutputPortWrapper<T>>(tp);}

    switch (t) {
        CASE(TYPE_int8_t, int8_t)
        CASE(TYPE_int16_t, int16_t)
        CASE(TYPE_int32_t, int32_t)
        CASE(TYPE_int64_t, int64_t)
        CASE(TYPE_uint8_t, uint8_t)
        CASE(TYPE_uint16_t, uint16_t)
        CASE(TYPE_uint32_t, uint32_t)
        CASE(TYPE_uint64_t, uint64_t)
        CASE(TYPE_float, float)
        CASE(TYPE_double, double)
        CASE(TYPE_MultiDimImageInt8, MultiDimImage<int8_t>)
        CASE(TYPE_MultiDimImageInt16, MultiDimImage<int16_t>)
        CASE(TYPE_MultiDimImageInt32, MultiDimImage<int32_t>)
        CASE(TYPE_MultiDimImageInt64, MultiDimImage<int64_t>)
        CASE(TYPE_MultiDimImageUInt8, MultiDimImage<uint8_t>)
        CASE(TYPE_MultiDimImageUInt16, MultiDimImage<uint16_t>)
        CASE(TYPE_MultiDimImageUInt32, MultiDimImage<uint32_t>)
        CASE(TYPE_MultiDimImageUInt64, MultiDimImage<uint64_t>)
        CASE(TYPE_MultiDimImageFloat, MultiDimImage<float>)
        CASE(TYPE_MultiDimImageDouble, MultiDimImage<double>)
        default: throw std::runtime_error("unknown output port type");
    }
    #undef CASE
}

void PythonComputeModule::buildPorts()
{
    auto& env = PythonEnvironment::instance();
    env.reset();
    env.exec(m_code);
    m_func = env.func;

    for (auto& p : env.inputs) {
        auto pw = createInputPortWrapper(p.second);
        m_inputPorts.push(p.first, pw);
    }

    for (auto& p : env.outputs) {
        auto pw = createOutputPortWrapper(p.second);
        m_outputPorts.push(p.first, pw);
    
    }
}

void PythonComputeModule::execute()
{
    auto& env = PythonEnvironment::instance();
    auto i = unique_ptr<int>(new int);
    py::object obj = py::cast(i);
    m_func();
}

// --------------------------------------------------------

DynamicInputPortCollection::DynamicInputPortCollection(ComputeModule& parent)
    : InputPortCollection(parent)
{}

void DynamicInputPortCollection::fetch()
{
    for (auto& p : m_orderedInputs) {
        p->fetch();
    }
}

std::weak_ptr<InputPort> DynamicInputPortCollection::get(size_t portId)
{
    if (portId >= m_orderedInputs.size()) {
        throw std::out_of_range("no such id in input ports");
    }
    return m_orderedInputs[portId];
}

void DynamicInputPortCollection::push(std::string name,
    std::shared_ptr<PyInputPortWrapper> portWrapper)
{
    m_orderedInputs.push_back(portWrapper->port());
    m_inputMap[name] = portWrapper;
}

size_t DynamicInputPortCollection::size() const
{
    return m_orderedInputs.size();
}

// --------------------------------------------------------

DynamicOutputPortCollection::DynamicOutputPortCollection(ComputeModule& parent)
    : OutputPortCollection(parent)
{}

std::weak_ptr<OutputPort> DynamicOutputPortCollection::get(size_t portId)
{
    if (portId >= m_orderedOutputs.size()) {
        throw std::out_of_range("no such id in output ports");
    }
    return m_orderedOutputs[portId];
}

size_t DynamicOutputPortCollection::size() const
{
    return m_orderedOutputs.size();
}

void DynamicOutputPortCollection::push(std::string name,
    std::shared_ptr<PyOutputPortWrapper> portWrapper)
{
    m_orderedOutputs.push_back(portWrapper->port());
    m_outputMap[name] = portWrapper;
}