#ifndef _app_NodePlatformBackend_h_
#define _app_NodePlatformBackend_h_

#include <memory>
#include <QtCore>
#include <QQmlComponent>
#include <QMultiMap>
#include <QtDebug>
#include <core/compute_platform/ports.h>
#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>
#include <core/multidim_image_platform/MultiDimImage.hpp>
#include <core/high_platform/PythonComputeModule.h>
#include "VolumeTexture.h"
#include "UltimateSinkModule.h"

namespace cp = core::compute_platform;
namespace hp = core::high_platform;
namespace md = core::multidim_image_platform;

// TODO: separate into files

class BackendModule {
public:
    BackendModule(int uid);
    int uid() const;
    std::string name() const;
    virtual cp::ComputeModule& getComputeModule() = 0;
    virtual const cp::ComputeModule& getComputeModule() const = 0;
    virtual ~BackendModule() = default;
protected:
    int m_uid;
};

class DataSourceModule : public cp::ComputeModule, public BackendModule {
public:
    DataSourceModule(cp::ComputePlatform& parent, int uid);
    void execute() override;
    void setData(std::shared_ptr<md::MultiDimImage<float>> data);
    cp::ComputeModule& getComputeModule() override;
    const cp::ComputeModule& getComputeModule() const override;
protected:
    std::shared_ptr<md::MultiDimImage<float>> m_data;
    cp::InputPortCollection m_inputs;
    cp::TypedOutputPortCollection<md::MultiDimImage<float>> m_outputs;
};

class GenericModule : public hp::PythonComputeModule, public BackendModule {
public:
    GenericModule(cp::ComputePlatform& parent, const std::string& script, int uid);
    cp::ComputeModule& getComputeModule() override;
    const cp::ComputeModule& getComputeModule() const override;
};

// --------------------------------------------------------
// TODO: move to a util file and write test for it

template <typename T, typename R, typename... Args, typename... Args2>
R decorateTryCatch(
    R(T::*f)(Args...),
    T& inst,
    std::function<void(const std::string&)> errorReporter,
    R&& returnValueOnError,
    Args2&&... args)
{
    try {
        return (inst.*f)(std::forward<Args2>(args)...);
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
        errorReporter(e.what());
    }
    return returnValueOnError;
}

template <typename T, typename... Args, typename... Args2>
void decorateTryCatch(
    void(T::*f)(Args...),
    T& inst,
    std::function<void(const std::string&)> errorReporter,
    Args2&&... args)
{
    try {
        (inst.*f)(std::forward<Args2>(args)...);
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
        errorReporter(e.what());
    }
}

class ParameterModule: public cp::ComputeModule {
public:
    ParameterModule(cp::ComputePlatform& parent);
    void execute() override;
    void setData(QVariant value);
protected:
    std::shared_ptr<md::MultiDimImage<float>> m_data;
    cp::InputPortCollection m_inputs;
    cp::TypedOutputPortCollection<md::MultiDimImage<float>> m_outputs;
};

// --------------------------------------------------------

class ParamHelperModule : public cp::ComputeModule {
public:
    // TODO: separate decl from def
    ParamHelperModule(cp::ComputePlatform& parent,
        const std::string& name,
        cp::OutputPortCollection& outputs)
        :
        cp::ComputeModule(parent, m_inputs, outputs, name),
        m_inputs(*this)
    {}
    virtual bool setData(QVariant value) = 0;
protected:
    cp::InputPortCollection m_inputs;
};

template <typename T>
class TypedParamHelperModule: public ParamHelperModule {
public:
    // TODO: separate decl from def
    TypedParamHelperModule(cp::ComputePlatform& parent, T initialValue)
        :
        ParamHelperModule(parent, "ParamHelper-" + std::string(typeid(T).name()), m_outputs),
        m_outputs(*this),
        m_data(std::make_shared<T>(initialValue))
    {}
    bool setData(QVariant var) override
    {
        if (var.canConvert<T>()) {
            *m_data = var.value<T>();
        } else {
            throw std::runtime_error("can not convert from "
                + std::string(var.typeName()) + " to " + typeid(T).name()
                + " in parameter input " + name());
        }
    }
    void execute() override
    {
        m_outputs.template output<0>()->forwardFromSharedPtr(m_data);
    }
protected:
    cp::TypedOutputPortCollection<T> m_outputs;
    std::shared_ptr<T> m_data;
};

// --------------------------------------------------------

class ImageOutputHelperModule : public cp::ComputeModule {
public:
    ImageOutputHelperModule(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_result = m_inputs.input<0>()->inputPtr().lock();
    }
    std::shared_ptr<md::MultiDimImage<float>> getImage()
    {
        return m_result;
    }
protected:
    std::shared_ptr<md::MultiDimImage<float>> m_result;
    cp::TypedInputPortCollection<md::MultiDimImage<float>> m_inputs;
    cp::OutputPortCollection m_outputs;
};

// --------------------------------------------------------

class PrivateModulePlatformBackend {
public:
    virtual ~PrivateModulePlatformBackend() = default;
    QList<int> createSourceModulesFromIcsFile(const QUrl& filename);
    int createGenericModule(const QString& scriptPath);
    void destroyModule(int uid);
    bool hasModule(int uid);
    QVariantMap getModuleProperties(int uid);
    QList<int> enumerateInputPorts(int uid);
    QList<int> enumerateParamPorts(int uid);
    QList<int> enumerateOutputPorts(int uid);
    QVariantMap getInputPortProperties(int uid, int portId);
    QVariantMap getOutputPortProperties(int uid, int portId);
    VolumeTexture* getOutputTexture(int uid, int portId);
    bool connectInputOutput(int outputModuleUid, int outputPortId, int inputModuleUid, int inputPortId);
    void disconnectInput(int inputModuleUid, int inputPortId);
    bool setParamPortProperty(int uid, int portId, QVariant value);
    void evaluatePlatform();
    QVariantList getModuleScriptsList();
private:
    cp::ComputePlatform m_platform;
    std::map<int, std::unique_ptr<BackendModule>> m_modules;
    std::map<QString, QObjectList> m_inputOptions;
    typedef std::pair<int, int> IdPair;
    std::map<IdPair, std::unique_ptr<ParamHelperModule>> m_paramHelpers;
    std::map<IdPair, std::unique_ptr<ImageOutputHelperModule>> m_imageOutputHelpers;
private:
    inline int nextUid() const;
    BackendModule& fetchBackendModule(int uid);
    std::weak_ptr<cp::InputPort> fetchInputPort(int uid, int portId);
    std::weak_ptr<cp::OutputPort> fetchOutputPort(int uid, int portId);
    QList<int> enumeratePorts(int uid,
        std::function<std::size_t(cp::ComputeModule&)> numInputsFunc,
        std::function<bool(cp::ComputeModule&, std::size_t)> predFunc);
    std::vector<std::pair<int, int>> fetchInputPortsCompatibleTo(std::shared_ptr<cp::OutputPort> port);
    std::vector<std::pair<int, int>> fetchOutputPortsCompatibleTo(std::shared_ptr<cp::InputPort> port);
    ParamHelperModule& fetchParamHelperModule(int uid, int portId);
    ImageOutputHelperModule& fetchImageOutputHelperModule(int uid, int portId);
    void buildParamHelperModules(int uid);
    void buildimageOutputHelperModules(int uid);
};

// TODO: write test for the backend

class ModulePlatformBackend: public QObject {
    Q_OBJECT
public:
    explicit ModulePlatformBackend(QObject* parent = Q_NULLPTR);
    virtual ~ModulePlatformBackend() = default;

    Q_INVOKABLE QList<int> createSourceModulesFromIcsFile(const QUrl& filename)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::createSourceModulesFromIcsFile,
            m_private, m_errorFunc, QList<int>(),
            filename
        );
    }
    Q_INVOKABLE int createGenericModule(const QString& scriptPath)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::createGenericModule,
            m_private, m_errorFunc, -1,
            scriptPath
        );
    }
    Q_INVOKABLE void destroyModule(int uid)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::destroyModule,
            m_private, m_errorFunc,
            uid
        );
    }
    Q_INVOKABLE bool hasModule(int uid)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::hasModule,
            m_private, m_errorFunc, false,
            uid
        );
    }
    Q_INVOKABLE QVariantMap getModuleProperties(int uid)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::getModuleProperties,
            m_private, m_errorFunc, QVariantMap(),
            uid
        );
    }
    Q_INVOKABLE QList<int> enumerateInputPorts(int uid)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::enumerateInputPorts,
            m_private, m_errorFunc, QList<int>(),
            uid
        );
    }
    Q_INVOKABLE QList<int> enumerateParamPorts(int uid)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::enumerateParamPorts,
            m_private, m_errorFunc, QList<int>(),
            uid
        );
    }
    Q_INVOKABLE QList<int> enumerateOutputPorts(int uid)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::enumerateOutputPorts,
            m_private, m_errorFunc, QList<int>(),
            uid
        );
    }
    Q_INVOKABLE QVariantMap getInputPortProperties(int uid, int portId)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::getInputPortProperties,
            m_private, m_errorFunc, QVariantMap(),
            uid, portId
        );
    }
    Q_INVOKABLE QVariantMap getOutputPortProperties(int uid, int portId)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::getOutputPortProperties,
            m_private, m_errorFunc, QVariantMap(),
            uid, portId
        );
    }
    Q_INVOKABLE VolumeTexture* getOutputTexture(int uid, int portId)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::getOutputTexture,
            m_private, m_errorFunc, (VolumeTexture*)nullptr,
            uid, portId
        );
    }
    Q_INVOKABLE bool connectInputOutput(int outputModuleUid, int outputPortId, int inputModuleUid, int inputPortId)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::connectInputOutput,
            m_private, m_errorFunc, false,
            outputModuleUid, outputPortId, inputModuleUid, inputPortId
        );
    }
    Q_INVOKABLE void disconnectInput(int inputModuleUid, int inputPortId)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::disconnectInput,
            m_private, m_errorFunc,
            inputModuleUid, inputPortId
        );
    }
    Q_INVOKABLE bool setParamPortProperty(int uid, int portId, QVariant value)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::setParamPortProperty,
            m_private, m_errorFunc, false,
            uid, portId, value
        );
    }
    Q_INVOKABLE void evaluatePlatform()
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::evaluatePlatform,
            m_private, m_errorFunc
        );
    }

    Q_INVOKABLE QVariantList getModuleScriptsList()
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::getModuleScriptsList,
            m_private, m_errorFunc, QVariantList()
        );
    }
protected:
    std::function<void(const std::string&)> m_errorFunc = [](const std::string& msg)
    {
        qCritical() << QString::fromStdString(msg);
    };
private:
    PrivateModulePlatformBackend m_private;
};

#endif
