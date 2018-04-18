#ifndef _core_compute_platform_ComputeModule_h_
#define _core_compute_platform_ComputeModule_h_

#include "ports.h"
#include <cstddef>
#include <core/directed_acyclic_graph/Node.h>

namespace core {
namespace compute_platform {

    class TriggerNode : public core::directed_acyclic_graph::Node {
    public:
        TriggerNode(ComputeModule& parent);
        void notified() override;
        ComputeModule& parent();
    private:
        ComputeModule& m_parent;
    };

    class ComputePlatform;

    class ComputeModule {
    public:
        void evaluate();
        std::size_t numInputs() const;
        std::size_t numOutputs() const;
        std::weak_ptr<InputPort> inputPort(std::size_t id);
        std::weak_ptr<OutputPort> outputPort(std::size_t id);
        core::directed_acyclic_graph::NodePtr node();
        void reset();
        virtual ~ComputeModule() = default;
    protected:
        ComputeModule(ComputePlatform& parent,
            InputPortCollection& inputs,
            OutputPortCollection& outputs);
        virtual void execute() = 0;
    private:
        ComputePlatform& m_parent;
        InputPortCollection& m_inputs;
        OutputPortCollection& m_outputs;
        std::shared_ptr<TriggerNode> m_node;
    };
    
    bool connectPorts(ComputeModule& outputModule, std::size_t outputId,
        ComputeModule& inputModule, std::size_t inputId);

    // TODO: write test for disconnectPorts
    void disconnectPorts(ComputeModule& outputModule, std::size_t outputId,
        ComputeModule& inputModule, std::size_t inputId);
}}

#endif
