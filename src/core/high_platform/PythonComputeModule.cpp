#include "PythonComputeModule.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <pybind11/pytypes.h>

#include <core/multidim_image_platform/MultiDimImage.hpp>
#include <core/compute_platform/port_utils.hpp>

#include <cstdlib>
#include <functional>
#include <memory>
#include <stdexcept>
#include <stdlib.h>

using namespace core::high_platform;
using namespace core::compute_platform;
using namespace core::multidim_image_platform;
using namespace std;

namespace py = pybind11;

OutStreamRouters PythonEnvironment::outStreamRouters;

PythonEnvironment& PythonEnvironment::instance()
{
    static PythonEnvironment instance;
    return instance;
}

void PythonEnvironment::updateStreamRedirects()
{
    py::module m = py::module::import("a3dc");
    m.attr("stdout") = outStreamRouters.stdOut;
    m.attr("stderr") = outStreamRouters.stdErr;
    py::exec(R"(
import a3dc
import sys
if a3dc.stdout is not None:
    sys.stdout = a3dc.stdout
if a3dc.stderr is not None:
    sys.stderr = a3dc.stderr
    )");
}

// TODO: move to some platform_specific.h/cpp place
#ifdef _WIN32
int setenv(const char *name, const char *value, int overwrite)
{
	int errcode = 0;
	if (!overwrite) {
		size_t envsize = 0;
		errcode = getenv_s(&envsize, NULL, 0, name);
		if (errcode || envsize) return errcode;
	}
	return _putenv_s(name, value);
}
#endif

PythonEnvironment::PythonEnvironment()
{
    if (auto venvPath = getenv("VIRTUAL_ENV")) {
        setenv("PYTHONHOME", venvPath, true);
    }
    py::initialize_interpreter();

    // TODO: it is crucial on Windows to redirect the python
    // stdout, because a Windows GUI program gets NoneType for
    // sys.stdout, that is used by pybind11::print method
    // find a safer way to eliminate the fatal error if not 
    // redirected
    updateStreamRedirects();

    py::module::import("a3dc");
    auto sys = py::module::import("sys");
    py::print("python env info:");
    py::print(sys.attr("path"));
    py::print(sys.attr("executable"));
    py::print(sys.attr("platform"));
    py::print(sys.attr("platform"));
    py::print(sys.attr("version"));
    py::print(sys.attr("version_info"));
}

void PythonEnvironment::reset()
{
    // TODO: implement or remove this function
}

void PythonEnvironment::exec(std::string code)
{
    updateStreamRedirects();
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
    .def("plane", [](MultiDimImage<T>& obj, const std::vector<std::size_t>& coords) -> py::array_t<T>
    {
        PyImageView<T> helper(obj, coords);
        // prevent copy on return
        py::capsule onDestroy(helper.data(), [](void*) {});
        return py::array_t<T>(
            {helper.rows(), helper.cols()},
            {sizeof(T) * helper.rows(), sizeof(T)},
            helper.data(),
            onDestroy);
    }, py::keep_alive<0, 1>(), py::return_value_policy::reference)
    .def("set_plane", [](MultiDimImage<T>& obj,
        const std::vector<std::size_t>& coords,
        const py::array_t<T>& arr)
    {
        PyImageView<T> helper(obj, coords);
        if (arr.ndim() != 2) {
            throw std::runtime_error("data should be 2d when calling set_plane");
        }

        if ((size_t)arr.shape(0) != helper.rows()) {
            throw std::runtime_error("data should have same number of rows when calling set_plane");
        }

        if ((size_t)arr.shape(1) != helper.cols()) {
            throw std::runtime_error("data should have same number of cols when calling set_plane");
        }

        if (arr.data() == helper.data()) {
            return;
        }

        std::vector<T> tmp(arr.data(), arr.data() + arr.size());
        helper.dataVec().swap(tmp);
    })

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
    .value("ImageDouble", PyTypes::TYPE_MultiDimImageDouble)
    .value("GeneralPyType", PyTypes::TYPE_GeneralPyType);

    using namespace pybind11::literals;

    m.def("def_process_module",
        [](ProcessArg inputs, ProcessArg outputs, const ProcessFunc& func)
        {
            auto& env = PythonEnvironment::instance();
            env.inputs = inputs;
            env.outputs = outputs;
            env.func = func;
        },
        "inputs"_a, "outputs"_a, "function"_a);

    py::class_<Arg>(m, "Arg")
    .def(py::init([](string name, PyTypes type, string tag = "")
    {
        return Arg{name, type, tag};
    }), "name"_a, "type"_a, "tag"_a = "")
    .def_readwrite("name", &Arg::name)
    .def_readwrite("type", &Arg::type);

    py::class_<CustomOutStream>(m, "CustomOutStream")
    .def(py::init<>())
    .def("write", &CustomOutStream::write)
    .def("flush", &CustomOutStream::flush);

    m.attr("inputs") = py::dict();
    m.attr("outputs") = py::dict();
    m.attr("stdout") = py::none();
    m.attr("stderr") = py::none();

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

PythonComputeModule::PythonComputeModule(ComputePlatform& platform,
    std::string code,
    const std::string& name)
    : ComputeModule(platform, m_inputPorts, m_outputPorts, name),
    m_inputPorts(*this),
    m_outputPorts(*this),
    m_code(code)
{
    buildPorts();
}

PyInputPortWrapperPtr PythonComputeModule::createInputPortWrapper(PyTypes t)
{
    #define CASE_POD(E, T) \
        case PyTypes::E: \
            {auto tp = TypedInputPort<T>::create(*this); \
            return PyInputPortWrapperPtr(new PyInputPortWrapperPod<T>(tp, PyTypes::E));}

    #define CASE_NON_POD(E, T) \
        case PyTypes::E: \
            {auto tp = TypedInputPort<T>::create(*this); \
            return PyInputPortWrapperPtr(new PyInputPortWrapperNonPod<T>(tp, PyTypes::E));}

    switch (t) {
        CASE_POD(TYPE_int8_t, int8_t)
        CASE_POD(TYPE_int16_t, int16_t)
        CASE_POD(TYPE_int32_t, int32_t)
        CASE_POD(TYPE_int64_t, int64_t)
        CASE_POD(TYPE_uint8_t, uint8_t)
        CASE_POD(TYPE_uint16_t, uint16_t)
        CASE_POD(TYPE_uint32_t, uint32_t)
        CASE_POD(TYPE_uint64_t, uint64_t)
        CASE_POD(TYPE_float, float)
        CASE_POD(TYPE_double, double)
        CASE_NON_POD(TYPE_MultiDimImageInt8, MultiDimImage<int8_t>)
        CASE_NON_POD(TYPE_MultiDimImageInt16, MultiDimImage<int16_t>)
        CASE_NON_POD(TYPE_MultiDimImageInt32, MultiDimImage<int32_t>)
        CASE_NON_POD(TYPE_MultiDimImageInt64, MultiDimImage<int64_t>)
        CASE_NON_POD(TYPE_MultiDimImageUInt8, MultiDimImage<uint8_t>)
        CASE_NON_POD(TYPE_MultiDimImageUInt16, MultiDimImage<uint16_t>)
        CASE_NON_POD(TYPE_MultiDimImageUInt32, MultiDimImage<uint32_t>)
        CASE_NON_POD(TYPE_MultiDimImageUInt64, MultiDimImage<uint64_t>)
        CASE_NON_POD(TYPE_MultiDimImageFloat, MultiDimImage<float>)
        CASE_NON_POD(TYPE_MultiDimImageDouble, MultiDimImage<double>)
        case PyTypes::TYPE_GeneralPyType: {
            auto tp = TypedInputPort<py::object>::create(*this);
            return PyInputPortWrapperPtr(new GeneralPyTypeInputPortWrapper(tp));
        }
        default: throw std::runtime_error("unknown input port type");
    }
    #undef CASE_POD
    #undef CASE_NON_POD
}

PyOutputPortWrapperPtr PythonComputeModule::createOutputPortWrapper(PyTypes t)
{
    #define CASE_POD(E, T) \
        case PyTypes::E: \
            {auto tp = TypedOutputPort<T>::create(*this); \
            return PyOutputPortWrapperPtr(new PyOutputPortWrapperPod<T>(tp, PyTypes::E));}

    #define CASE_NON_POD(E, T) \
        case PyTypes::E: \
            {auto tp = TypedOutputPort<T>::create(*this); \
            return PyOutputPortWrapperPtr(new PyOutputPortWrapperNonPod<T>(tp, PyTypes::E));}

    switch (t) {
        CASE_POD(TYPE_int8_t, int8_t)
        CASE_POD(TYPE_int16_t, int16_t)
        CASE_POD(TYPE_int32_t, int32_t)
        CASE_POD(TYPE_int64_t, int64_t)
        CASE_POD(TYPE_uint8_t, uint8_t)
        CASE_POD(TYPE_uint16_t, uint16_t)
        CASE_POD(TYPE_uint32_t, uint32_t)
        CASE_POD(TYPE_uint64_t, uint64_t)
        CASE_POD(TYPE_float, float)
        CASE_POD(TYPE_double, double)
        CASE_NON_POD(TYPE_MultiDimImageInt8, MultiDimImage<int8_t>)
        CASE_NON_POD(TYPE_MultiDimImageInt16, MultiDimImage<int16_t>)
        CASE_NON_POD(TYPE_MultiDimImageInt32, MultiDimImage<int32_t>)
        CASE_NON_POD(TYPE_MultiDimImageInt64, MultiDimImage<int64_t>)
        CASE_NON_POD(TYPE_MultiDimImageUInt8, MultiDimImage<uint8_t>)
        CASE_NON_POD(TYPE_MultiDimImageUInt16, MultiDimImage<uint16_t>)
        CASE_NON_POD(TYPE_MultiDimImageUInt32, MultiDimImage<uint32_t>)
        CASE_NON_POD(TYPE_MultiDimImageUInt64, MultiDimImage<uint64_t>)
        CASE_NON_POD(TYPE_MultiDimImageFloat, MultiDimImage<float>)
        CASE_NON_POD(TYPE_MultiDimImageDouble, MultiDimImage<double>)
        case PyTypes::TYPE_GeneralPyType: {
            auto tp = TypedOutputPort<py::object>::create(*this);
            return PyOutputPortWrapperPtr(new GeneralPyTypeOutputPortWrapper(tp));
        }
        default: throw std::runtime_error("unknown input port type");
    }
    #undef CASE_POD
    #undef CASE_NON_POD
}

PyTypes PythonComputeModule::inputPortPyType(std::string name)
{
    auto item = find_if(m_inputPorts.begin(), m_inputPorts.end(),
        [&name](const pair<std::string, PyInputPortWrapperPtr>& p)
        {
            return p.first == name;
        });
    if (item != m_inputPorts.end()) {
        return item->second->pyType();
    }
    return PyTypes::TYPE_unknown;
}

PyTypes PythonComputeModule::outputPortPyType(std::string name)
{
    auto item = find_if(m_outputPorts.begin(), m_outputPorts.end(),
        [&name](const pair<std::string, PyOutputPortWrapperPtr>& p)
        {
            return p.first == name;
        });
    if (item != m_outputPorts.end()) {
        return item->second->pyType();
    }
    return PyTypes::TYPE_unknown;
}

void PythonComputeModule::buildPorts()
{
    auto& env = PythonEnvironment::instance();
    env.reset();
    env.exec(m_code);
    m_func = env.func;

    for (auto& p : env.inputs) {
        auto pw = createInputPortWrapper(p.type);
        pw->port()->setName(p.name);
        pw->port()->setTags(p.tag);
        m_inputPorts.push(p.name, pw);
    }

    for (auto& p : env.outputs) {
        auto pw = createOutputPortWrapper(p.type);
        pw->port()->setName(p.name);
        pw->port()->setTags(p.tag);
        m_outputPorts.push(p.name, pw);
    }
}

void PythonComputeModule::execute()
{
    py::module m = py::module::import("a3dc");
    py::object inputs = m.attr("inputs");
    py::object outputs = m.attr("outputs");

    inputs.attr("clear")();

    for (auto& p : m_inputPorts) {
        inputs.attr("__setitem__")(p.first, p.second->toPyObject());
    }

    for (auto& p : m_outputPorts) {
        outputs.attr("__setitem__")(p.first, py::none());
    }

    m_func();

    for (auto& p : m_outputPorts) {
        p.second->fromPyObject(outputs.attr("__getitem__")(p.first));
    }
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

void DynamicInputPortCollection::push(std::string name, PyInputPortWrapperPtr portWrapper)
{
    portWrapper->port()->setName(name);
    m_orderedInputs.push_back(portWrapper->port());
    m_map[name] = portWrapper;
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

void DynamicOutputPortCollection::push(std::string name, PyOutputPortWrapperPtr portWrapper)
{
    portWrapper->port()->setName(name);
    m_orderedOutputs.push_back(portWrapper->port());
    m_map[name] = portWrapper;
}
