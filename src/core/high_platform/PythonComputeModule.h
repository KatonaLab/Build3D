#ifndef _core_high_platform_PythonComputeModule_h_
#define _core_high_platform_PythonComputeModule_h_

#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>
#include <core/compute_platform/ports.h>
#include <core/compute_platform/port_utils.hpp>
#include <core/multidim_image_platform/MultiDimImage.hpp>

#include <string>
#include <utility>

#include <pybind11/embed.h>

namespace core {
namespace high_platform {

namespace py = pybind11;
namespace cp = core::compute_platform;
namespace md = core::multidim_image_platform;

// --------------------------------------------------------

class PythonEnvironment {
public:
    static PythonEnvironment& instance();
    void run();
    ~PythonEnvironment();
protected:
    PythonEnvironment();
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
