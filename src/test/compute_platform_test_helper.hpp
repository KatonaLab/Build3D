#ifndef _test_compute_platform_test_helper_hpp_
#define _test_compute_platform_test_helper_hpp_

#include <iostream>
#include <memory>
#include <string>

#include <core/compute_platform/ports.h>
#include <core/compute_platform/port_utils.hpp>
#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>

namespace cp = core::compute_platform;

namespace compute_platform_test {

class IntSource : public cp::ComputeModule {
public:
    IntSource(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_seed(std::make_shared<int>()),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_outputs.output<0>()->forwardFromSharedPtr(m_seed);
    }
    void setData(int x)
    {
        *m_seed = x;
    }
protected:
    std::shared_ptr<int> m_seed;
    cp::InputPortCollection m_inputs;
    cp::TypedOutputPortCollection<int> m_outputs;
};

class PlusOne : public cp::ComputeModule {
public:
    PlusOne(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_inputs.input<0>()->value() += 1;
        m_outputs.output<0>()->forwardFromInput(m_inputs.input<0>());
    }
protected:
    cp::TypedInputPortCollection<int> m_inputs;
    cp::TypedOutputPortCollection<int> m_outputs;
};

class Add : public cp::ComputeModule {
public:
    Add(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_inputs.input<0>()->value() += m_inputs.input<1>()->value();
        m_outputs.output<0>()->forwardFromInput(m_inputs.input<0>());
    }
protected:
    cp::TypedInputPortCollection<int, int> m_inputs;
    cp::TypedOutputPortCollection<int> m_outputs;
};

class IntDestination : public cp::ComputeModule {
public:
    IntDestination(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_store = m_inputs.input<0>()->value();
    }
    int getResult()
    {
        return m_store;
    }
protected:
    cp::TypedInputPortCollection<int> m_inputs;
    cp::OutputPortCollection m_outputs;
    int m_store;
};

class Data {
public:
    Data()
    {
        m_instanceId = m_nextInstanceId++;
        m_instancesAlive++;
    }
    Data(const Data&)
    {
        m_instanceId = m_nextInstanceId++;
        m_instancesAlive++;
    }
    ~Data()
    {
        m_instancesAlive--;
    }
    static int nextInstanceId() {
        return m_nextInstanceId;
    }
public:
    int m_instanceId = 0;
    static int m_instancesAlive;
private:
    static int m_nextInstanceId;
};

int Data::m_nextInstanceId = 0;
int Data::m_instancesAlive = 0;

class DataSource : public cp::ComputeModule {
public:
    DataSource(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_seed(std::make_shared<Data>()),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_outputs.output<0>()->forwardFromSharedPtr(m_seed);
    }
    std::weak_ptr<Data> giveWeakPtrToData()
    {
        return m_seed;
    }
protected:
    std::shared_ptr<Data> m_seed;
    cp::InputPortCollection m_inputs;
    cp::TypedOutputPortCollection<Data> m_outputs;
};

class DataBypass : public cp::ComputeModule {
public:
    DataBypass(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_outputs.output<0>()->forwardFromInput(m_inputs.input<0>());
    }
protected:
    cp::TypedInputPortCollection<Data> m_inputs;
    cp::TypedOutputPortCollection<Data> m_outputs;
};

class TwoInputBypass : public cp::ComputeModule {
public:
    TwoInputBypass(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_outputs.output<0>()->forwardFromInput(m_inputs.input<0>());
    }
protected:
    cp::TypedInputPortCollection<Data, Data> m_inputs;
    cp::TypedOutputPortCollection<Data> m_outputs;
};

class DataSink : public cp::ComputeModule {
public:
    DataSink(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_result = m_inputs.input<0>()->inputPtr().lock();
    }
protected:
    std::shared_ptr<Data> m_result;
    cp::TypedInputPortCollection<Data> m_inputs;
    cp::OutputPortCollection m_outputs;
};

class TwoInputSink : public cp::ComputeModule {
public:
    TwoInputSink(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_result = m_inputs.input<0>()->inputPtr().lock();
    }
protected:
    std::shared_ptr<Data> m_result;
    cp::TypedInputPortCollection<Data, Data> m_inputs;
    cp::OutputPortCollection m_outputs;
};

}

#endif