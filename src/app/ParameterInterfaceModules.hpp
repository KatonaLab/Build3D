#ifndef _app_ParameterInterfaceModule_h_
#define _app_ParameterInterfaceModule_h_

#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/port_utils.hpp>

// for PORT_TYPE_TRAITS reg
#include <core/high_platform/PythonComputeModule.h>

#include <QVariant>
#include <QString>
#include <QUrl>

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
    TypedParameterInterfaceModule(ComputePlatform& parent, T initialValue = T());
    bool setData(QVariant var) override;
    void execute() override;
    QVariant data() override;
protected:
    TypedOutputPortCollection<T> m_outputs;
    std::shared_ptr<T> m_data;
};

template <>
class TypedParameterInterfaceModule<EnumPair>: public ParameterInterfaceModule {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    template<typename R> using TypedOutputPortCollection =
        core::compute_platform::TypedOutputPortCollection<R>;
public:
    TypedParameterInterfaceModule(ComputePlatform& parent, EnumPair initialValue = std::make_pair(-1, -1));
    bool setData(QVariant var) override;
    void execute() override;
    QVariant data() override;
protected:
    TypedOutputPortCollection<EnumPair> m_outputs;
    std::shared_ptr<EnumPair> m_data;
};

template <>
class TypedParameterInterfaceModule<QString>: public ParameterInterfaceModule {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    template<typename R> using TypedOutputPortCollection =
        core::compute_platform::TypedOutputPortCollection<R>;
public:
    TypedParameterInterfaceModule(ComputePlatform& parent, QString initialValue = QString());
    bool setData(QVariant var) override;
    void execute() override;
    QVariant data() override;
protected:
    TypedOutputPortCollection<std::string> m_outputs;
    std::shared_ptr<std::string> m_data;
};

template <>
class TypedParameterInterfaceModule<QUrl>: public ParameterInterfaceModule {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    template<typename R> using TypedOutputPortCollection =
        core::compute_platform::TypedOutputPortCollection<R>;
public:
    TypedParameterInterfaceModule(ComputePlatform& parent, QUrl initialValue = QUrl());
    bool setData(QVariant var) override;
    void execute() override;
    QVariant data() override;
protected:
    TypedOutputPortCollection<Url> m_outputs;
    std::shared_ptr<Url> m_data;
};

#include "ParameterInterfaceModules.ipp"

#endif
