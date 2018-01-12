#include "Node.h"

#include <iostream>
#include <memory>

#include "Group.h"

using namespace core;
using namespace std;

Node::Ptr Node::create(const std::string& name)
{
    return shared_ptr<Node>(new Node(name));
}

Node::Node(const std::string& name) : m_name(name)
{}

bool Node::valid() const
{
    return !m_parent.expired() && m_parent.lock()->valid();
}

const std::vector<Node::Ptr>& Node::inputs() const
{
    return m_inputs;
}

const std::vector<Node::Ptr>& Node::outputs() const
{
    return m_outputs;
}

const std::string& Node::name() const
{
    return m_name;
}

void Node::addIntput(Ptr& node)
{
    m_inputs.push_back(node);
}

void Node::addOutput(Ptr& node)
{
    m_outputs.push_back(node);
}