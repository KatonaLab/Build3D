#include "ports.h"

#include "ComputeModule.h"

using namespace core::compute_platform;
using namespace std;

OutputPort::OutputPort(ComputeModule& parent) : m_parent(parent)
{}

std::string OutputPort::name() const
{
    return m_name;
}

void OutputPort::setName(const std::string& name)
{
    m_name = name;
}

bool OutputPort::bind(std::weak_ptr<InputPort> inputPort)
{
    if (compatible(inputPort)) {
        if (auto ptr = inputPort.lock()) {
            bool canConnect = m_parent.node()->connect(ptr->parent().node());
            if (canConnect) {
                m_numBinds++;
                ptr->m_source = shared_from_this();
                return true;
            }
        }
    }
    return false;
}

void OutputPort::unbind(std::weak_ptr<InputPort> inputPort)
{
    if (auto inputPtr = inputPort.lock()) {
        if (auto ptr = inputPtr->m_source.lock()) {
            if (ptr == shared_from_this()) {
                inputPtr->m_source.reset();
                if (m_numBinds == 1) {
                    m_parent.node()->disconnect(inputPtr->parent().node());
                } else if (m_numBinds < 1) {
                    throw std::runtime_error(string("output bind count decreases below 0, ") +
                        string("indicating some port managing issues in compute_platform library"));
                }
                m_numBinds--;
            }
        }
    }
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

ComputeModule& OutputPort::parent()
{
    return m_parent;
}

OutputPort::~OutputPort()
{}

InputPort::InputPort(ComputeModule& parent) : m_parent(parent)
{}

std::string InputPort::name() const
{
    return m_name;
}

void InputPort::setName(const std::string& name)
{
    m_name = name;
}

ComputeModule& InputPort::parent()
{
    return m_parent;
}

bool InputPort::connected() const
{
    return m_source.lock() != nullptr;
}

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