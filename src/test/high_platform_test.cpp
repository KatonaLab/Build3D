#include "catch.hpp"
#include <pybind11/embed.h> // everything needed for embedding

// #include <core/high_platform/Platform.hpp>
// #include <core/compute_platform/PythonComputeModule.h>

// using namespace core::high_platform;
using namespace std;

string codeSource = R"code(
import a3dc
import numpy

a3dc.set_module_meta({
    inputs: {},
    outputs: {'out_image': a3dc.types.image}})

def module_main():
    return {'out_image': np.ones((320, 240))}

)code";

string codeAdd = R"code(
import a3dc

a3dc.set_module_meta({
    inputs: {'image1': a3dc.types.image, 'image2': a3dc.types.image},
    outputs: {'out_image': a3dc.types.image}})

def module_main(image1, image2):
    return {'out_image': image1 + image2}

)code";

string codeTarget = R"code(
import a3dc

a3dc.set_module_meta({
    inputs: {'image': a3dc.types.image},
    outputs: {}})

def module_main():
    return None

)code";


namespace py = pybind11;

SCENARIO("test python", "[core/high_platform]")
{
    Py_SetProgramName((wchar_t*)L"/Users/fodorbalint/projects/a3dc/virtualenv/bin/python");

    Py_SetPythonHome((wchar_t*)L"/Users/fodorbalint/projects/a3dc/virtualenv/bin:"
        "/Users/fodorbalint/projects/a3dc/virtualenv/lib:"
        "/Users/fodorbalint/projects/a3dc/virtualenv/lib/python3.6:"
        "/Users/fodorbalint/projects/a3dc/virtualenv/lib/python3.6/lib-dynload:"
        "/Users/fodorbalint/projects/a3dc/virtualenv/lib/python3.6/site-packages");

    string code = R"(
import sys
import os
print(sys.version)
print(sys.executable)
print(sys.path)
print(os.sys.path)
# sys.path.append('/Users/fodorbalint/projects/a3dc/virtualenv/lib/python3.6/site-packages')
# print(sys.path)
# print(os.sys.path)
import pandas
    )";
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive
    py::exec(code); // use the Python API
}

SCENARIO("high_platform basic usage", "[core/high_platform]")
{
    // GIVEN("a simple net") {
    //     ComputePlatform p;

    //     PythonComputeModule src1(p, codeSource);
    //     PythonComputeModule src2(p, codeSource);
    //     PythonComputeModule add(p, codeAdd);
    //     PythonComputeModule dst(p, codeTarget);

    //     REQUIRE(p.size() == 3);

    //     REQUIRE(src1.outputPort(0).lock()->bind(add.inputPort(0)) == true);
    //     REQUIRE(src2.outputPort(0).lock()->bind(add.inputPort(1)) == true);
    //     REQUIRE(add.outputPort(0).lock()->bind(dst.inputPort(0)) == true);

    //     WHEN("run is called") {
    //         p.run();
    //         THEN("it outputs the correct result") {
    //             //REQUIRE(dd.getResult() == 43);
    //         }
    //     }
    // }
}