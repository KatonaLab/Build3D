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
    BackendModule(int uid);
    int uid() const;
    virtual QVariantMap getProperties() = 0;
    virtual VolumeTexture* getModuleTexture(std::size_t outputPortId) = 0;
    virtual bool hasTexture(std::size_t outputPortId) = 0;
    virtual cp::ComputeModule& getModule() = 0;
    virtual ~BackendModule() = default;
protected:
    int m_uid;
};

class DataSourceModule : public cp::ComputeModule, public BackendModule {
public:
    DataSourceModule(cp::ComputePlatform& parent, int uid);
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
    GenericModule(cp::ComputePlatform& parent, const std::string& script, int uid);
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
    Q_INVOKABLE QList<int> createSourceModulesFromIcsFile(const QUrl& filename);
    Q_INVOKABLE int createGenericModule(const QString& scriptPath);
    Q_INVOKABLE bool hasModule(int uid);
    Q_INVOKABLE void destroyModule(int uid);
    Q_INVOKABLE QVariantMap getModuleProperties(int uid);
    Q_INVOKABLE VolumeTexture* getModuleTexture(int uid, int outputPortId);
    Q_INVOKABLE QMultiMap<int, std::size_t> getCompatibleModules(int uid, int inputPortId);
Q_SIGNALS:
    void moduleCreated(int uid);
    void moduleWillBeDestroyed(int uid);
    void moduleDestroyed(int uid);
private:
    inline int nextUid() const;
    cp::ComputePlatform m_platform;
    std::map<int, std::unique_ptr<BackendModule>> m_modules;
};

#endif
