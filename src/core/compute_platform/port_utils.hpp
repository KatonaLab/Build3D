#ifndef _core_compute_platform_ports_utils_h_
#define _core_compute_platform_ports_utils_h_

#include "ports.h"

#include <list>
#include <memory>
#include <typeinfo>

namespace core {
namespace compute_platform {

class ComputeModule;

template <typename T>
class TypedInputPort;

template <typename T>
class TypedOutputPort : public OutputPort {
public:
    static std::shared_ptr<TypedOutputPort> create(ComputeModule& parent)
    {
        return std::shared_ptr<TypedOutputPort>(new TypedOutputPort(parent));
    }
    std::weak_ptr<T> serve();
    const PortTypeTraitsBase& traits() const override;
    T& value();
    std::shared_ptr<T> sharedValue();
    void forwardFromInput(std::weak_ptr<TypedInputPort<T>> input);
    void forwardFromSharedPtr(std::shared_ptr<T> data);
protected:
    TypedOutputPort(ComputeModule& parent);
    virtual bool compatible(std::weak_ptr<InputPort> input) const override;
    virtual void cleanOnReset() override;
private:
    std::shared_ptr<T> m_original;
    std::list<std::shared_ptr<T>> m_replicas;
};

template <typename T>
class TypedInputPort : public InputPort {
    friend class TypedOutputPort<T>;
public:
    static std::shared_ptr<TypedInputPort> create(ComputeModule& parent)
    {
        return std::shared_ptr<TypedInputPort>(new TypedInputPort(parent));
    }
    virtual void fetch() override;
    const PortTypeTraitsBase& traits() const override;
    T& value();
    std::shared_ptr<T> sharedValue();
    std::weak_ptr<T> inputPtr();
protected:
    TypedInputPort(ComputeModule& parent);
private:
    std::weak_ptr<T> m_ptr;
};

template <typename T, typename ...Ts>
class TypedInputPortCollection final : public InputPortCollectionBase {
    using PortTuple = std::tuple<
        std::shared_ptr<TypedInputPort<T>>,
        std::shared_ptr<TypedInputPort<Ts>>...>;
    using TypeTuple = std::tuple<T, Ts...>;
public:
    TypedInputPortCollection(ComputeModule& parent);
    void fetch() override;
    std::weak_ptr<InputPort> get(size_t portId) override;
    size_t size() const override;
    template <std::size_t N> typename std::tuple_element<N, PortTuple>::type input();
private:
    PortTuple m_inputPorts;
    std::vector<std::weak_ptr<InputPort>> m_typelessPorts;
};

template <typename T, typename ...Ts>
class TypedOutputPortCollection final : public OutputPortCollectionBase {
    using PortTuple = std::tuple<
        std::shared_ptr<TypedOutputPort<T>>,
        std::shared_ptr<TypedOutputPort<Ts>>...>;
    using TypeTuple = std::tuple<T, Ts...>;
public:
    TypedOutputPortCollection(ComputeModule& parent);
    std::weak_ptr<OutputPort> get(size_t portId) override;
    size_t size() const override;
    template <std::size_t N> typename std::tuple_element<N, PortTuple>::type output();
private:
    PortTuple m_outputPorts;
    std::vector<std::weak_ptr<OutputPort>> m_typelessPorts;
};

namespace detail {

    template <int N, typename TupleType, typename VectorType>
    struct TupleToVectorHelper {
        static void tupleToVector(TupleType& tpl, VectorType& vec);
        static void tupleSharedInit(TupleType& tpl);
    };
}

#include "port_utils.ipp"

}}

#endif
