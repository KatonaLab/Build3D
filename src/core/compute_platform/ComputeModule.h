#ifndef _core_compute_platform_ComputeModule_h_
#define _core_compute_platform_ComputeModule_h_

#include "ports.h"

namespace core {
namespace compute_platform {

    class ComputePlatform;

    class ComputeModule {
    public:
        void evaluate();
        size_t numInputs() const;
        size_t numOutputs() const;
        std::weak_ptr<InputPort> inputPort(size_t id);
        std::weak_ptr<OutputPort> outputPort(size_t id);
    protected:
        ComputeModule(ComputePlatform& parent,
            InputPortCollection& inputs,
            OutputPortCollection& outputs);
        virtual void execute() = 0;
    private:
        ComputePlatform& m_parent;
        InputPortCollection& m_inputs;
        OutputPortCollection& m_outputs;
    };
    
}}

#endif
