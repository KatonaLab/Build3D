#include "UltimateSinkModule.h"

using namespace core::compute_platform;

SinkInputPort::SinkInputPort(cp::ComputeModule& parent)
    : core::compute_platform::InputPort(parent)
{}

void SinkInputPort::fetch()
{}

const PortTypeTraitsBase& SinkInputPort::traits() const
{
    if (m_fakeTraits) {
        return *m_fakeTraits;
    } else {
        return PortTypeTraits<void>::instance();
    }
}

void SinkInputPort::fakeTraits(const PortTypeTraitsBase& traits)
{
    m_fakeTraits = &traits;
}

SinkInputPortCollection::SinkInputPortCollection(cp::ComputeModule& parent)
    : cp::InputPortCollection(parent)
{}

std::weak_ptr<cp::InputPort> SinkInputPortCollection::get(std::size_t portId)
{
    return m_ports[portId];
}

std::size_t SinkInputPortCollection::size() const
{
    return m_ports.size();
}

void SinkInputPortCollection::sinkOutputPort(std::shared_ptr<cp::OutputPort> output)
{
    auto newInput = std::make_shared<SinkInputPort>(m_parent);
    newInput->fakeTraits(output->traits());
    output->bind(newInput);
    m_ports.push_back(newInput);
}

UltimateSinkModule::UltimateSinkModule(cp::ComputePlatform& parent)
    : cp::ComputeModule(parent, m_inputs, m_outputs),
    m_inputs(*this),
    m_outputs(*this)
{}

void UltimateSinkModule::sinkOutputPort(std::shared_ptr<cp::OutputPort> output)
{
    m_inputs.sinkOutputPort(output);
}

void UltimateSinkModule::execute()
{}