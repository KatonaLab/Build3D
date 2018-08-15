#ifndef _app_OutputInterfaceModule_h_
#define _app_OutputInterfaceModule_h_

#include <core/compute_platform/ComputeModule.h>
#include <core/high_platform/PythonComputeModule.h>
#include <core/compute_platform/port_utils.hpp>
#include <core/multidim_image_platform/MultiDimImage.hpp>

class ImageOutputInterfaceModule : public core::compute_platform::ComputeModule {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    typedef core::compute_platform::InputPortCollectionBase InputPortCollectionBase;
    typedef core::compute_platform::OutputPortCollectionBase OutputPortCollectionBase;
    typedef core::compute_platform::ComputeModule ComputeModule;
    template <typename R> using MultiDimImage = core::multidim_image_platform::MultiDimImage<R>;
public:
    ImageOutputInterfaceModule(ComputePlatform& parent,
        InputPortCollectionBase& inputs,
        OutputPortCollectionBase& outputs,
        const std::string& name = "");
    std::shared_ptr<MultiDimImage<float>> getImage();
    virtual ~ImageOutputInterfaceModule() = default;
protected:
    std::shared_ptr<MultiDimImage<float>> m_result;
};

template <typename T>
class TypedImageOutputInterfaceModule : public ImageOutputInterfaceModule {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    template <typename R> using MultiDimImage = core::multidim_image_platform::MultiDimImage<R>;
    template <typename R> using TypedInputPortCollection = core::compute_platform::TypedInputPortCollection<R>;
    typedef core::compute_platform::OutputPortCollection OutputPortCollection;
public:
    TypedImageOutputInterfaceModule(ComputePlatform& parent);
    void execute() override;
protected:
    TypedInputPortCollection<MultiDimImage<T>> m_inputs;
    OutputPortCollection m_outputs;
};

template <>
class TypedImageOutputInterfaceModule<float>: public ImageOutputInterfaceModule {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    template <typename R> using MultiDimImage = core::multidim_image_platform::MultiDimImage<R>;
    template <typename R> using TypedInputPortCollection = core::compute_platform::TypedInputPortCollection<R>;
    typedef core::compute_platform::OutputPortCollection OutputPortCollection;
public:
    TypedImageOutputInterfaceModule(ComputePlatform& parent);
    void execute() override;
protected:
    TypedInputPortCollection<MultiDimImage<float>> m_inputs;
    OutputPortCollection m_outputs;
};

#include "OutputInterfaceModules.ipp"

#endif
