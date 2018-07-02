#include "ports.h"

#include "ComputeModule.h"

#include <algorithm>
#include <iostream>

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

const PropertyMap& PortBase::properties() const
{
    return m_propertyMap;
}

PropertyMap& PortBase::properties()
{
    return const_cast<PropertyMap&>(static_cast<const PortBase*>(this)->properties());
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
                m_destinations.push_back(inputPort);
                ptr->m_source = shared_from_this();
                return true;
            }
            std::cout << "can not connect types " << traits().typeName() << " -> " << ptr->traits().typeName() << std::endl;
        }
    }
    return false;
}

void OutputPort::unbind(std::weak_ptr<InputPort> inputPort)
{
    if (auto inputPtr = inputPort.lock()) {

        // do we have this input port?
        bool outputHadInput = false;
        auto dstIt = find_if(m_destinations.begin(), m_destinations.end(),
            [inputPtr](std::weak_ptr<InputPort> item)
            {
                return item.lock() == inputPtr;
            });
        if (dstIt != m_destinations.end()) {
            m_destinations.erase(dstIt);
            outputHadInput = true;
        }

        // do the input port has this output port?
        bool inputHadOutput = false;
        if (auto ptr = inputPtr->m_source.lock()) {
            if (ptr == shared_from_this()) {
                inputPtr->m_source.reset();
                inputHadOutput = true;
            }
        }

        if (m_destinations.size() == 0) {
            m_parent.node()->disconnect(inputPtr->parent().node());
        }
        
        if ((outputHadInput && !inputHadOutput) || (!outputHadInput && inputHadOutput)) {
            throw std::runtime_error("asymetric bookkeeping error while unbinding an output");
        }
    }
}

size_t OutputPort::numBinds() const
{
    return m_destinations.size();
}

void OutputPort::reset()
{
    cleanOnReset();
    m_numInputServed = 0;
}

void OutputPort::purgeInvalidInputs()
{
    m_destinations.erase(remove_if(m_destinations.begin(),
        m_destinations.end(), [](weak_ptr<InputPort> x)
        {
            return x.lock() == nullptr;
        }));
}

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
{
    if (auto p = m_source.lock()) {
        // NOTE: note that this dctr (~InputPort) is called because
        // the shared_ptr ref counter of InputPort is decreased to 0
        // so at this point the shared_ptr is invalidated
        p->purgeInvalidInputs();
    }
}

InputPortCollectionBase::InputPortCollectionBase(ComputeModule& parent)
    : m_parent(parent)
{}

OutputPortCollectionBase::OutputPortCollectionBase(ComputeModule& parent)
    : m_parent(parent)
{}

InputPortCollection::InputPortCollection(ComputeModule& parent)
    : InputPortCollectionBase(parent)
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

OutputPortCollection::OutputPortCollection(ComputeModule& parent)
    : OutputPortCollectionBase(parent)
{}

std::weak_ptr<OutputPort> OutputPortCollection::get(size_t)
{
    throw std::range_error("no ports in output port collection");
}

size_t OutputPortCollection::size() const
{
    return 0;
}
