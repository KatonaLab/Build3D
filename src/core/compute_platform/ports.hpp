#ifndef _core_compute_platform_ports_h_
#define _core_compute_platform_ports_h_

#include <string>
#include <typeinfo>

namespace core {
namespace compute_platform {

class OutputBufferPort {
public:
    virtual ~OutputBufferPort() {}
};

template <typename T>
class TypedOutputBufferPort : public OutputBufferPort {
public:
    std::shared_ptr<T> m_data;
};

class InputPort {
public:
    virtual bool set(OutputBufferPort& port) = 0;
    virtual void fetch() = 0;
    virtual std::string type() const = 0;
    virtual ~InputPort() {}
};

template <typename T>
class TypedInputPort : public InputPort {
public:
    virtual bool set(OutputBufferPort& port);
    virtual void fetch();
    virtual std::string type() const
    {
        return typeid(T).name();
    }
private:
    std::weak_ptr<T> m_ptr;
};

template <typename T, typename ...Ts>
class InputPortCollection {
public:
    const InputPort* input(size_t portId);
protected:
    InputPortCollection();
public:
    std::tuple<TypedInputPort<T>, TypedInputPort<Ts>...> m_inputPorts;
    std::vector<const InputPort*> m_typelessPorts;
};

namespace detail {

    template <int N, typename T, typename ...Ts>
    struct InputPortsHelper {
        static void fillVector(InputPortCollection<T, Ts...>& obj);
    };

}

#include "ports.ipp"

}}

#endif
