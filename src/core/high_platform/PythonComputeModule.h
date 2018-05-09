#ifndef _core_high_platform_PythonComputeModule_h_
#define _core_high_platform_PythonComputeModule_h_

#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>
#include <core/compute_platform/ports.h>
#include <core/compute_platform/port_utils.hpp>
#include <core/multidim_image_platform/MultiDimImage.hpp>

#include <functional>
#include <map>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

// --------------------------------------------------------
PORT_TYPE_TRAITS(core::multidim_image_platform::MultiDimImage<float>, {"float-image"});
PORT_TYPE_TRAITS(core::multidim_image_platform::MultiDimImage<uint32_t>, {"int-image"});
// --------------------------------------------------------

namespace core {
namespace high_platform {

namespace cp = core::compute_platform;
namespace md = core::multidim_image_platform;

// TODO: separate into files

// --------------------------------------------------------

enum class PyTypes {
    TYPE_unknown,
    TYPE_int8_t,
    TYPE_int16_t,
    TYPE_int32_t,
    TYPE_int64_t,
    TYPE_uint8_t,
    TYPE_uint16_t,
    TYPE_uint32_t,
    TYPE_uint64_t,
    TYPE_float,
    TYPE_double,
    TYPE_MultiDimImageInt8,
    TYPE_MultiDimImageInt16,
    TYPE_MultiDimImageInt32,
    TYPE_MultiDimImageInt64,
    TYPE_MultiDimImageUInt8,
    TYPE_MultiDimImageUInt16,
    TYPE_MultiDimImageUInt32,
    TYPE_MultiDimImageUInt64,
    TYPE_MultiDimImageFloat,
    TYPE_MultiDimImageDouble
};

struct Arg {
    std::string name;
    PyTypes type;
    std::string tag;
};

typedef std::vector<Arg> ProcessArg;
typedef std::function<void()> ProcessFunc;

// --------------------------------------------------------

struct CustomOutStream {
    void write(const std::string& str)
    {
        callback(str);
    }
    std::function<void(const std::string&)> callback;
};

struct OutStreamRouters {
    CustomOutStream stdOut = {[](const std::string& str) { std::cout << "custom " << str; }};
    CustomOutStream stdErr = {[](const std::string& str) { std::cerr << "custom " << str; }};
};

class PythonEnvironment {    
public:
    static PythonEnvironment& instance();
    void reset();
    void exec(std::string code);
    ~PythonEnvironment();
    // TODO: hide naked member variables
    ProcessArg inputs;
    ProcessArg outputs;
    ProcessFunc func;
    // TODO: reassigning these will break things,
    // hide these member variables
    OutStreamRouters outStreamRouters;
protected:
    PythonEnvironment();
};

// --------------------------------------------------------

class PyInputPortWrapper {
public:
    PyInputPortWrapper(PyTypes type) : m_type(type)
    {}
    PyTypes pyType() const
    {
        return m_type;
    }
    virtual pybind11::object toPyObject()
    {
        return pybind11::none();
    }
    virtual std::shared_ptr<cp::InputPort> port()
    {
        throw std::runtime_error("PyInputPortWrapper");
        return std::shared_ptr<cp::InputPort>();
    }
    virtual ~PyInputPortWrapper() = default;
protected:
    PyTypes m_type;
};

typedef std::shared_ptr<PyInputPortWrapper> PyInputPortWrapperPtr;

class PyOutputPortWrapper {
public:
    PyOutputPortWrapper(PyTypes type) : m_type(type)
    {}
    PyTypes pyType() const
    {
        return m_type;
    }
    virtual pybind11::object toPyObject()
    {
        return pybind11::none();
    }
    virtual void fromPyObject(pybind11::object)
    {}
    virtual std::shared_ptr<cp::OutputPort> port()
    {
        throw std::runtime_error("PyOutputPortWrapper");
        return std::shared_ptr<cp::OutputPort>();
    }
    virtual ~PyOutputPortWrapper() = default;
protected:
    PyTypes m_type;
};

typedef std::shared_ptr<PyOutputPortWrapper> PyOutputPortWrapperPtr;

// --------------------------------------------------------

template <typename T>
class PyInputPortWrapperPod : public PyInputPortWrapper {
public:
    PyInputPortWrapperPod(std::shared_ptr<cp::TypedInputPort<T>> port, PyTypes type)
        : PyInputPortWrapper(type), m_port(port)
    {}
    pybind11::object toPyObject() override
    {
        return pybind11::cast(m_port->value());
    }
    std::shared_ptr<cp::InputPort> port() override
    {
        return m_port;
    }
protected:
    std::shared_ptr<cp::TypedInputPort<T>> m_port;
};

template <typename T>
class PyInputPortWrapperNonPod : public PyInputPortWrapper {
public:
    PyInputPortWrapperNonPod(std::shared_ptr<cp::TypedInputPort<T>> port, PyTypes type)
        : PyInputPortWrapper(type), m_port(port)
    {}
    pybind11::object toPyObject() override
    {
        return pybind11::cast(m_port->sharedValue());
    }
    std::shared_ptr<cp::InputPort> port() override
    {
        return m_port;
    }
protected:
    std::shared_ptr<cp::TypedInputPort<T>> m_port;
};

template <typename T>
class PyOutputPortWrapperPod : public PyOutputPortWrapper {
public:
    PyOutputPortWrapperPod(std::shared_ptr<cp::TypedOutputPort<T>> port, PyTypes type)
        : PyOutputPortWrapper(type), m_port(port)
    {}
    pybind11::object toPyObject() override
    {
         return pybind11::cast<T>(m_port->value());
    }
    void fromPyObject(pybind11::object obj) override
    {
        m_port->forwardFromSharedPtr(std::make_shared<T>(obj.cast<T>()));
    }
    std::shared_ptr<cp::OutputPort> port() override
    {
        return m_port;
    }
protected:
    std::shared_ptr<cp::TypedOutputPort<T>> m_port;
};

template <typename T>
class PyOutputPortWrapperNonPod : public PyOutputPortWrapper {
public:
    PyOutputPortWrapperNonPod(std::shared_ptr<cp::TypedOutputPort<T>> port, PyTypes type)
        : PyOutputPortWrapper(type), m_port(port)
    {}
    pybind11::object toPyObject() override
    {
        return pybind11::cast(m_port->sharedValue());
    }
    void fromPyObject(pybind11::object obj) override
    {
        m_port->forwardFromSharedPtr(obj.cast<std::shared_ptr<T>>());
    }
    std::shared_ptr<cp::OutputPort> port() override
    {
        return m_port;
    }
protected:
    std::shared_ptr<cp::TypedOutputPort<T>> m_port;
};

// --------------------------------------------------------

class DynamicInputPortCollection : public cp::InputPortCollection {
    typedef std::map<std::string, PyInputPortWrapperPtr> MapType;
public:
    DynamicInputPortCollection(cp::ComputeModule& parent);
    void fetch() override;
    std::weak_ptr<cp::InputPort> get(size_t portId) override;
    size_t size() const override;
    void push(std::string name, PyInputPortWrapperPtr portWrapper);
    MapType::iterator begin() { return m_map.begin(); }
    MapType::iterator end() { return m_map.end(); }
protected:
    MapType m_map;
    std::vector<std::shared_ptr<cp::InputPort>> m_orderedInputs;
};

// --------------------------------------------------------

class DynamicOutputPortCollection : public cp::OutputPortCollection {
    typedef std::map<std::string, PyOutputPortWrapperPtr> MapType;
public:
    DynamicOutputPortCollection(cp::ComputeModule& parent);
    std::weak_ptr<cp::OutputPort> get(size_t portId) override;
    size_t size() const override;
    void push(std::string name, PyOutputPortWrapperPtr portWrapper);
    MapType::iterator begin() { return m_map.begin(); }
    MapType::iterator end() { return m_map.end(); }
protected:
    MapType m_map;
    std::vector<std::shared_ptr<cp::OutputPort>> m_orderedOutputs;
};

// --------------------------------------------------------

class PythonComputeModule : public cp::ComputeModule {
public:
    PythonComputeModule(cp::ComputePlatform& platform,
        std::string code,
        const std::string& name = "");
protected:
    void buildPorts();
    void execute() override;
    PyInputPortWrapperPtr createInputPortWrapper(PyTypes t);
    PyOutputPortWrapperPtr createOutputPortWrapper(PyTypes t);
    PyTypes inputPortPyType(std::string name);
    PyTypes outputPortPyType(std::string name);
private:
    DynamicInputPortCollection m_inputPorts;
    DynamicOutputPortCollection m_outputPorts;
    std::string m_code;
    ProcessFunc m_func;
};

// --------------------------------------------------------

// TODO: review this class and consider removing it
// TODO: this is a highly dangerous class along with the 
// private-data-opening MultiDimImage mechanism, FIXME
template <typename T>
class PyImageView {
public:
    PyImageView(md::MultiDimImage<T>& im, const std::vector<std::size_t>& coords)
        : m_source(im)
    {
        switch (im.dims())
        {
            case 0:
            case 1:
            case 2:
                m_planeId = 0;
                break;
            default:
                auto dims = im.dimList();
                dims.erase(dims.begin(), dims.begin() + 2);
                m_planeId = core::multidim_image_platform::detail::flatCoordinate(coords, dims);
        }
    }
    std::size_t rows()
    {
        switch (m_source.dims())
        {
            case 0: return 0;
            case 1: return 1;
            default: return m_source.dim(0);
        }
    }
    std::size_t cols()
    {
        switch (m_source.dims())
        {
            case 0: return 0;
            default: return m_source.dim(1);
        }
    }
    T* data()
    {
        return &m_source.unsafeData()[m_planeId].front();
    }
    std::vector<T>& dataVec()
    {
        return m_source.unsafeData()[m_planeId];
    }
protected:
    md::MultiDimImage<T>& m_source;
    std::size_t m_planeId;
};

}}

#endif
