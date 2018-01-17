#include "catch.hpp"

#include <iostream>
#include <memory>
#include <tuple>

#include <core/compute_platform/ports.h>
#include <core/compute_platform/port_utils.hpp>
#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>

using namespace core::compute_platform;
using namespace std;

class DataSource : public ComputeModule {
public:
    DataSource(ComputePlatform& parent)
        : ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_outputs.output<0>() = m_seed;
    }
    void setData(int x)
    {
        m_seed = x;
    }
protected:
    int m_seed = 0;
    InputPortCollection m_inputs;
    TypedOutputPortCollection<int> m_outputs;
};

class PlusOne : public ComputeModule {
public:
    PlusOne(ComputePlatform& parent)
        : ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_outputs.output<0>() = m_inputs.input<0>() + 1;
    }
protected:
    TypedInputPortCollection<int> m_inputs;
    TypedOutputPortCollection<int> m_outputs;
};

class DataDestination : public ComputeModule {
public:
    DataDestination(ComputePlatform& parent)
        : ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_store = m_inputs.input<0>();
    }
    int getResult()
    {
        return m_store;
    }
protected:
    TypedInputPortCollection<int> m_inputs;
    OutputPortCollection m_outputs;
    int m_store;
};

SCENARIO("intuitive usage", "[core/compute_platform]")
{
    GIVEN("a simple net") {
        ComputePlatform p;
        
        DataSource ds(p);
        PlusOne po(p);
        DataDestination dd(p);

        REQUIRE(p.size() == 3);

        REQUIRE(ds.outputPort(0).lock()->bind(po.inputPort(0)) == true);
        REQUIRE(po.outputPort(0).lock()->bind(dd.inputPort(0)) == true);

        ds.setData(42);

        WHEN("run") {
            p.run();
            THEN("it outputs the correct result") {
                REQUIRE(dd.getResult() == 43);
            }
        }
    }
}