template <typename T>
inline bool TypedInputPort<T>::set(OutputBufferPort& port)
{
    if (dynamic_cast<TypedOutputBufferPort<T>*>(&port) != nullptr) {
        m_ptr = ((TypedOutputBufferPort<T>*)(&port))->m_data;
        return true;
    }
    return false;
}

template <typename T>
inline void TypedInputPort<T>::fetch()
{
    // TODO
}

template <typename T, typename ...Ts>
inline InputPortCollection<T, Ts...>::InputPortCollection()
{
    detail::InputPortsHelper<sizeof...(Ts), T, Ts...>::fillVector(*this);
}

template <typename T, typename ...Ts>
inline const InputPort* InputPortCollection<T, Ts...>::input(size_t portId)
{
    return m_typelessPorts[portId];
}

namespace detail {

    template <int N, typename T, typename ...Ts>
    void InputPortsHelper<N, T, Ts...>::fillVector(InputPortCollection<T, Ts...>& obj)
    {
        InputPortsHelper<N - 1, T, Ts...>::fillVector(obj);
        obj.m_typelessPorts.push_back(&std::get<N>(obj.m_inputPorts));
    }

    template <typename T, typename ...Ts>
    struct InputPortsHelper<0, T, Ts...> {
        static void fillVector(InputPortCollection<T, Ts...>& obj)
        {
            obj.m_typelessPorts.push_back(&std::get<0>(obj.m_inputPorts));
        }
    };
}

// template <typename T, typename ...Ts>
// template <int N>
// inline InputPorts<T, Ts...>::VectorFiller<N>::VectorFiller(InputPorts& parent)
//     : m_parent(parent)
// {}

// template <typename T, typename ...Ts>
// template <int N>
// inline void InputPorts<T, Ts...>::VectorFiller<N>::put()
// {
//     VectorFiller<N - 1>(m_parent).put();
//     m_parent.m_typelessPorts.push_back(&std::get<N>(m_parent.m_inputPorts));
// }

// template <typename T, typename ...Ts>
// template <>
// inline void InputPorts<T, Ts...>::VectorFiller<0>::put()
// {
//     // m_parent.m_typelessPorts.push_back(&std::get<0>(m_parent.m_inputPorts));
// }
