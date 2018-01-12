#include "Group.h"

#include <algorithm>
#include <numeric>

#include "Node.h"

using namespace core;
using namespace std;

Group::Ptr Group::create(const std::string& name)
{
    return shared_ptr<Group>(new Group(name));
}

Group::Group(const std::string& name) : m_name(name)
{}

bool Group::empty() const
{
    return m_nodes.empty() && m_groups.empty();
}

size_t Group::size() const
{
    size_t accum = accumulate(m_groups.begin(), m_groups.end(), 0,
        [](size_t a, const Group::Ptr& g) { return a + g->size(); });
    return m_nodes.size() + m_groups.size() + accum;
}

Group::NodePtr Group::addNode(const std::string& name)
{
    m_nodes.push_back(Node::create(name));
    m_nodes.back()->m_parent = shared_from_this();
    return m_nodes.back();
}

Group::Ptr Group::addGroup(const std::string& name)
{
    m_groups.push_back(Group::create(name));
    m_groups.back()->m_parent = shared_from_this();
    return m_groups.back();
}

void Group::remove(const NodePtr &node)
{
    auto it = find(m_nodes.begin(), m_nodes.end(), node);
    if (it != m_nodes.end()) {
        (*it)->m_parent.reset();
        m_nodes.erase(it);
    }
}

void Group::remove(const Ptr &group)
{
    auto it = find(m_groups.begin(), m_groups.end(), group);
    if (it != m_groups.end()) {
        (*it)->m_parent.reset();
        m_groups.erase(it);
    }
}

void Group::clear()
{
    for_each(m_nodes.begin(), m_nodes.end(),
        [](NodePtr& n) { n->m_parent.reset(); });
    for_each(m_groups.begin(), m_groups.end(),
        [](Ptr& g) { g->m_parent.reset(); });
    m_nodes.clear();
    m_groups.clear();
}

bool Group::valid() const
{
    return !m_parent.expired() && m_parent.lock()->valid();
}

const std::vector<Group::NodePtr>& Group::nodes() const
{
    return m_nodes;
}

const std::vector<Group::Ptr>& Group::groups() const
{
    return m_groups;
}

std::string Group::name() const
{
    return m_name;
}

Group::~Group()
{
    clear();
}