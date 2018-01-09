#include "Group.h"

using namespace core;

Group::Group(std::string name)
{

}

bool Group::empty() const
{
    // TODO
    return true;
}

size_t Group::size() const
{
    // TODO
    return 0;
}

NodePtr Group::addNode(const std::string &name)
{
    NodePtr ptr;
    return ptr;
}

GroupPtr Group::addGroup(const std::string &name)
{
    GroupPtr ptr;
    return ptr;
}

void Group::remove(const NodePtr &node)
{

}

void Group::remove(const GroupPtr &node)
{

}

void Group::clear()
{

}

bool Group::valid() const
{

}