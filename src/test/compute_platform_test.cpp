#include "catch.hpp"

#include <memory>
#include <tuple>

// #include <core/compute_layer/ComputeNode.h>
// #include <core/compute_layer/Buffer.h>

// using namespace core;
using namespace std;

// class ComputeModule {};

class OutputBufferPort {
public:
    virtual ~OutputBufferPort() {}
};

class InputPort {
public:
    virtual void set(OutputBufferPort& port) = 0;
    virtual ~InputPort() {}
};

template <typename T>
class TypedOutputBufferPort : public OutputBufferPort {
public:
    std::shared_ptr<T> m_data;
};

template <typename T>
class TypedInputPort : public InputPort {
public:
    bool set(OutputBufferPort& port) override
    {
         if (dynamic_cast<TypedOutputBufferPort<T>*>(&port) != nullptr) {
             m_ptr = ((TypedOutputBufferPort<T>*)(&port))->m_data;
             return true;
         }
         return false;
    }
private:
    std::weak_ptr<T> m_ptr;
};

struct Data {
    int a, b, c;
};

// class DummyModule : public ComputeModule,
//     public InputPorts<DummyModule, int, Data>,
//     public OutputPorts<DummyModule, int, Data> {
// public:
//     void execute() override
//     {
//         output<0>() = input<0>() + input<1>().a;
//         input<1>().b = input<1>().a + input<1>().c;
//         output<1>() = input<0>() + input<1>().b;
//     }
// };

SCENARIO("intuitive usage", "[core/compute_platform]")
{
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