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

// --------------------------------------------------------
// TODO: move to a util file and write test for it

template <typename T, typename R, typename... Args, typename... Args2>
R decorateTryCatch(R(T::*f)(Args...), T& inst, R&& returnValueOnError, Args2&&... args)
{
    try {
        return (inst.*f)(std::forward<Args2>(args)...);
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
    return returnValueOnError;
}

template <typename T, typename... Args, typename... Args2>
void decorateTryCatch(void(T::*f)(Args...), T& inst, Args2&&... args)
{
    try {
        (inst.*f)(std::forward<Args2>(args)...);
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << std::endl;
    }
}

// --------------------------------------------------------

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
    void evaluatePlatform();
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

// TODO: write test for the backend

class ModulePlatformBackend: public QObject {
    Q_OBJECT
public:
    explicit ModulePlatformBackend(QObject* parent = Q_NULLPTR)
        : QObject(parent)
    {}
    virtual ~ModulePlatformBackend() = default;
    Q_INVOKABLE QList<int> createSourceModulesFromIcsFile(const QUrl& filename)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::createSourceModulesFromIcsFile,
            m_private, QList<int>(), filename);
    }
    Q_INVOKABLE int createGenericModule(const QString& scriptPath)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::createGenericModule,
            m_private, -1, scriptPath);
    }
    Q_INVOKABLE bool hasModule(int uid)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::hasModule,
            m_private, false, uid);
    }
    Q_INVOKABLE void destroyModule(int uid)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::destroyModule,
            m_private, uid);
    }
    Q_INVOKABLE QVariantList getInputOptions(int uid, int inputPortId)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::getInputOptions,
            m_private, QVariantList(), uid, inputPortId);
    }
    Q_INVOKABLE bool connectInputOutput(int outputModuleUid, int outputPortId, int inputModuleUid, int inputPortId)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::connectInputOutput,
            m_private, false, outputModuleUid, outputPortId, inputModuleUid, inputPortId);
    }
    Q_INVOKABLE void disconnectInput(int inputModuleUid, int inputPortId)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::disconnectInput,
            m_private, inputModuleUid, inputPortId);
    }
    Q_INVOKABLE QVariantList getInputs(int uid)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::getInputs,
            m_private, QVariantList(), uid);
    }
    Q_INVOKABLE QVariantList getParameters(int uid)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::getParameters,
            m_private, QVariantList(), uid);
    }
    Q_INVOKABLE void setParameter(int uid, int paramId, QVariant value)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::setParameter,
            m_private, uid, paramId, value);
    }
    Q_INVOKABLE QVariantList getOutputs(int uid)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::getOutputs,
            m_private, QVariantList(), uid);
    }
    Q_INVOKABLE VolumeTexture* getModuleTexture(int uid, int outputPortId)
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::getModuleTexture,
            m_private, static_cast<VolumeTexture*>(nullptr), uid, outputPortId);
    }
    Q_INVOKABLE void evaluatePlatform()
    {
        return decorateTryCatch(
            &PrivateModulePlatformBackend::evaluatePlatform,
            m_private);
    }
private:
    PrivateModulePlatformBackend m_private;
};

#endif
