#include "ComputeModule.h"

#include "ComputePlatform.h"

using namespace core::compute_platform;
using namespace core::directed_acyclic_graph;

TriggerNode::TriggerNode(ComputeModule& parent)
: m_parent(parent)
{}

void TriggerNode::notified()
{
    m_parent.evaluate();
}

ComputeModule& TriggerNode::parent()
{
    return m_parent;
}

void ComputeModule::evaluate()
{
    m_inputs.fetch();
    m_context.name = name();
    m_context.type = moduleTypeName();
    execute(m_context);
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
    InputPortCollectionBase& inputs,
    OutputPortCollectionBase& outputs,
    const std::string& name)
    : m_parent(parent), m_inputs(inputs),
    m_outputs(outputs), m_name(name)
{
    m_node = std::make_shared<TriggerNode>(*this);
    m_parent.addModule(*this, m_node);
}

ComputeModule::~ComputeModule()
{
    m_parent.removeModule(*this);
}

ComputePlatform& ComputeModule::platform()
{
    return m_parent;
}

void ComputeModule::setContext(ModuleContext ctx)
{
    m_context = ctx;
}

ModuleContext ComputeModule::context()
{
    return m_context;
}

NodePtr ComputeModule::node()
{
    return m_node;
}

void ComputeModule::reset()
{
    for (size_t i = 0; i < m_outputs.size(); ++i) {
        m_outputs.get(i).lock()->reset();
    }
}

std::string ComputeModule::name() const
{
    return m_name;
}

void ComputeModule::setName(const std::string& name)
{
    m_name = name;
}

std::string ComputeModule::moduleTypeName() const
{
    return "Unknown Compute Module";
}

bool core::compute_platform::connectPorts(ComputeModule& outputModule, std::size_t outputId,
    ComputeModule& inputModule, std::size_t inputId)
{
    return outputModule.outputPort(outputId).lock()->bind(inputModule.inputPort(inputId));
}

void core::compute_platform::disconnectPorts(ComputeModule& outputModule, std::size_t outputId,
    ComputeModule& inputModule, std::size_t inputId)
{
    outputModule.outputPort(outputId).lock()->unbind(inputModule.inputPort(inputId));
}
