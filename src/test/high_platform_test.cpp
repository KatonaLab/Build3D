#include "catch.hpp"
#include "high_platform_test_helper.hpp"
#include <core/compute_platform/ComputeModule.h>
#include <core/high_platform/PythonComputeModule.h>
#include <core/multidim_image_platform/MultiDimImage.hpp>

#include <memory>

using namespace core::high_platform;
using namespace core::compute_platform;
using namespace core::multidim_image_platform;
using namespace std;

namespace high_platform_test {

    vector<shared_ptr<ComputeModule>>
    addGrowingLayer(vector<shared_ptr<ComputeModule>> prevLayer,
        ComputePlatform& p, bool lastLayer)
    {
        vector<shared_ptr<ComputeModule>> result;
        if (prevLayer.empty()) {
            result.push_back(make_shared<ImageSource>(p));
            return result;
        }
        
        if (!lastLayer) {
            auto side1 = make_shared<ImageBypass>(p);
            auto side2 = make_shared<ImageBypass>(p);

            REQUIRE(connectPorts(*prevLayer[0], 0, *side1, 0) == true);
            REQUIRE(connectPorts(*prevLayer[prevLayer.size()-1], 0, *side2, 0) == true);

            result.push_back(side1);
            for (int i = 0; i < (int)prevLayer.size() - 1; ++i) {
                auto m = make_shared<TwoImageBypass>(p);
                result.push_back(m);
                REQUIRE(connectPorts(*prevLayer[i], 0, *m, 0) == true);
                REQUIRE(connectPorts(*prevLayer[i + 1], 0, *m, 1) == true);
            }
            result.push_back(side2);

            return result;
        } else {
            auto side1 = make_shared<ImageSink>(p);
            auto side2 = make_shared<ImageSink>(p);

            REQUIRE(connectPorts(*prevLayer[0], 0, *side1, 0) == true);
            REQUIRE(connectPorts(*prevLayer[prevLayer.size()-1], 0, *side2, 0) == true);

            result.push_back(side1);
            for (int i = 0; i < (int)prevLayer.size() - 1; ++i) {
                auto m = make_shared<TwoImageSink>(p);
                result.push_back(m);
                REQUIRE(connectPorts(*prevLayer[i], 0, *m, 0) == true);
                REQUIRE(connectPorts(*prevLayer[i + 1], 0, *m, 1) == true);
            }
            result.push_back(side2);

            return result;
        }
    }
}

using namespace high_platform_test;

SCENARIO("compute platform works with multidim images", "[core/high_platform]")
{
    GIVEN("a net manipulating multidim images, the nodes forwards its input to the output") {
        ComputePlatform p;
        vector<vector<shared_ptr<ComputeModule>>> modules;
        vector<shared_ptr<ComputeModule>> first;
        modules.push_back(high_platform_test::addGrowingLayer(first, p, false));
        for (int i = 0; i < 3; ++i) {
            auto layer = high_platform_test::addGrowingLayer(modules.back(), p, i == 2);
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

        Image im({64, 64, 3});
        im.at({11, 17, 0}) = 42.0f;
        dynamic_pointer_cast<ImageSource>(modules[0][0])->setImage(im);

        WHEN("running the net") {
            p.run();

            THEN("all the outputs contain the same value") {
                REQUIRE(dynamic_pointer_cast<ImageSink>(modules[3][0])->getImage().at({11, 17, 0}) == 42.0f);
                REQUIRE(dynamic_pointer_cast<TwoImageSink>(modules[3][1])->getImage().at({11, 17, 0}) == 42.0f);
                REQUIRE(dynamic_pointer_cast<TwoImageSink>(modules[3][2])->getImage().at({11, 17, 0}) == 42.0f);
                REQUIRE(dynamic_pointer_cast<ImageSink>(modules[3][3])->getImage().at({11, 17, 0}) == 42.0f);
            }
        }
    }
}

namespace py = pybind11;

SCENARIO("test pybind11 python binding: a3dc.MultiDimImage", "[core/high_platform]")
{
    GIVEN("a 2d MultiDimImage instanced on cpp side") {
        PythonEnvironment::instance();

        auto im = std::shared_ptr<MultiDimImage<uint8_t>>(new MultiDimImage<uint8_t>({32, 32}));
        im->at({17, 19}) = 42;
        WHEN("passed to python side") {
            using namespace py::literals;
            auto locals = py::dict("x"_a = im);
            THEN("it is passed correctly") {
                py::exec(R"(
                    y = x[17, 19] == 42
                    x[17, 19] = 255
                )", py::globals(), locals);
                
                bool y = locals["y"].cast<bool>();
                REQUIRE(y == true);
                REQUIRE(im->at({17, 19}) == 255);
            }
        }
    }

    GIVEN("a 2d MultiDimImage instanced on python side") {
        PythonEnvironment::instance();
        
        auto locals = py::dict();
        py::exec(R"(
            import a3dc
            x = a3dc.MultiDimImageUInt8([16, 16])
            x[7, 9] = 42
        )", py::globals(), locals);
        
        WHEN("passed to the cpp side") {
            THEN("it is passed correctly") {
                auto im = locals["x"].cast<MultiDimImage<uint8_t>>();
                REQUIRE(im.at({7, 9}) == 42);
            }
        }
    }

    GIVEN("a nd MultiDimImage instanced on cpp side") {
        PythonEnvironment::instance();

        auto im = std::shared_ptr<MultiDimImage<uint8_t>>(new MultiDimImage<uint8_t>({32, 32, 3, 4}));
        im->at({17, 19, 1, 2}) = 42;
        WHEN("passed to python side") {
            using namespace py::literals;
            auto locals = py::dict("x"_a = im);
            THEN("it is passed correctly") {
                py::exec(R"(
                    import numpy as np
                    p = x.plane([1, 2])
                    y = p[17, 19] == 42
                    p[17, 19] = 255
                )", py::globals(), locals);
                
                bool y = locals["y"].cast<bool>();
                REQUIRE(y == true);
                REQUIRE(im->at({17, 19, 1, 2}) == 255);
            }
        }
    }

    GIVEN("a nd MultiDimImage instanced on python side") {
        PythonEnvironment::instance();
        
        auto locals = py::dict();
        py::exec(R"(
            import a3dc
            import numpy as np
            x = a3dc.MultiDimImageUInt8([16, 16, 3, 4])
            p = x.plane([1, 2])
            p[7, 9] = 42
        )", py::globals(), locals);
        
        WHEN("passed to the cpp side") {
            THEN("it is passed correctly") {
                auto im = locals["x"].cast<MultiDimImage<uint8_t>>();
                REQUIRE(im.at({7, 9, 1, 2}) == 42);
            }
        }
    }

    // TODO: test: Meta item read/write
}

SCENARIO("high_platform source py module test", "[core/high_platform]")
{
    string codeSource =
    R"(
import a3dc

def module_main():
    im = a3dc.MultiDimImageFloat([16, 24])
    im[7, 23] = 42
    a3dc.outputs['out_image'] = im

a3dc.def_process_module({}, {'out_image': a3dc.types.ImageFloat}, module_main)
    )";

    GIVEN("a simple net") {
        ComputePlatform p;

        PythonComputeModule source(p, codeSource);
        ImageSink imSink(p);

        REQUIRE(source.outputPort(0).lock()->bind(imSink.inputPort(0)) == true);

        WHEN("run is called") {
            p.run();
            THEN("it outputs the correct result") {
                REQUIRE(imSink.getImage().at({7, 23}) == 42);
            }
        }
    }
}

SCENARIO("high_platform sink py module test", "[core/high_platform]")
{
    string codeSource =
    R"(
import a3dc

def module_main():
    im = a3dc.inputs['in_image']
    if im[7, 23] != 42:
        raise Exception('im[7, 23] != 42')

a3dc.def_process_module({'in_image': a3dc.types.ImageFloat}, {}, module_main)
    )";

    GIVEN("a simple net") {
        ComputePlatform p;

        MultiDimImage<float> im({32, 32});
        im.at({7, 23}) = 42;
        ImageSource source(p);
        source.setImage(im);
        PythonComputeModule sink(p, codeSource);

        REQUIRE(source.outputPort(0).lock()->bind(sink.inputPort(0)) == true);

        WHEN("run is called") {
            p.run();
            THEN("it runs correctly") {
            }
        }
    }
}