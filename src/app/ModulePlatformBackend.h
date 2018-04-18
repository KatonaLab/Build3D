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

// TODO: expose BackendModule to QML and return BackendModule object
// when calling getComputeModule(uid) e.g.

class ModulePlatformBackend: public QObject {
    Q_OBJECT
public:
    explicit ModulePlatformBackend(QObject* parent = Q_NULLPTR);
    virtual ~ModulePlatformBackend() = default;
    Q_INVOKABLE QList<int> createSourceModulesFromIcsFile(const QUrl& filename);
    Q_INVOKABLE int createGenericModule(const QString& scriptPath);
    Q_INVOKABLE bool hasModule(int uid);
    Q_INVOKABLE void destroyModule(int uid);
    Q_INVOKABLE QVariantList getInputOptions(int uid, int inputPortId);
    Q_INVOKABLE bool connectInputOutput(int outputModuleUid, int outputPortId, int inputModuleUid, int inputPortId);
    Q_INVOKABLE void disconnectInput(int inputModuleUid, int inputPortId);
    Q_INVOKABLE QVariantList getInputs(int uid);
    Q_INVOKABLE QVariantList getParameters(int uid);
    Q_INVOKABLE void setParameter(int uid, int paramId, QVariant value);
    Q_INVOKABLE QVariantList getOutputs(int uid);
    Q_INVOKABLE VolumeTexture* getModuleTexture(int uid, int outputPortId);
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

#endif
