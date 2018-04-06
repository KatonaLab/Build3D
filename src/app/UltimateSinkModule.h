#ifndef _app_UltimateSinkModule_h_
#define _app_UltimateSinkModule_h_

#include <core/compute_platform/ports.h>
#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>

namespace cp = core::compute_platform;

class SinkInputPort: public cp::InputPort {
public:
    SinkInputPort(cp::ComputeModule& parent);
    void fetch() override;
    std::size_t typeHash() const override;
    void fakeTypeHash(std::size_t typeHash);
protected:
    std::size_t m_fakeTypeHash = 0;
};

class SinkInputPortCollection: public cp::InputPortCollection {
public:
    SinkInputPortCollection(cp::ComputeModule& parent);
    std::weak_ptr<cp::InputPort> get(std::size_t portId) override;
    std::size_t size() const override;
    void sinkOutputPort(std::shared_ptr<cp::OutputPort> output);
protected:
    std::vector<std::shared_ptr<SinkInputPort>> m_ports;
};

class UltimateSinkModule : public cp::ComputeModule {
public:
    UltimateSinkModule(cp::ComputePlatform& parent);
    void sinkOutputPort(std::shared_ptr<cp::OutputPort> output);
    void execute() override;
protected:
    SinkInputPortCollection m_inputs;
    cp::OutputPortCollection m_outputs;
};

#endif
