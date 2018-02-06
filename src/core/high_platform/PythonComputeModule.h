#ifndef _core_high_platform_PythonComputeModule_h_
#define _core_high_platform_PythonComputeModule_h_

#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>
#include <core/compute_platform/ports.h>

#include <pybind11/embed.h>

#include <string>

namespace core {
namespace high_platform {

namespace py = pybind11;
namespace cp = core::compute_platform;

// --------------------------------------------------------

class PythonEnvironment {
public:
    static PythonEnvironment& instance();
    void setMain(py::function func);
    void setArgs(py::dict args);
    void run();
protected:
    PythonEnvironment();
    py::function m_func;
    py::dict m_args;
};

// --------------------------------------------------------

class PythonOutputPort : public cp::OutputPort {
public:
    PythonOutputPort(cp::ComputeModule& parent);
    size_t typeHash() const override;
    ~PythonOutputPort() override;
protected:
    bool compatible(std::weak_ptr<cp::InputPort> input) const override;
    void cleanOnReset() override;
};

// --------------------------------------------------------

class PythonInputPort : public cp::InputPort {
    friend class PythonOutputPort;
public:
    PythonInputPort(cp::ComputeModule& parent);
    void fetch() override;
    size_t typeHash() const override;
    ~PythonInputPort() override;
};

// --------------------------------------------------------

class PythonInputPortCollection : public cp::InputPortCollection {
public:
    PythonInputPortCollection(cp::ComputeModule& parent);
    void fetch() override;
    std::weak_ptr<cp::InputPort> get(size_t portId) override;
    size_t size() const override;
    ~PythonInputPortCollection() override;
};

// --------------------------------------------------------

class PythonOutputPortCollection : public cp::OutputPortCollection {
public:
    PythonOutputPortCollection(cp::ComputeModule& parent);
    std::weak_ptr<cp::OutputPort> get(size_t portId) override;
    size_t size() const override;
    ~PythonOutputPortCollection() override;
};

// --------------------------------------------------------

class PythonComputeModule : public cp::ComputeModule {
public:
    PythonComputeModule(cp::ComputePlatform& platform,
        std::string code);
protected:
    void execute() override;
private:
    PythonInputPortCollection m_inputPorts;
    PythonOutputPortCollection m_outputPorts;
    std::string m_code;
};

}}

#endif
