#include "catch.hpp"

#include <iostream>
#include <memory>
#include <tuple>

#include "compute_platform_test_helper.hpp"

using namespace core::compute_platform;
using namespace std;
using namespace compute_platform_test;

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
        REQUIRE(ds.outputPort(0).lock()->numBinds() == 1);
        REQUIRE(po.outputPort(0).lock()->numBinds() == 1);

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

        AND_WHEN("the a link is removed") {
            ds.outputPort(0).lock()->unbind(po.inputPort(0));
            THEN("the bind counter decreased") {
                REQUIRE(ds.outputPort(0).lock()->numBinds() == 0);
            }
            AND_WHEN("we try to run the net") {
                THEN("we get exception") {
                    REQUIRE_THROWS(p.run());
                }
            }
        }
    }
}

SCENARIO("basic usage with a more complex net", "[core/compute_platform]")
{
    GIVEN("a more complex net but also manipulating a simple int") {
        ComputePlatform p;
        
        //       +-------+
        //       |       v 
        // dd1<-ds1->po1->ad1->dd4
        //       |    | 
        //       v    v
        // ds2->ad2  dd3
        //  |    |
        //  v    v
        // dd2  dd5

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

SCENARIO("efficient data handling and leakage free checks with simple net", "[core/compute_platform]")
{
    GIVEN("a net manipulating complex data, the nodes forwards its input to the output") {
        weak_ptr<Data> weakData;
        {
            ComputePlatform p;
            
            DataSource ds(p);
            weakData = ds.giveWeakPtrToData();
            REQUIRE(weakData.lock());

            DataBypass po(p);
            DataSink dd(p);

            REQUIRE(p.size() == 3);
            REQUIRE(ds.outputPort(0).lock()->bind(po.inputPort(0)) == true);
            REQUIRE(po.outputPort(0).lock()->bind(dd.inputPort(0)) == true);

            // ds and weakData is refering the data
            REQUIRE(weakData.lock().use_count() == 2);

            WHEN("run is called") {
                p.run();
                THEN("the reference count increases") {
                    // ds.m_seed, ds.m_outputs, po.m_outputs, dd.m_outputs, dd.m_result
                    REQUIRE(weakData.lock().use_count() == 5);
                    REQUIRE(weakData.lock()->nextInstanceId() == 1);
                }
            }
        }
        WHEN("the platform is wiped out") {
            THEN("the data is cleaned") {
                REQUIRE(weakData.expired() == true);
            }
        }
    }
}

namespace compute_platform_test {

    vector<shared_ptr<ComputeModule>>
    addGrowingLayer(vector<shared_ptr<ComputeModule>> prevLayer,
        ComputePlatform& p, bool lastLayer)
    {
        vector<shared_ptr<ComputeModule>> result;
        if (prevLayer.empty()) {
            result.push_back(make_shared<DataSource>(p));
            return result;
        }
        
        if (!lastLayer) {
            auto side1 = make_shared<DataBypass>(p);
            auto side2 = make_shared<DataBypass>(p);

            REQUIRE(prevLayer[0]->outputPort(0).lock()->bind(side1->inputPort(0)) == true);
            REQUIRE(prevLayer[prevLayer.size()-1]->outputPort(0).lock()->bind(side2->inputPort(0)) == true);

            result.push_back(side1);
            for (int i = 0; i < (int)prevLayer.size() - 1; ++i) {
                auto m = make_shared<TwoInputBypass>(p);
                result.push_back(m);
                REQUIRE(prevLayer[i]->outputPort(0).lock()->bind(m->inputPort(0)) == true);
                REQUIRE(prevLayer[i + 1]->outputPort(0).lock()->bind(m->inputPort(1)) == true);
            }
            result.push_back(side2);

            return result;
        } else {
            auto side1 = make_shared<DataSink>(p);
            auto side2 = make_shared<DataSink>(p);

            REQUIRE(prevLayer[0]->outputPort(0).lock()->bind(side1->inputPort(0)) == true);
            REQUIRE(prevLayer[prevLayer.size()-1]->outputPort(0).lock()->bind(side2->inputPort(0)) == true);

            result.push_back(side1);
            for (int i = 0; i < (int)prevLayer.size() - 1; ++i) {
                auto m = make_shared<TwoInputSink>(p);
                result.push_back(m);
                REQUIRE(prevLayer[i]->outputPort(0).lock()->bind(m->inputPort(0)) == true);
                REQUIRE(prevLayer[i + 1]->outputPort(0).lock()->bind(m->inputPort(1)) == true);
            }
            result.push_back(side2);

            return result;
        }
    }
}

SCENARIO("efficient data handling and leakage free checks with complex net", "[core/compute_platform]")
{
    GIVEN("a net manipulating complex data, the nodes forwards its input to the output") {
        vector<weak_ptr<ComputeModule>> weakModules;
        {
            ComputePlatform p;
            vector<vector<shared_ptr<ComputeModule>>> modules;
            vector<shared_ptr<ComputeModule>> first;
            modules.push_back(addGrowingLayer(first, p, false));
            for (int i = 0; i < 3; ++i) {
                auto layer = addGrowingLayer(modules.back(), p, i == 2);
                modules.push_back(layer);
            }

            // S = source
            // M = module
            // SNK = sink
            //
            // S -> M -> M -> SNK
            // |    |    |
            // v    v    v
            // M -> M -> SNK
            // |    |
            // v    v
            // M -> SNK
            // |
            // v
            // SNK
            //
            // Data should be copied at every junction, and the originals should be carried through
            // resulting in 6 copies and 3 carried 'originals'

            REQUIRE(p.checkCompleteness() == true);

            WHEN("running the net") {
                p.run();
                THEN("Data should be instatiated 4 times") {
                    auto ptr = dynamic_pointer_cast<DataSource>(modules[0][0]);
                    auto data = ptr->giveWeakPtrToData().lock();
                    REQUIRE(data->nextInstanceId() == 9);
                }
            }
        }
        WHEN("the platform is wiped out") {
            THEN("the modules are cleaned") {
                for (auto module : weakModules) {
                    REQUIRE(module.lock() == nullptr);
                }
            }

            AND_THEN("the data is cleaned") {
                REQUIRE(Data::m_instancesAlive == 0);
            }
        }
    }
}

SCENARIO("disconnect ports", "[core/compute_platform]")
{
    GIVEN("a simple connected net") {
        ComputePlatform p;
        
        IntSource src1(p);
        IntSource src2(p);
        Add mid1(p);
        Add mid2(p);
        Add mid3(p);
        IntDestination dst1(p);
        IntDestination dst2(p);
        IntDestination dst3(p);

        REQUIRE(p.size() == 8);

        REQUIRE(connectPorts(src1, 0, mid1, 0) == true);
        REQUIRE(connectPorts(src1, 0, mid2, 1) == true);
        REQUIRE(connectPorts(src2, 0, mid1, 1) == true);
        REQUIRE(connectPorts(src2, 0, mid2, 0) == true);
        REQUIRE(connectPorts(src1, 0, mid3, 0) == true);
        REQUIRE(connectPorts(src1, 0, mid3, 1) == true);
        REQUIRE(connectPorts(mid1, 0, dst1, 0) == true);
        REQUIRE(connectPorts(mid2, 0, dst2, 0) == true);
        REQUIRE(connectPorts(mid3, 0, dst3, 0) == true);

        REQUIRE(src1.outputPort(0).lock()->numBinds() == 4);
        REQUIRE(src2.outputPort(0).lock()->numBinds() == 2);
        REQUIRE(mid1.outputPort(0).lock()->numBinds() == 1);
        REQUIRE(mid2.outputPort(0).lock()->numBinds() == 1);
        REQUIRE(mid3.outputPort(0).lock()->numBinds() == 1);

        src1.setData(1);
        src2.setData(2);

        WHEN("run is called") {
            p.run();
            THEN("it outputs the correct results") {
                REQUIRE(dst1.getResult() == 3);
                REQUIRE(dst2.getResult() == 3);
                REQUIRE(dst3.getResult() == 2);
            }
        }

        WHEN("some connections removed") {
            disconnectPorts(src1, 0, mid1, 0);
            // note that there is no such connection src2:0 -> mid1:0
            disconnectPorts(src2, 0, mid1, 0);
            disconnectPorts(mid3, 0, dst3, 0);

            THEN("the connections are removed") {
                REQUIRE(src1.outputPort(0).lock()->numBinds() == 3);
                REQUIRE(src2.outputPort(0).lock()->numBinds() == 2);
                REQUIRE(mid1.outputPort(0).lock()->numBinds() == 1);
                REQUIRE(mid2.outputPort(0).lock()->numBinds() == 1);
                REQUIRE(mid3.outputPort(0).lock()->numBinds() == 0);
            }

            AND_THEN("the net is not valid, so can not be run") {
                REQUIRE_THROWS(p.run());
            }
        }
    }
}