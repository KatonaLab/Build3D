#ifndef _app_IcsDataSourceModule_h_
#define _app_IcsDataSourceModule_h_

#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ports.h>
#include <core/compute_platform/port_utils.hpp>
#include <core/compute_platform/ModuleContext.h>
#include <core/multidim_image_platform/MultiDimImage.hpp>

// for QUrl PORT_TYPE_TRAITS reg
#include <core/high_platform/PythonComputeModule.h>
#include <QUrl>

class IcsDataSourceModule : public core::compute_platform::ComputeModule {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    template <typename T> using MultiDimImage = core::multidim_image_platform::MultiDimImage<T>;
    template <typename T, typename ...Ts> using TypedInputPortCollection = core::compute_platform::TypedInputPortCollection<T, Ts...>;
    template <typename T, typename ...Ts> using TypedOutputPortCollection = core::compute_platform::TypedOutputPortCollection<T, Ts...>;
    typedef core::compute_platform::ModuleContext ModuleContext;
public:
    IcsDataSourceModule(ComputePlatform& parent);
    void execute(ModuleContext&) override;
    std::string moduleTypeName() const override
    {
        return "ics data source";
    }
protected:
    bool modifiedParameters();
    Url m_lastPathValue;
    bool m_lastNormalizeValue;
    std::vector<std::shared_ptr<MultiDimImage<float>>> m_cache;
    TypedInputPortCollection<Url, bool> m_inputs;
    TypedOutputPortCollection<
        MultiDimImage<float>,
        MultiDimImage<float>,
        MultiDimImage<float>,
        MultiDimImage<float>> m_outputs;
};

// TODO: split into separate files -------------------------------

class TwoChannelIcsModule : public core::compute_platform::ComputeModule {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    template <typename T> using MultiDimImage = core::multidim_image_platform::MultiDimImage<T>;
    template <typename T, typename ...Ts> using TypedInputPortCollection = core::compute_platform::TypedInputPortCollection<T, Ts...>;
    template <typename T, typename ...Ts> using TypedOutputPortCollection = core::compute_platform::TypedOutputPortCollection<T, Ts...>;
    typedef core::compute_platform::ModuleContext ModuleContext;
public:
    TwoChannelIcsModule(ComputePlatform& parent);
    void execute(ModuleContext&) override;
    std::string moduleTypeName() const override
    {
        return "ics source";
    }
protected:
    bool modifiedParameters();
    Url m_lastPathValue;
    std::vector<std::shared_ptr<MultiDimImage<float>>> m_cache;
    TypedInputPortCollection<Url, uint32_t, uint32_t> m_inputs;
    TypedOutputPortCollection<
        MultiDimImage<float>,
        MultiDimImage<float>> m_outputs;
};


#endif
