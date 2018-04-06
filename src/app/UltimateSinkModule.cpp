#include "UltimateSinkModule.h"

SinkInputPort::SinkInputPort(cp::ComputeModule& parent)
    : core::compute_platform::InputPort(parent)
{}

void SinkInputPort::fetch()
{}

std::size_t SinkInputPort::typeHash() const
{
    return m_fakeTypeHash;
}

void SinkInputPort::fakeTypeHash(std::size_t typeHash)
{
    m_fakeTypeHash = typeHash;
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
    newInput->fakeTypeHash(output->typeHash());
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