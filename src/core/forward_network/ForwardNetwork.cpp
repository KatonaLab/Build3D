#include "ForwardNetwork.h"

#include <memory>

using namespace core;
using namespace std;

ForwardNetwork::Ptr ForwardNetwork::create(const std::string& name)
{
    return shared_ptr<ForwardNetwork>(new ForwardNetwork(name));
}

ForwardNetwork::ForwardNetwork(const std::string& name) : Group(name)
{}

bool ForwardNetwork::connect(Node::Ptr &from, Node::Ptr &to)
{
    // TODO: cycle detection
    from->addOutput(to);
    to->addIntput(from);
    return true;
}

void ForwardNetwork::walk(std::function<void(const Node::Ptr &)>)
{
    // TODO
}

bool ForwardNetwork::valid() const
{
    return true;
}