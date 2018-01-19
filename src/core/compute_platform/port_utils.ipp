
template <typename T>
inline
TypedOutputPort<T>::TypedOutputPort(ComputeModule& parent)
    : OutputPort(parent)
{}

template <typename T>
inline
bool TypedOutputPort<T>::compatible(std::weak_ptr<InputPort> input) const
{
    if (auto ptr = input.lock()) {
        return ptr->typeHash() == typeHash();
    }
    return false;
}

template <typename T>
inline
std::weak_ptr<T> TypedOutputPort<T>::serve()
{
    m_numInputServed++;

    if (m_numInputServed < m_numBinds) {
        // T's copy constructor should do a deep copy
        m_replicas.push_back(std::make_shared<T>(*m_original));
        return std::weak_ptr<T>(m_replicas.back());
    } else if (m_numInputServed == m_numBinds) {
        return std::weak_ptr<T>(m_original);
    }
    // m_numInputServed > m_numBinds
    throw std::runtime_error("more serve requests arrived than the number of the bindings");
}

template <typename T>
inline
size_t TypedOutputPort<T>::typeHash() const
{
    return typeid(T).hash_code();
}

template <typename T>
inline
T& TypedOutputPort<T>::value()
{
    if (m_original) {
        return *m_original;
    } else {
        throw std::runtime_error("output port has no data");
    }
}

template <typename T>
inline
void TypedOutputPort<T>::forwardFromInput(std::weak_ptr<TypedInputPort<T>> input)
{
    if (auto inputPtr = input.lock()) {
        if (auto dataPtr = inputPtr->m_ptr.lock()) {
            m_original = dataPtr;
        } else {
            throw std::runtime_error("can not forward empty input data");
        }
    } else {
        throw std::runtime_error("can not forward an invalid input");
    }
}

template <typename T>
inline
void TypedOutputPort<T>::forwardFromSharedPtr(std::shared_ptr<T> data)
{
    m_original = data;
}

template <typename T>
inline
void TypedOutputPort<T>::cleanOnReset()
{
    m_replicas.clear();
}

template <typename T>
inline
TypedInputPort<T>::TypedInputPort(ComputeModule& parent) : InputPort(parent)
{}

template <typename T>
inline
void TypedInputPort<T>::fetch()
{
    if (auto sourcePtr = m_source.lock()) {
        auto source = dynamic_cast<TypedOutputPort<T>*>(sourcePtr.get());
        if (source == nullptr) {
            throw std::runtime_error("input type differs from the output type");
        }
        m_ptr = source->serve();
    } else {
        throw std::runtime_error("input not connected to an output");
    }
}

template <typename T>
inline
size_t TypedInputPort<T>::typeHash() const
{
    return typeid(T).hash_code();
}

template <typename T>
inline
T& TypedInputPort<T>::value()
{
    if (auto ptr = m_ptr.lock()) {
        return *ptr;
    }
    throw std::runtime_error("tried to get the value of an unconnected input");
}

template <typename T>
inline
std::weak_ptr<T> TypedInputPort<T>::inputPtr()
{
    return m_ptr;
}

template <typename T, typename ...Ts>
inline 
TypedInputPortCollection<T, Ts...>::TypedInputPortCollection(ComputeModule& parent)
    : InputPortCollection(parent),
    m_inputPorts(std::make_shared<TypedInputPort<T>>(parent),
    std::make_shared<TypedInputPort<Ts>>(parent)...)
{
    detail::TupleToVectorHelper<sizeof...(Ts), decltype(m_inputPorts), decltype(m_typelessPorts)>
        ::tupleToVector(m_inputPorts, m_typelessPorts);
}

template <typename T, typename ...Ts>
inline
void TypedInputPortCollection<T, Ts...>::fetch()
{
    for (auto p : m_typelessPorts) {
        if (auto ptr = p.lock()) {
            ptr->fetch();
        } else {
            throw std::runtime_error("tried to fetch with nullptr port");
        }
    }
}

template <typename T, typename ...Ts>
inline
std::weak_ptr<InputPort> TypedInputPortCollection<T, Ts...>::get(size_t portId)
{
    return m_typelessPorts[portId];
}

template <typename T, typename ...Ts>
inline
size_t TypedInputPortCollection<T, Ts...>::size() const
{
    return m_typelessPorts.size();
}

template <typename T, typename ...Ts>
template <std::size_t N>
typename std::tuple_element<N, typename TypedInputPortCollection<T, Ts...>::PortTuple>::type
TypedInputPortCollection<T, Ts...>::input()
{
    return std::get<N>(m_inputPorts);
}

template <typename T, typename ...Ts>
inline TypedOutputPortCollection<T, Ts...>::TypedOutputPortCollection(ComputeModule& parent)
    : OutputPortCollection(parent),
    m_outputPorts(std::make_shared<TypedOutputPort<T>>(parent),
        std::make_shared<TypedOutputPort<Ts>>(parent)...)
{
    detail::TupleToVectorHelper<sizeof...(Ts), decltype(m_outputPorts), decltype(m_typelessPorts)>
        ::tupleToVector(m_outputPorts, m_typelessPorts);
}

template <typename T, typename ...Ts>
inline
std::weak_ptr<OutputPort> TypedOutputPortCollection<T, Ts...>::get(size_t portId)
{
    return m_typelessPorts[portId];
}

template <typename T, typename ...Ts>
inline
size_t TypedOutputPortCollection<T, Ts...>::size() const
{
    return m_typelessPorts.size();
}

template <typename T, typename ...Ts>
template <std::size_t N>
typename std::tuple_element<N, typename TypedOutputPortCollection<T, Ts...>::PortTuple>::type
TypedOutputPortCollection<T, Ts...>::output()
{
    return std::get<N>(m_outputPorts);
}

namespace detail {

    template <int N, typename TupleType, typename VectorType>
    void TupleToVectorHelper<N, TupleType, VectorType>::tupleToVector(
        TupleType& tpl, VectorType& vec)
    {
        TupleToVectorHelper<N - 1, TupleType, VectorType>::tupleToVector(tpl, vec);
        vec.push_back(std::get<N>(tpl));
    }

    template <typename TupleType, typename VectorType>
    struct TupleToVectorHelper<0, TupleType, VectorType> {
        static void tupleToVector(TupleType& tpl, VectorType& vec)
        {
            vec.push_back(std::get<0>(tpl));
        }
    };
}
