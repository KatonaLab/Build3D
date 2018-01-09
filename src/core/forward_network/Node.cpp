#include "Node.h"

using namespace core;

bool Node::valid() const
{
    // TODO
    return true;
}

const std::vector<NodePtr>& Node::inputs() const
{

}

const std::vector<NodePtr>& Node::outputs() const
{

}

const std::string& Node::name() const
{
    return m_name;
}