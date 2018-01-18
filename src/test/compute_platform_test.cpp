#include "catch.hpp"

#include <iostream>
#include <memory>
#include <tuple>

#include "compute_platform_test_helper.hpp"

using namespace core::compute_platform;
using namespace std;

SCENARIO("basic usage", "[core/compute_platform]")
{
    GIVEN("a simple net manipulating a simple int") {
        ComputePlatform p;
        
        IntSource ds(p);
        PlusOne po(p);
        IntDestination dd(p);

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
        
        IntSource ds1(p);
        IntSource ds2(p);
        PlusOne po1(p);
        Add ad1(p);
        Add ad2(p);
        IntDestination dd1(p);
        IntDestination dd2(p);
        IntDestination dd3(p);
        IntDestination dd4(p);
        IntDestination dd5(p);

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

SCENARIO("leakage free", "[core/compute_platform]")
{
    GIVEN("a simple net manipulating complex data") {
        ComputePlatform p;
        
        DataSource ds(p);
        DataBypass po(p);
        DataSink dd(p);

        REQUIRE(p.size() == 3);

        REQUIRE(ds.outputPort(0).lock()->bind(po.inputPort(0)) == true);
        REQUIRE(po.outputPort(0).lock()->bind(dd.inputPort(0)) == true);

        WHEN("run is called") {
            p.run();
        }
        // TODO: check Data is created only once
        // TODO: check in a more complex network Data and check for cleanup after running
    }
    // cout << Data::ctrReport << endl;
    // cout << Data::dtrReport << endl;
}