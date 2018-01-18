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

class Add : public ComputeModule {
public:
    Add(ComputePlatform& parent)
        : ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_outputs.output<0>() = m_inputs.input<0>() + m_inputs.input<1>();
    }
protected:
    TypedInputPortCollection<int, int> m_inputs;
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
    GIVEN("a simple net manipulating a simple int") {
        ComputePlatform p;
        
        DataSource ds(p);
        PlusOne po(p);
        DataDestination dd(p);

        REQUIRE(p.size() == 3);

        REQUIRE(ds.outputPort(0).lock()->bind(po.inputPort(0)) == true);
        REQUIRE(po.outputPort(0).lock()->bind(dd.inputPort(0)) == true);

        ds.setData(42);

        WHEN("run is called") {
            p.run();
            THEN("it outputs the correct result") {
                REQUIRE(dd.getResult() == 43);
            }
        }

        AND_WHEN("run called again") {
            p.run();
            THEN("it outputs the correct result again") {
                REQUIRE(dd.getResult() == 43);
            }
        }
    }

    GIVEN("a more complex net but also manipulating a simple int") {
        ComputePlatform p;
        
        DataSource ds1(p);
        DataSource ds2(p);
        PlusOne po1(p);
        Add ad1(p);
        Add ad2(p);
        DataDestination dd1(p);
        DataDestination dd2(p);
        DataDestination dd3(p);
        DataDestination dd4(p);
        DataDestination dd5(p);

        REQUIRE(p.size() == 10);

        REQUIRE(ds1.outputPort(0).lock()->bind(po1.inputPort(0)) == true);
        REQUIRE(po1.outputPort(0).lock()->bind(ad1.inputPort(0)) == true);
        REQUIRE(ds1.outputPort(0).lock()->bind(ad1.inputPort(1)) == true);
        REQUIRE(ds2.outputPort(0).lock()->bind(ad2.inputPort(0)) == true);
        REQUIRE(po1.outputPort(0).lock()->bind(ad2.inputPort(1)) == true);
        
        REQUIRE(ds1.outputPort(0).lock()->bind(dd1.inputPort(0)) == true);
        REQUIRE(ds2.outputPort(0).lock()->bind(dd2.inputPort(0)) == true);
        REQUIRE(po1.outputPort(0).lock()->bind(dd3.inputPort(0)) == true);
        REQUIRE(ad1.outputPort(0).lock()->bind(dd4.inputPort(0)) == true);
        REQUIRE(ad2.outputPort(0).lock()->bind(dd5.inputPort(0)) == true);

        ds1.setData(42);
        ds2.setData(2024);

        WHEN("run") {
            p.run();
            THEN("it outputs the correct result") {
                REQUIRE(dd1.getResult() == 42);
                REQUIRE(dd2.getResult() == 2024);
                REQUIRE(dd3.getResult() == 43);
                REQUIRE(dd4.getResult() == 85);
                REQUIRE(dd5.getResult() == 2067);
            }
        }

        AND_WHEN("run again") {
            p.run();
            THEN("it outputs the correct result") {
                REQUIRE(dd1.getResult() == 42);
                REQUIRE(dd2.getResult() == 2024);
                REQUIRE(dd3.getResult() == 43);
                REQUIRE(dd4.getResult() == 85);
                REQUIRE(dd5.getResult() == 2067);
            }
        }
    }
}