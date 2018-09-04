#ifndef _core_compute_platform_ComputePlatform_h_
#define _core_compute_platform_ComputePlatform_h_

#include <list>

#include <core/directed_acyclic_graph/types.h>
#include "ModuleContext.h"
#include <vector>

namespace core {
namespace compute_platform {

class ComputeModule;

class ComputePlatform {
    friend class ComputeModule;
public:
    ComputePlatform();
    // TODO: removeModule <- NOTE: it is done when the ComputeModule instance is destroyed
    // TODO: should be managed explicilty, not just through module ctr/dctr ?
    size_t size() const;
    std::vector<ModuleContext> run(ModuleContext ctx = ModuleContext());
    bool checkCompleteness();
    // TODO: test this function
    void printModuleConnections();
protected:
    void addModule(ComputeModule& module, core::directed_acyclic_graph::NodePtr node);
    void removeModule(ComputeModule& toBeRemoved);
private:
    std::list<std::reference_wrapper<ComputeModule>> m_modules;
    core::directed_acyclic_graph::GraphPtr m_graph;
};

}}

#endif
