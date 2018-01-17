#include "ports.h"

#include "ComputeModule.h"

using namespace core::compute_platform;
using namespace std;

OutputPort::OutputPort(ComputeModule& parent) : m_parent(parent)
{}

bool OutputPort::bind(std::weak_ptr<InputPort> inputPort)
{
    if (compatible(inputPort)) {
        // bool canConnect = m_parent.node().connect(inputPort.lock()->m_parent.node());
        // if (canConnect) {
            m_numBinds++;
            return true;
        // }
    }
    return false;
}

size_t OutputPort::numBinds() const
{
    return m_numBinds;
}

void OutputPort::reset()
{
    cleanOnReset();
    m_numInputServed = 0;
}

OutputPort::~OutputPort()
{}

InputPort::InputPort(ComputeModule& parent) : m_parent(parent)
{}

InputPort::~InputPort()
{}

InputPortCollection::InputPortCollection(ComputeModule& parent) : m_parent(parent)
{}

void InputPortCollection::fetch()
{}

std::weak_ptr<InputPort> InputPortCollection::get(size_t)
{
    throw std::range_error("no ports in input port collection");
}

size_t InputPortCollection::size() const
{
    return 0;
}

InputPortCollection::~InputPortCollection()
{}

OutputPortCollection::OutputPortCollection(ComputeModule& parent) : m_parent(parent)
{}

std::weak_ptr<OutputPort> OutputPortCollection::get(size_t)
{
    throw std::range_error("no ports in output port collection");
}

size_t OutputPortCollection::size() const
{
    return 0;
}

OutputPortCollection::~OutputPortCollection()
{}