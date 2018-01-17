#ifndef _core_compute_platform_ports_utils_h_
#define _core_compute_platform_ports_utils_h_

#include "ports.h"

#include <list>
#include <memory>

namespace core {
namespace compute_platform {

class ComputeModule;

template <typename T>
class TypedOutputPort : public OutputPort {
public:
    TypedOutputPort(ComputeModule& parent);
    std::weak_ptr<T> serve();
    T& value();
protected:
    virtual bool compatible(std::weak_ptr<InputPort> input) const;
    virtual void cleanOnReset();
private:
    std::shared_ptr<T> m_original;
    std::list<std::shared_ptr<T>> m_replicas;
};

template <typename T>
class TypedInputPort : public InputPort {
public:
    TypedInputPort(ComputeModule& parent);
    virtual void fetch();
    T& value();
private:
    std::weak_ptr<T> m_ptr;
};

template <typename T, typename ...Ts>
class TypedInputPortCollection : public InputPortCollection {
    using PortTuple = std::tuple<
        std::shared_ptr<TypedInputPort<T>>,
        std::shared_ptr<TypedInputPort<Ts>>...>;
    using TypeTuple = std::tuple<T, Ts...>;
public:
    TypedInputPortCollection(ComputeModule& parent);
    virtual void fetch();
    virtual std::weak_ptr<InputPort> get(size_t portId);
    virtual size_t size() const;
    template <std::size_t N> typename std::tuple_element<N, TypeTuple>::type& input();
private:
    PortTuple m_inputPorts;
    std::vector<std::weak_ptr<InputPort>> m_typelessPorts;
};

template <typename T, typename ...Ts>
class TypedOutputPortCollection : public OutputPortCollection {
    using PortTuple = std::tuple<
        std::shared_ptr<TypedOutputPort<T>>,
        std::shared_ptr<TypedOutputPort<Ts>>...>;
    using TypeTuple = std::tuple<T, Ts...>;
public:
    TypedOutputPortCollection(ComputeModule& parent);
    virtual std::weak_ptr<OutputPort> get(size_t portId);
    virtual size_t size() const;
    template <std::size_t N> typename std::tuple_element<N, TypeTuple>::type& output();
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
