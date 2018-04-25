#ifndef _core_compute_platform_ports_h_
#define _core_compute_platform_ports_h_

#include <memory>
#include <set>
#include <string>

namespace core {
namespace compute_platform {

class InputPort;
class ComputeModule;

class PortTypeTraitsBase {
public:
    // TODO: naming can be confusing -> we only want type equality not
    // all traits equality, rename it like typeEquals or so
    bool equals(const PortTypeTraitsBase& ptt) const
    {
        return hash() == ptt.hash();
    }
    virtual bool hasTrait(const std::string& trait) const = 0;
protected:
    virtual std::size_t hash() const = 0;
};

template <typename T>
struct TraitSet {
    std::set<std::string> set = {};
};

template <typename T>
class PortTypeTraits : public PortTypeTraitsBase {
public:
    bool hasTrait(const std::string& trait) const override
    {
        return m_traits.set.find(trait) != m_traits.set.end();
    }
    static const PortTypeTraits& instance()
    {
        static const PortTypeTraits theOne;
        return theOne;
    }
protected:
    std::size_t hash() const override
    {
        return typeid(T).hash_code();
    }
    static const TraitSet<T> m_traits;
};

template <typename T> const TraitSet<T> PortTypeTraits<T>::m_traits;

#define PORT_TYPE_TRAITS(Type, x) template <> struct core::compute_platform::TraitSet<Type> { std::set<std::string> set = x; };

PORT_TYPE_TRAITS(uint8_t, {"int-like"});
PORT_TYPE_TRAITS(uint16_t, {"int-like"});
PORT_TYPE_TRAITS(uint32_t, {"int-like"});
PORT_TYPE_TRAITS(uint64_t, {"int-like"});
PORT_TYPE_TRAITS(int8_t, {"int-like"});
PORT_TYPE_TRAITS(int16_t, {"int-like"});
PORT_TYPE_TRAITS(int32_t, {"int-like"});
PORT_TYPE_TRAITS(int64_t, {"int-like"});
PORT_TYPE_TRAITS(float, {"float-like"});
PORT_TYPE_TRAITS(double, {"float-like"});
PORT_TYPE_TRAITS(bool, {"bool-like"});

// --------------------------

class PortBase {
public:
    PortBase(ComputeModule& parent);
    std::string name() const;
    void setName(const std::string& name);
    ComputeModule& parent();
    virtual const PortTypeTraitsBase& traits() const = 0;
    virtual ~PortBase() = default;
protected:
    std::string m_name;
    ComputeModule& m_parent;
};

class OutputPort : public PortBase, public std::enable_shared_from_this<OutputPort> {
public:
    OutputPort(ComputeModule& parent);
    bool bind(std::weak_ptr<InputPort> inputPort);
    void unbind(std::weak_ptr<InputPort> inputPort);
    size_t numBinds() const;
    void reset();
    virtual ~OutputPort();
protected:
    virtual bool compatible(std::weak_ptr<InputPort> input) const = 0;
    virtual void cleanOnReset() = 0;
    size_t m_numBinds = 0;
    size_t m_numInputServed = 0;
};

class InputPort : public PortBase {
    friend class OutputPort;
public:
    InputPort(ComputeModule& parent);
    virtual void fetch() = 0;
    bool connected() const;
    std::weak_ptr<OutputPort> getSource() const;
    virtual ~InputPort();
protected:
    std::weak_ptr<OutputPort> m_source;
};

class InputPortCollection {
public:
    InputPortCollection(ComputeModule& parent);
    virtual void fetch();
    virtual std::weak_ptr<InputPort> get(size_t portId);
    virtual size_t size() const;
    virtual ~InputPortCollection();
protected:
    ComputeModule& m_parent;
};

class OutputPortCollection {
public:
    OutputPortCollection(ComputeModule& parent);
    virtual std::weak_ptr<OutputPort> get(size_t portId);
    virtual size_t size() const;
    virtual ~OutputPortCollection();
protected:
    ComputeModule& m_parent;
};

}}

#endif
