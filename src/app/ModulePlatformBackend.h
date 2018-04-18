#ifndef _app_NodePlatformBackend_h_
#define _app_NodePlatformBackend_h_

#include <memory>
#include <QtCore>
#include <QQmlComponent>
#include <QMultiMap>
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
    BackendModule(int uid, const std::string& name);
    int uid() const;
    std::string name() const;
    virtual VolumeTexture* getModuleTexture(std::size_t outputPortId) = 0;
    virtual bool hasTexture(std::size_t outputPortId) = 0;
    virtual cp::ComputeModule& getComputeModule() = 0;
    virtual ~BackendModule() = default;
protected:
    int m_uid;
    std::string m_name;
};

class DataSourceModule : public cp::ComputeModule, public BackendModule {
public:
    DataSourceModule(cp::ComputePlatform& parent, int uid);
    void execute() override;
    void setData(std::shared_ptr<md::MultiDimImage<float>> data);
    bool hasTexture(std::size_t outputPortId) override;
    VolumeTexture* getModuleTexture(std::size_t outputPortId) override;
    cp::ComputeModule& getComputeModule() override;
protected:
    std::shared_ptr<md::MultiDimImage<float>> m_data;
    cp::InputPortCollection m_inputs;
    cp::TypedOutputPortCollection<md::MultiDimImage<float>> m_outputs;
};

class GenericModule : public hp::PythonComputeModule, public BackendModule {
public:
    GenericModule(cp::ComputePlatform& parent, const std::string& script, int uid);
    bool hasTexture(std::size_t outputPortId) override;
    VolumeTexture* getModuleTexture(std::size_t outputPortId) override;
    cp::ComputeModule& getComputeModule() override;
};

class PrivateModulePlatformBackend;

template <typename R, typename... Args>
struct TryCatchDecorator {
    TryCatchDecorator(R(PrivateModulePlatformBackend::*memberFunction)(Args...),
        PrivateModulePlatformBackend& instance)
    : m_function(memberFunction), m_instance(instance)
    {}

    R call(Args&&... args)
    {
        try {
            return (m_instance.*m_function)(std::forward<Args>(args)...);
        } catch (std::exception& e) {
            std::cerr << "exception: " << e.what() << std::endl;
        }
        return R();
    }
    R(PrivateModulePlatformBackend::*m_function)(Args...);
    PrivateModulePlatformBackend& m_instance;
};

template <typename R, typename... Args, typename... Args2>
R callDecorator(R(PrivateModulePlatformBackend::*function)(Args...),
    PrivateModulePlatformBackend& instance,
    Args2&&... args)
{
    TryCatchDecorator<R, Args...> d(function, instance);
    return d.call(std::forward<Args>(args)...);
}

class PrivateModulePlatformBackend {
public:
    virtual ~PrivateModulePlatformBackend() = default;
    QList<int> createSourceModulesFromIcsFile(const QUrl& filename);
    int createGenericModule(const QString& scriptPath);
    bool hasModule(int uid);
    void destroyModule(int uid);
    QVariantList getInputOptions(int uid, int inputPortId);
    bool connectInputOutput(int outputModuleUid, int outputPortId, int inputModuleUid, int inputPortId);
    void disconnectInput(int inputModuleUid, int inputPortId);
    QVariantList getInputs(int uid);
    QVariantList getParameters(int uid);
    void setParameter(int uid, int paramId, QVariant value);
    QVariantList getOutputs(int uid);
    VolumeTexture* getModuleTexture(int uid, int outputPortId);
private:
    cp::ComputePlatform m_platform;
    std::map<int, std::unique_ptr<BackendModule>> m_modules;
    std::map<QString, QObjectList> m_inputOptions;
private:
    inline int nextUid() const;
    BackendModule& getBackendModule(int uid);
    std::weak_ptr<cp::InputPort> getInputPort(int uid, int portId);
    std::weak_ptr<cp::OutputPort> getOutputPort(int uid, int portId);
};

class ModulePlatformBackend: public QObject {
    Q_OBJECT
public:
    explicit ModulePlatformBackend(QObject* parent = Q_NULLPTR)
        : QObject(parent)
    {}
    virtual ~ModulePlatformBackend() = default;
    Q_INVOKABLE QList<int> createSourceModulesFromIcsFile(const QUrl& filename)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::createSourceModulesFromIcsFile,
            m_private, filename);
    }
    Q_INVOKABLE int createGenericModule(const QString& scriptPath)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::createGenericModule,
            m_private, scriptPath);
    }
    Q_INVOKABLE bool hasModule(int uid)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::hasModule,
            m_private, uid);
    }
    Q_INVOKABLE void destroyModule(int uid)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::destroyModule,
            m_private, uid);
    }
    Q_INVOKABLE QVariantList getInputOptions(int uid, int inputPortId)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::getInputOptions,
            m_private, uid, inputPortId);
    }
    Q_INVOKABLE bool connectInputOutput(int outputModuleUid, int outputPortId, int inputModuleUid, int inputPortId)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::connectInputOutput,
            m_private, outputModuleUid, outputPortId, inputModuleUid, inputPortId);
    }
    Q_INVOKABLE void disconnectInput(int inputModuleUid, int inputPortId)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::disconnectInput,
            m_private, inputModuleUid, inputPortId);
    }
    Q_INVOKABLE QVariantList getInputs(int uid)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::getInputs,
            m_private, uid);
    }
    Q_INVOKABLE QVariantList getParameters(int uid)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::getParameters,
            m_private, uid);
    }
    Q_INVOKABLE void setParameter(int uid, int paramId, QVariant value)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::setParameter,
            m_private, uid, paramId, value);
    }
    Q_INVOKABLE QVariantList getOutputs(int uid)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::getOutputs,
            m_private, uid);
    }
    Q_INVOKABLE VolumeTexture* getModuleTexture(int uid, int outputPortId)
    {
        return callDecorator(
            &PrivateModulePlatformBackend::getModuleTexture,
            m_private, uid, outputPortId);
    }
private:
    PrivateModulePlatformBackend m_private;
};

#endif
