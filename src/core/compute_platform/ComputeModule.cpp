#include "ComputeModule.h"

#include "ComputePlatform.h"

using namespace core::compute_platform;

void ComputeModule::evaluate()
{
    m_inputs.fetch();
    execute();
}

size_t ComputeModule::numInputs() const
{
    return m_inputs.size();
}

size_t ComputeModule::numOutputs() const
{
    return m_outputs.size();
}

std::weak_ptr<InputPort> ComputeModule::inputPort(size_t id)
{
    return m_inputs.get(id);
}

std::weak_ptr<OutputPort> ComputeModule::outputPort(size_t id)
{
    return m_outputs.get(id);
}

ComputeModule::ComputeModule(ComputePlatform& parent,
    InputPortCollection& inputs,
    OutputPortCollection& outputs)
    : m_parent(parent), m_inputs(inputs), m_outputs(outputs)
{
    m_parent.addModule(this);
}