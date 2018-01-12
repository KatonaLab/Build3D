#include "catch.hpp"

#include <memory>
#include <tuple>

#include <core/compute_layer/ComputeNode.h>
#include <core/compute_layer/Buffer.h>

using namespace core;
using namespace std;

template <typename ...T>
class OutputPorts {
protected:
    std::tuple<const std::unique_ptr<T>...> outputs;
};

template <typename ...T>
class InputPorts {
protected:
    std::tuple<std::unique_ptr<T>...> inputs;
};

class DummyData {
public:
    int a, b, c;
};

class DummySourceNode : public ComputeNode, 
    public OutputPorts<int, DummyData> {
public:
    
protected:
    //void execute() override;
};

class DummyNode : public ComputeNode, 
    public InputPorts<int, DummyData>,
    public OutputPorts<int, DummyData> {
public:
protected:
};

class DummySinkNode : public ComputeNode, 
    public InputPorts<int, DummyData> {
public:
protected:
};

SCENARIO("buffers working")
{
    GIVEN("a simple compute platform") {
        // ComputePlatform platform;
        
        DummySourceNode source;
        DummyNode node;
        DummySinkNode sink;

        std::vector<ComputeNode*> vec;
        vec.push_back(&source);
        vec.push_back(&sink);
        vec.push_back(&node);
        
        // platform.connect(source.output<PORT0, int>(), node.input<PORT0, int>());
        // platform.connect(source.output<PORT1, DummyData>(), node.input<PORT1, DummyData>());
        // platform.connect(node.output<PORT0, int>(), sink.input<PORT0, int>());
        // platform.connect(node.output<PORT1, DummyData>(), sink.input<PORT1, DummyData>());
    }
}