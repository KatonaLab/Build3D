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
#include <utility>

#include <pybind11/embed.h>

namespace core {
namespace high_platform {

namespace py = pybind11;
namespace cp = core::compute_platform;
namespace md = core::multidim_image_platform;

// TODO: separate into files

// --------------------------------------------------------

enum class PyTypes {
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

typedef std::map<std::string, PyTypes> ProcessArg;
typedef std::function<void()> ProcessFunc;

// --------------------------------------------------------

class PythonEnvironment {
public:
    static PythonEnvironment& instance();
    void reset();
    void exec(std::string code);
    ~PythonEnvironment();
    ProcessArg inputs;
    ProcessArg outputs;
    ProcessFunc func;
protected:
    PythonEnvironment();
};

// --------------------------------------------------------

class DynamicInputPortCollection : public cp::InputPortCollection {
public:
    DynamicInputPortCollection(cp::ComputeModule& parent);
    void fetch() override;
    std::weak_ptr<cp::InputPort> get(size_t portId) override;
    size_t size() const override;
    void push(std::shared_ptr<cp::InputPort> port);
protected:
    std::vector<std::shared_ptr<cp::InputPort>> m_inputPorts;
};

// --------------------------------------------------------

class DynamicOutputPortCollection : public cp::OutputPortCollection {
public:
    DynamicOutputPortCollection(cp::ComputeModule& parent);
    std::weak_ptr<cp::OutputPort> get(size_t portId) override;
    size_t size() const override;
    void push(std::shared_ptr<cp::OutputPort> port);
protected:
    std::vector<std::shared_ptr<cp::OutputPort>> m_outputPorts;
};

// --------------------------------------------------------

class PythonComputeModule : public cp::ComputeModule {
public:
    PythonComputeModule(cp::ComputePlatform& platform, std::string code);
protected:
    void buildPorts();
    void execute() override;
    std::shared_ptr<cp::InputPort> createInputPort(PyTypes t);
    std::shared_ptr<cp::OutputPort> createOutputPort(PyTypes t);
private:
    DynamicInputPortCollection m_inputPorts;
    DynamicOutputPortCollection m_outputPorts;
    std::string m_code;
    std::map<std::string, std::shared_ptr<cp::InputPort>> m_inputPortMap;
    std::map<std::string, std::shared_ptr<cp::OutputPort>> m_outputPortMap;
    ProcessFunc m_func;
};

// --------------------------------------------------------

// TODO: review this class and consider removing it
template <typename T>
class PyImageView {
public:
    PyImageView(md::MultiDimImage<T>& im, std::vector<std::size_t> coords)
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
protected:
    md::MultiDimImage<T>& m_source;
    std::size_t m_planeId;
};

}}

#endif
