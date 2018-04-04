#ifndef _app_NodePlatformBackend_h_
#define _app_NodePlatformBackend_h_

#include <memory>
#include <QtCore>
#include <QQmlComponent>
#include <core/compute_platform/ports.h>
#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>
#include <core/multidim_image_platform/MultiDimImage.hpp>

typedef core::multidim_image_platform;::MultiDimImage<float> Image;

class VolumeTexture;
typedef QSharedPointer<VolumeTexture> VolumeTexturePtr;

class SinkInputPort: public core::compute_platform::InputPort {
public:
    SinkInputPort(core::compute_platform::ComputeModule& parent)
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

class SinkInputPortCollection: public core::compute_platform::InputPortCollection {
public:
    SinkInputPortCollection(core::compute_platform::ComputeModule& parent)
    : core::compute_platform::InputPortCollection(parent)
    {}

    std::weak_ptr<core::compute_platform::InputPort> get(std::size_t portId) override
    {
        return m_ports[portId];
    }

    std::size_t size() const override
    {
        return m_ports.size();
    }

    void sinkOutputPort(std::shared_ptr<core::compute_platform::OutputPort> output)
    {
        auto newInput = std::make_shared<SinkInputPort>(m_parent);
        newInput->fakeTypeHash(output->typeHash());
        output->bind(newInput);
        m_ports.push_back(newInput);
    }
protected:
    std::vector<std::shared_ptr<SinkInputPort>> m_ports;
};

class UltimateSink : public core::compute_platform::ComputeModule {
public:
    UltimateSink(core::compute_platform::ComputePlatform& parent)
        : core::compute_platform::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}

    void sinkOutputPort(std::shared_ptr<core::compute_platform::OutputPort> output)
    {
        m_inputs.sinkOutputPort(output);
    }

    void execute() override
    {}
protected:
    SinkInputPortCollection m_inputs;
    core::compute_platform::OutputPortCollection m_outputs;
};

class DataSourceModule : public core::compute_platform::ComputeModule {
public:
    DataSourceModule(core::compute_platform::ComputePlatform& parent)
        : core::compute_platform::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}

    void execute() override
    {}
protected:
    std::shared_ptr<Image> m_seed;
    core::compute_platform::InputPortCollection m_inputs;
    core::compute_platform::OutputPortCollection<Image> m_outputs;
};

class NodePlatformBackend: public QObject {
    Q_OBJECT
public:
    explicit NodePlatformBackend(QObject* parent = Q_NULLPTR);
    virtual ~NodePlatformBackend() = default;
    Q_INVOKABLE QList<uint32_t> createSourceNodesFromIcsFile(const QUrl& filename);
    Q_INVOKABLE uint32_t createGenericNode(const QUrl& scriptPath);
    Q_INVOKABLE void hasNode(uint32_t uid);
    Q_INVOKABLE void destroyNode(uint32_t uid);
    Q_INVOKABLE QVariant getNodeState(uint32_t uid);
    Q_INVOKABLE VolumeTexture& getNodeTexture(uint32_t uid, std::size_t outputPortId);
    Q_INVOKABLE QList<uint32_t> getPortCompatibleNodes(uint32_t uid, std::size_t inputPortId);
Q_SIGNALS:
    void nodeCreated(uint32_t uid);
    void nodeWillBeDestroyed(uint32_t uid);
    void nodeDestroyed(uint32_t uid);
private:
    typedef core::compute_platform::ComputePlatform Platform;
    typedef core::compute_platform::ComputeModule Module;
    Platform m_computePlatform;
    std::vector<std::unique_ptr<Module>> m_modules;
    core::multidim_image_platform::MultiDimImage<float> m_image;
};

#endif
