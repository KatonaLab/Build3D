#ifndef _app_NodePlatformBackend_h_
#define _app_NodePlatformBackend_h_

#include <memory>
#include <QtCore>
#include <QQmlComponent>
#include <core/compute_platform/ports.h>
#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>
#include <core/multidim_image_platform/MultiDimImage.hpp>
#include <core/high_platform/PythonComputeModule.h>

typedef core::multidim_image_platform::MultiDimImage<float> Image;

// TODO: split into separate files

namespace cp = core::compute_platform;
namespace hp = core::high_platform;

class VolumeTexture;
typedef QSharedPointer<VolumeTexture> VolumeTexturePtr;

class SinkInputPort: public cp::InputPort {
public:
    SinkInputPort(cp::ComputeModule& parent)
    : core::compute_platform::InputPort(parent)
    {}

    void fetch() override
    {}

    std::size_t typeHash() const override
    {
        return m_fakeTypeHash;
    }

    void fakeTypeHash(std::size_t typeHash)
    {
        m_fakeTypeHash = typeHash;
    }
protected:
    std::size_t m_fakeTypeHash = 0;
};

class SinkInputPortCollection: public cp::InputPortCollection {
public:
    SinkInputPortCollection(cp::ComputeModule& parent)
    : cp::InputPortCollection(parent)
    {}

    std::weak_ptr<cp::InputPort> get(std::size_t portId) override
    {
        return m_ports[portId];
    }

    std::size_t size() const override
    {
        return m_ports.size();
    }

    void sinkOutputPort(std::shared_ptr<cp::OutputPort> output)
    {
        auto newInput = std::make_shared<SinkInputPort>(m_parent);
        newInput->fakeTypeHash(output->typeHash());
        output->bind(newInput);
        m_ports.push_back(newInput);
    }
protected:
    std::vector<std::shared_ptr<SinkInputPort>> m_ports;
};

class UltimateSink : public cp::ComputeModule {
public:
    UltimateSink(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}

    void sinkOutputPort(std::shared_ptr<cp::OutputPort> output)
    {
        m_inputs.sinkOutputPort(output);
    }

    void execute() override
    {}
protected:
    SinkInputPortCollection m_inputs;
    cp::OutputPortCollection m_outputs;
};

class BackendModule {
public:
    BackendModule(uint32_t uid) : m_uid(uid)
    {}
    
    uint32_t uid() const
    {
        return m_uid;
    }

    virtual QVariant getProperties() = 0;
    virtual VolumeTexture& getModuleTexture(std::size_t outputPortId) = 0;
    virtual ~BackendModule() = default;
protected:
    uint32_t m_uid;
};

class DataSourceModule : public cp::ComputeModule, public BackendModule {
public:
    DataSourceModule(cp::ComputePlatform& parent, uint32_t uid)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        BackendModule(uid),
        m_inputs(*this),
        m_outputs(*this)
    {}

    void execute() override
    {
        m_outputs.output<0>()->forwardFromSharedPtr(m_data);
    }

    void setData(std::shared_ptr<Image> data)
    {
        m_data = data;
    }
protected:
    std::shared_ptr<Image> m_data;
    cp::InputPortCollection m_inputs;
    cp::TypedOutputPortCollection<Image> m_outputs;
};

class GenericModule : public hp::PythonComputeModule, public BackendModule {
public:
    GenericModule(cp::ComputePlatform& parent, const std::string& script, uint32_t uid)
        : hp::PythonComputeModule(parent, script),
        BackendModule(uid)
    {}
};

class ModulePlatformBackend: public QObject {
    Q_OBJECT
public:
    explicit ModulePlatformBackend(QObject* parent = Q_NULLPTR);
    virtual ~ModulePlatformBackend() = default;
    Q_INVOKABLE QList<uint32_t> createSourceNodesFromIcsFile(const QUrl& filename);
    Q_INVOKABLE uint32_t createGenericNode(const QUrl& scriptPath);
    Q_INVOKABLE bool hasNode(uint32_t uid);
    Q_INVOKABLE void destroyNode(uint32_t uid);
    Q_INVOKABLE QVariant getNodeState(uint32_t uid);
    Q_INVOKABLE VolumeTexture& getNodeTexture(uint32_t uid, std::size_t outputPortId);
    Q_INVOKABLE QList<uint32_t> getPortCompatibleNodes(uint32_t uid, std::size_t inputPortId);
Q_SIGNALS:
    void nodeCreated(uint32_t uid);
    void nodeWillBeDestroyed(uint32_t uid);
    void nodeDestroyed(uint32_t uid);
private:
    uint32_t nextUid() const
    {
        static uint32_t counter = 0;
        return counter++;
    }
    
    cp::ComputePlatform m_platform;
    std::vector<std::unique_ptr<BackendModule>> m_modules;
    core::multidim_image_platform::MultiDimImage<float> m_image;
};

#endif
