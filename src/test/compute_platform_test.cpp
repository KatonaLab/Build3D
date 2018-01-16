#include "catch.hpp"

#include <iostream>
#include <memory>
#include <tuple>

#include <core/compute_platform/ports.hpp>

using namespace core::compute_platform;
using namespace std;

class ComputeModule {};

struct Data {
    int a, b, c;
};

class DummyModule : public ComputeModule,
    public InputPortCollection<int, Data> {
public:
//     void execute() override
//     {
//         output<0>() = input<0>() + input<1>().a;
//         input<1>().b = input<1>().a + input<1>().c;
//         output<1>() = input<0>() + input<1>().b;
//     }
};

SCENARIO("intuitive usage", "[core/compute_platform]")
{
    DummyModule module;
    cout << module.input(0)->type() << endl;
    cout << module.input(1)->type() << endl;

    // GIVEN("some dummy compute module") {
    //     ComputePlatform platform;

    //     DummySourceInt intSource(platform);
    //     DummySourceData dataSource(platform);
    //     DummyModule module(platform);
    //     DummySink sink(platform);

    //     module.inputPort(0).set(intSource.outBufferPort(0));
    //     module.inputPort(1).set(dataSource.outBufferPort(0))
    //     sink.inputPort(0).set(module.outBufferPort(0));
    //     sink.inputPort(1).set(module.outBufferPort(1));

    //     platform.initialize();
    //     platform.process();
    // }
}