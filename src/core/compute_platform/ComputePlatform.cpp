#include "ComputePlatform.h"

#include <core/directed_acyclic_graph/Graph.h>
#include <core/directed_acyclic_graph/Node.h>

#include "ComputeModule.h"

#include <iostream>

using namespace core::compute_platform;
using namespace core::directed_acyclic_graph;
using namespace std;

ComputePlatform::ComputePlatform()
    : m_graph(Graph::create())
{}

void ComputePlatform::addModule(ComputeModule& module,
    core::directed_acyclic_graph::NodePtr node)
{
    m_modules.push_back(module);
    m_graph->add(node);
}

size_t ComputePlatform::size() const
{
    return m_modules.size();
}

void ComputePlatform::run()
{
    for (ComputeModule& module : m_modules) {
        module.reset();
    }

    auto t = m_graph->traverse();
    while (t.hasNext()) {
        t.next()->notified();
    }
}

bool ComputePlatform::checkCompleteness()
{
    for (ComputeModule& module : m_modules) {
        for (size_t i = 0; i < module.numInputs(); ++i) {
            bool con = module.inputPort(i).lock()->connected();
            if (con == false) {
                return false;
            }
        }
    }
    return true;
}