#ifndef _core_high_platform_PythonComputeModule_h_
#define _core_high_platform_PythonComputeModule_h_

#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>
#include <core/compute_platform/ports.h>
#include <core/compute_platform/port_utils.hpp>
#include <core/multidim_image_platform/MultiDimImage.hpp>

#include <pybind11/embed.h>

#include <string>
#include <utility>

namespace core {
namespace high_platform {

namespace py = pybind11;
namespace cp = core::compute_platform;
namespace md = core::multidim_image_platform;

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

// --------------------------------------------------------

template <typename T>
class PyMultiDimImage : public md::MultiDimImage<T> {
public:
    PyMultiDimImage(std::vector<std::size_t> dims = {})
        : md::MultiDimImage<T>(dims)
    {}
    const md::Meta& getMeta() const
    {
        return this->meta;
    }
    const std::vector<std::vector<T>>& getData() const
    {
        return this->m_planes;
    }
    const std::vector<std::size_t>& getDims() const
    {
        return this->m_dims;
    }
    void setData(std::vector<std::vector<T>>& data, std::vector<std::size_t> dims)
    {
        std::size_t planeSize = 0;
        std::size_t restSize = 0;

        switch (dims.size()) {
            case 0: {
                planeSize = 0;
                restSize = 0;
                break;
            }
            case 1: {
                planeSize = dims[0];
                restSize = 1;
                break;
            }
            case 2: {
                planeSize = dims[0] * dims[1];
                restSize = 1;
                break;
            }
            default: {
                planeSize = dims[0] * dims[1];
                restSize = 1;
                for (std::size_t i = 2; i < dims.size(); ++i) {
                    restSize *= dims[i];
                }
            }
        }

        if (data.size() != restSize) {
            throw std::runtime_error("data dimensions and the given dimensions differ");
        }

        for (std::size_t i = 0; i < data.size(); ++i) {
            if (data[i].size() != planeSize) {
                throw std::runtime_error("data dimensions and the given dimensions differ");
            }
        }

        PyMultiDimImage<T> newImage(dims);
        newImage.m_planes = data;
        std::swap(*this, newImage);
    }
};

}}

#endif
