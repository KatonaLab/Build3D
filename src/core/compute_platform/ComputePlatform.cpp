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

void ComputePlatform::removeModule(ComputeModule& toBeRemoved)
{
    auto it = find_if(m_modules.begin(), m_modules.end(),
        [&toBeRemoved](const std::reference_wrapper<ComputeModule>& item)
        {
            return &(item.get()) == &toBeRemoved;
        });

    if (it == m_modules.end()) {
        // TODO: give a warning (?), no module found
        return;
    }

    m_graph->remove(it->get().node());
    m_modules.erase(it);
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

// TODO: test this function
void ComputePlatform::printModuleConnections()
{
    // TODO: don't use cout, make it print into an ostream
    // or do the operator<< way
    for (auto& mr : m_modules) {
        auto& m = mr.get();
        cout << "module " << m.name() << endl;
        for (size_t i = 0; i < m.numInputs(); ++i) {
            auto inPort = m.inputPort(i).lock();
            if (inPort->getSource().lock()) {
                cout << "\t" << inPort->getSource().lock()->parent().name()
                    << ":" << inPort->getSource().lock()->name() << " --> "
                    << inPort->name() << endl;
            } else {
                cout << "\t" << inPort->name() << endl;
            }
        }

        for (size_t i = 0; i < m.numOutputs(); ++i) {
            auto outPort = m.outputPort(i).lock();
            if (outPort->numBinds()) {
                cout << "\t" << outPort->name() << " --> " << outPort->numBinds() << endl;
            } else {
                cout << "\t" << outPort->name() << endl;
            }
        }
    }
}