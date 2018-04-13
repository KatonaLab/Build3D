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
                    y = p[19, 17] == 42
                    p[19, 17] = 255
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
            p[9, 7] = 42
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

a3dc.def_process_module([], [], [a3dc.Arg('out_image', a3dc.types.ImageFloat)], module_main)
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
from a3dc import Arg

def module_main():
    im = a3dc.inputs['in_image']
    if im[7, 23] != 42:
        raise Exception('im[7, 23] != 42')

a3dc.def_process_module([Arg('in_image', a3dc.types.ImageFloat)], [], [], module_main)
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

SCENARIO("high_platform common py module test", "[core/high_platform]")
{
    string codeSource =
    R"(
import a3dc
from a3dc import Arg

def module_main():
    im = a3dc.inputs['in_image']
    im[7, 23] += 42
    im[7, 24] = a3dc.inputs['in_int_param']
    im[7, 25] = a3dc.inputs['in_double_param']

    a3dc.outputs['out_image'] = im
    a3dc.outputs['out_uint_param'] = 42000000
    a3dc.outputs['out_float_param'] = 2024.2024

inputs = [
    Arg('in_image', a3dc.types.ImageFloat),
    Arg('in_int_param', a3dc.types.int64),
    Arg('in_double_param', a3dc.types.double)]
params = []
outputs = [
    Arg('out_image', a3dc.types.ImageFloat),
    Arg('out_uint_param', a3dc.types.uint64),
    Arg('out_float_param', a3dc.types.float)]
a3dc.def_process_module(inputs, params, outputs, module_main)
    )";

    GIVEN("a simple net") {
        ComputePlatform p;

        MultiDimImage<float> im({32, 32});
        im.at({7, 23}) = 42;
        ImageSource imSource(p);
        NumberSource<int64_t> intSource(p);
        NumberSource<double> doubleSource(p);

        PythonComputeModule module(p, codeSource);

        ImageSink imSink(p);
        NumberSink<uint64_t> intSink(p);
        NumberSink<float> floatSink(p);

        imSource.setImage(im);
        intSource.setNumber(420000);
        doubleSource.setNumber(3.1415);

        REQUIRE(connectPorts(imSource, 0, module, 0) == true);
        REQUIRE(connectPorts(intSource, 0, module, 1) == true);
        REQUIRE(connectPorts(doubleSource, 0, module, 2) == true);
        REQUIRE(connectPorts(module, 0, imSink, 0) == true);
        REQUIRE(connectPorts(module, 1, intSink, 0) == true);
        REQUIRE(connectPorts(module, 2, floatSink, 0) == true);

        WHEN("run is called") {
            p.run();
            THEN("it runs correctly") {
                REQUIRE(imSink.getImage().at({7, 23}) == 84);
                REQUIRE(imSink.getImage().at({7, 24}) == 420000);
                REQUIRE(imSink.getImage().at({7, 25}) == 3.1415f);
                REQUIRE(intSink.getNumber() == 42000000);
                REQUIRE(floatSink.getNumber() == 2024.2024f);
            }
        }
    }
}

SCENARIO("high_platform complex py module test", "[core/high_platform]")
{
    string generatorCode =
    R"(
import a3dc
import numpy as np

def module_main():
    im = a3dc.MultiDimImageUInt8([1024, 1024, 32])
    for i in range(32):
        p = np.array(im.plane([i]), copy=False)
        p[:, :] = 42
    a3dc.outputs['out_image'] = im

inputs = []
outputs = [a3dc.Arg('out_image', a3dc.types.ImageUInt8)]
a3dc.def_process_module(inputs, [], outputs, module_main)
    )";

    string incrementCode =
    R"(
import a3dc
import numpy as np

def module_main():
    im = a3dc.inputs['in_image']
    for i in range(im.dims()[2]):
        p = np.array(im.plane([i]), copy=False)
        p += 1
    a3dc.outputs['out_image'] = im

inputs = [a3dc.Arg('in_image', a3dc.types.ImageUInt8)]
outputs = [a3dc.Arg('out_image', a3dc.types.ImageUInt8)]
a3dc.def_process_module(inputs, [], outputs, module_main)
    )";

    string addCode =
    R"(
import a3dc
from a3dc import Arg
import numpy as np

def module_main():
    im1 = a3dc.inputs['in_image1']
    im2 = a3dc.inputs['in_image2']
    out = a3dc.MultiDimImageUInt8(im1.dims())
    for i in range(im1.dims()[2]):
        s = im1.plane([i]) + im2.plane([i])
        out.set_plane([i], s)
        
    a3dc.outputs['out_image'] = out

inputs = [
    Arg('in_image1', a3dc.types.ImageUInt8),
    Arg('in_image2', a3dc.types.ImageUInt8)]
outputs = [Arg('out_image', a3dc.types.ImageUInt8)]
a3dc.def_process_module(inputs, [], outputs, module_main)
    )";

    string sinkCode =
    R"(
import a3dc
import numpy as np

def module_main():
    im = a3dc.inputs['in_image']
    for i in range(im.dims()[2]):
        p = np.array(im.plane([i]), copy=False)
        if not np.all(p == a3dc.inputs['require']):
            raise Exception('image value is not the required one')

inputs = [a3dc.Arg('in_image', a3dc.types.ImageUInt8),
    a3dc.Arg('require', a3dc.types.uint8)]
outputs = []
a3dc.def_process_module(inputs, [], outputs, module_main)
    )";

    GIVEN("a simple net") {
        ComputePlatform p;

        PythonComputeModule src1(p, generatorCode);

        PythonComputeModule inc1(p, incrementCode);
        PythonComputeModule inc2(p, incrementCode);
        PythonComputeModule inc3(p, incrementCode);
        PythonComputeModule inc4(p, incrementCode);
        PythonComputeModule inc5(p, incrementCode);

        PythonComputeModule add1(p, addCode);
        PythonComputeModule add2(p, addCode);

        PythonComputeModule sink1(p, sinkCode);
        PythonComputeModule sink2(p, sinkCode);

        NumberSource<uint8_t> r1(p);
        NumberSource<uint8_t> r2(p);

        REQUIRE(connectPorts(src1, 0, inc1, 0) == true);
        REQUIRE(connectPorts(src1, 0, inc2, 0) == true);
        REQUIRE(connectPorts(src1, 0, inc3, 0) == true);

        REQUIRE(connectPorts(inc2, 0, inc5, 0) == true);

        REQUIRE(connectPorts(inc1, 0, add1, 0) == true);
        REQUIRE(connectPorts(inc5, 0, add1, 1) == true);

        REQUIRE(connectPorts(add1, 0, sink1, 0) == true);
        REQUIRE(connectPorts(add1, 0, inc4, 0) == true);

        REQUIRE(connectPorts(inc4, 0, add2, 0) == true);
        REQUIRE(connectPorts(inc3, 0, add2, 1) == true);

        REQUIRE(connectPorts(add2, 0, sink2, 0) == true);

        REQUIRE(connectPorts(r1, 0, sink1, 1) == true);
        REQUIRE(connectPorts(r2, 0, sink2, 1) == true);

        r1.setNumber(87);
        r2.setNumber(131);

        WHEN("run is called") {
            p.run();
            THEN("it runs correctly") {
            }
        }
    }
}
