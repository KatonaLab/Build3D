#ifndef _core_high_platform_PythonComputeModule_h_
#define _core_high_platform_PythonComputeModule_h_

#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>
#include <core/compute_platform/ports.h>
#include <core/compute_platform/port_utils.hpp>

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

class PythonComputeModule : public cp::ComputeModule {
public:
    PythonComputeModule(cp::ComputePlatform& platform,
        std::string code);
protected:
    void execute() override;
private:
    cp::InputPortCollection m_inputPorts;
    cp::OutputPortCollection m_outputPorts;
    std::string m_code;
};

}}

#endif
