#include "ports.h"

#include "ComputeModule.h"

using namespace core::compute_platform;
using namespace std;

PortBase::PortBase(ComputeModule& parent)
    : m_parent(parent)
{}

std::string PortBase::name() const
{
    return m_name;
}

void PortBase::setName(const std::string& name)
{
    m_name = name;
}

std::string PortBase::tags() const
{
    return m_tags;
}

bool PortBase::hasTag(const std::string& tag) const
{
    // TODO: !!! hasTag("param") will return true with m_tags == "non-parameter" !!!
    // FIXME:
    return m_tags.find(tag) != std::string::npos;
}

void PortBase::setTags(const std::string& tags)
{
    m_tags = tags;
}

ComputeModule& PortBase::parent()
{
    return m_parent;
}

// TODO: separate classes into files
// ---------------------

OutputPort::OutputPort(ComputeModule& parent) : PortBase(parent)
{}

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

OutputPort::~OutputPort()
{}

InputPort::InputPort(ComputeModule& parent) : PortBase(parent)
{}

bool InputPort::connected() const
{
    return m_source.lock() != nullptr;
}

std::weak_ptr<OutputPort> InputPort::getSource() const
{
    return m_source;
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