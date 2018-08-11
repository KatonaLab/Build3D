#ifndef _app_ParameterInterfaceModule_h_
#define _app_ParameterInterfaceModule_h_

#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/port_utils.hpp>
#include <QVariant>
#include <QString>

class ParameterInterfaceModule : public core::compute_platform::ComputeModule {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    typedef core::compute_platform::OutputPortCollectionBase OutputPortCollectionBase;
    typedef core::compute_platform::InputPortCollection InputPortCollection;
public:
    ParameterInterfaceModule(ComputePlatform& parent,
        const std::string& name,
        OutputPortCollectionBase& outputs);
    virtual bool setData(QVariant value) = 0;
    virtual QVariant data() = 0;
    virtual ~ParameterInterfaceModule() = default;
protected:
    InputPortCollection m_inputs;
};

template <typename T>
class TypedParameterInterfaceModule: public ParameterInterfaceModule {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    template<typename R> using TypedOutputPortCollection =
        core::compute_platform::TypedOutputPortCollection<R>;
public:
    TypedParameterInterfaceModule(ComputePlatform& parent, T initialValue);
    bool setData(QVariant var) override;
    void execute() override;
    QVariant data() override;
protected:
    TypedOutputPortCollection<T> m_outputs;
    std::shared_ptr<T> m_data;
};

// template spetialization for QString <-> std::string interchange
template <>
class TypedParameterInterfaceModule<QString>: public ParameterInterfaceModule {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    template<typename R> using TypedOutputPortCollection =
        core::compute_platform::TypedOutputPortCollection<R>;
public:
    TypedParameterInterfaceModule(ComputePlatform& parent, QString initialValue);
    bool setData(QVariant var) override;
    void execute() override;
    QVariant data() override;
protected:
    TypedOutputPortCollection<std::string> m_outputs;
    std::shared_ptr<std::string> m_data;
};

#include "ParameterInterfaceModules.ipp"

#endif
