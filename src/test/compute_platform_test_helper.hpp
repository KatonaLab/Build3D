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

class Data : public std::enable_shared_from_this<Data> {
public:
    Data()
    {
        m_instanceId = instanceCounter++;
        ctrReport += std::to_string(m_instanceId) + " ";
        std::cout << "ctr " << m_instanceId << std::endl;
    }
    ~Data() {
        dtrReport += std::to_string(m_instanceId) + " ";
        std::cout << "dtr " << m_instanceId << std::endl;
    }
    static std::string ctrReport;
    static std::string dtrReport;
private:
    int m_instanceId = 0;
    static int instanceCounter;
};

int Data::instanceCounter = 0;
std::string Data::ctrReport;
std::string Data::dtrReport;

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

class DataSink : public cp::ComputeModule {
public:
    DataSink(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        // TODO: copy only the shared_ptr of input
        // m_result = m_inputs.input<0>()->value();
    }
protected:
    std::shared_ptr<Data> m_result;
    cp::TypedInputPortCollection<Data> m_inputs;
    cp::OutputPortCollection m_outputs;
};

#endif