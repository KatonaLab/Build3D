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
    BackendModule(uint32_t uid);
    uint32_t uid() const;
    virtual QVariantMap getProperties() = 0;
    virtual VolumeTexture* getModuleTexture(std::size_t outputPortId) = 0;
    virtual bool hasTexture(std::size_t outputPortId) = 0;
    virtual cp::ComputeModule& getModule() = 0;
    virtual ~BackendModule() = default;
protected:
    uint32_t m_uid;
};

class DataSourceModule : public cp::ComputeModule, public BackendModule {
public:
    DataSourceModule(cp::ComputePlatform& parent, uint32_t uid);
    void execute() override;
    void setData(std::shared_ptr<md::MultiDimImage<float>> data);
    QVariantMap getProperties() override;
    bool hasTexture(std::size_t outputPortId) override;
    VolumeTexture* getModuleTexture(std::size_t outputPortId) override;
    cp::ComputeModule& getModule() override;
protected:
    std::shared_ptr<md::MultiDimImage<float>> m_data;
    cp::InputPortCollection m_inputs;
    cp::TypedOutputPortCollection<md::MultiDimImage<float>> m_outputs;
};

class GenericModule : public hp::PythonComputeModule, public BackendModule {
public:
    GenericModule(cp::ComputePlatform& parent, const std::string& script, uint32_t uid);
    QVariantMap getProperties() override;
    bool hasTexture(std::size_t outputPortId) override;
    VolumeTexture* getModuleTexture(std::size_t outputPortId) override;
    cp::ComputeModule& getModule() override;
};

class ModulePlatformBackend: public QObject {
    Q_OBJECT
public:
    explicit ModulePlatformBackend(QObject* parent = Q_NULLPTR);
    virtual ~ModulePlatformBackend() = default;
    Q_INVOKABLE QList<uint32_t> createSourceModulesFromIcsFile(const QUrl& filename);
    Q_INVOKABLE uint32_t createGenericModule(const QUrl& scriptPath);
    Q_INVOKABLE bool hasModule(uint32_t uid);
    Q_INVOKABLE void destroyModule(uint32_t uid);
    Q_INVOKABLE QVariantMap getModuleProperties(uint32_t uid);
    Q_INVOKABLE VolumeTexture* getModuleTexture(uint32_t uid, std::size_t outputPortId);
    Q_INVOKABLE QMultiMap<uint32_t, std::size_t> getCompatibleModules(uint32_t uid, std::size_t inputPortId);
Q_SIGNALS:
    void moduleCreated(uint32_t uid);
    void moduleWillBeDestroyed(uint32_t uid);
    void moduleDestroyed(uint32_t uid);
private:
    inline uint32_t nextUid() const;
    cp::ComputePlatform m_platform;
    std::map<uint32_t, std::unique_ptr<BackendModule>> m_modules;
};

#endif
