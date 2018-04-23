#include "Node.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>

#include "Graph.h"

using namespace core::directed_acyclic_graph;
using namespace std;

NodePtr Node::create(const std::string& name)
{
    return NodePtr(new Node(name));
}

Node::Node(const std::string& name) : m_name(name)
{}

NodePtr Node::input(size_t k)
{
    return m_inputs[k].lock();
}

NodePtr Node::output(size_t k)
{
    return m_outputs[k];
}

size_t Node::numberOfInputs() const
{
    return m_inputs.size();
}

size_t Node::numberOfOutputs() const
{
    return m_outputs.size();
}

bool Node::hasOwner() const
{
    return m_owner.expired() == false && m_owner.lock() != nullptr;
}

WeakGraphPtr& Node::owner()
{
    return m_owner;
}

const std::string& Node::name() const
{
    return m_name;
}

bool Node::nodeInOutputs(const ConstNodePtr& node) const
{
    return findOutputConstIterator(node) != m_outputs.end();
}

bool Node::nodeInInputs(const ConstNodePtr& node) const
{
    return findInputConstIterator(node) != m_inputs.end();
}

bool Node::nodeInSubgraph(const ConstNodePtr& targetNode, TraversalMode mode) const
{
    ConstTraversal t = constTraverse(mode);
    while (t.hasNext()) {
        ConstNodePtr current = t.next();
        if (current == targetNode) {
            return true;
        }
    }
    return false;
}

bool Node::connect(NodePtr to)
{
    if (nodeInOutputs(to)) {
        return true;
    }

    if (to->nodeInSubgraph(shared_from_this(), TraversalMode::OutputsOnly) == false) {
        if (hasOwner()) {
            m_owner.lock()->add(to);
        }
        m_outputs.push_back(to);
        to->m_inputs.push_back(shared_from_this());
        return true;
    }

    return false;
}

std::vector<NodePtr>::iterator Node::findOutputIterator(const ConstNodePtr& node)
{
    auto it = std::find(m_outputs.begin(), m_outputs.end(), node);
    return it;
}

std::vector<WeakNodePtr>::iterator Node::findInputIterator(const ConstNodePtr& node)
{
    auto it = find_if(m_inputs.begin(), m_inputs.end(),
        [&node](const WeakNodePtr& w)
        {
            return w.lock() == node;
        });
    return it;
}

std::vector<NodePtr>::const_iterator Node::findOutputConstIterator(const ConstNodePtr& node) const
{
    auto it = std::find(m_outputs.cbegin(), m_outputs.cend(), node);
    return it;
}

std::vector<WeakNodePtr>::const_iterator Node::findInputConstIterator(const ConstNodePtr& node) const
{
    auto it = find_if(m_inputs.cbegin(), m_inputs.cend(),
        [&node](const WeakNodePtr& w)
        {
            return w.lock() == node;
        });
    return it;
}

void Node::disconnect(NodePtr with)
{
    auto outIt = findOutputIterator(with);
    if (outIt != m_outputs.end()) {
        m_outputs.erase(outIt);
        auto inIt = with->findInputIterator(shared_from_this());
        with->m_inputs.erase(inIt);

        return;
    }

    auto inIt = findInputIterator(with);
    if (inIt != m_inputs.end()) {
        m_inputs.erase(inIt);
        auto oIt = with->findOutputIterator(shared_from_this());
        with->m_outputs.erase(oIt);
    }
}

void Node::notified()
{}

Traversal Node::traverse(TraversalMode mode)
{
    return Traversal(shared_from_this(), mode);
}

ConstTraversal Node::constTraverse(TraversalMode mode) const
{
    return ConstTraversal(shared_from_this(), mode);
}

Node::~Node()
{}

