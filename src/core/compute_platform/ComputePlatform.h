#ifndef _core_compute_platform_ComputePlatform_h_
#define _core_compute_platform_ComputePlatform_h_

#include <list>

#include <core/directed_acyclic_graph/types.h>

namespace core {
namespace compute_platform {

class ComputeModule;

class ComputePlatform {
public:
    ComputePlatform();
    void addModule(ComputeModule& module,
        core::directed_acyclic_graph::NodePtr node);
    // TODO: removeModule
    size_t size() const;
    void run();
    bool checkCompleteness();
private:
    std::list<std::reference_wrapper<ComputeModule>> m_modules;
    core::directed_acyclic_graph::GraphPtr m_graph;
};

}}

#endif
