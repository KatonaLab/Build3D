#include "ForwardNetwork.h"

using namespace core;

ForwardNetwork::ForwardNetwork(std::string name) : Group(name)
{}

bool ForwardNetwork::connect(NodePtr &from, NodePtr &to)
{
    // TODO
    return true;
}

void ForwardNetwork::walk(std::function<void(const NodePtr &)>)
{
    // TODO
}