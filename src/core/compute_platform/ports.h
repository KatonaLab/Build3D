#ifndef _core_compute_platform_ports_h_
#define _core_compute_platform_ports_h_

#include <memory>
#include <set>
#include <string>
#include <vector>

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
    virtual std::string typeName() const = 0;
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
    std::string typeName() const override
    {
        return typeid(T).name();
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
#define PORT_TYPE_TRAITS_EX(Type, x...) template <> struct core::compute_platform::TraitSet<Type> { std::set<std::string> set = {x}; };

// TODO: remove the exact type traits, it's a hack that should be fixed with a project-wide static-dynamic typeing system
PORT_TYPE_TRAITS_EX(uint8_t, "int-like", "uint8_t");
PORT_TYPE_TRAITS_EX(uint16_t, "int-like", "uint16_t");
PORT_TYPE_TRAITS_EX(uint32_t, "int-like", "uint32_t");
PORT_TYPE_TRAITS_EX(uint64_t, "int-like", "uint64_t");
PORT_TYPE_TRAITS_EX(int8_t, "int-like", "int8_t");
PORT_TYPE_TRAITS_EX(int16_t, "int-like", "int16_t");
PORT_TYPE_TRAITS_EX(int32_t, "int-like", "int32_t");
PORT_TYPE_TRAITS_EX(int64_t, "int-like", "int64_t");
PORT_TYPE_TRAITS_EX(float, "float-like", "float");
PORT_TYPE_TRAITS_EX(double, "float-like", "double");
PORT_TYPE_TRAITS_EX(bool, "bool-like", "bool");

// --------------------------

class PortBase {
public:
    PortBase(ComputeModule& parent);
    std::string name() const;
    void setName(const std::string& name);
    
    std::string tags() const;
    bool hasTag(const std::string& tag) const;
    void setTags(const std::string& tags);

    ComputeModule& parent();
    virtual const PortTypeTraitsBase& traits() const = 0;
    virtual ~PortBase() = default;
protected:
    std::string m_name;
    std::string m_tags;
    ComputeModule& m_parent;
};

// TODO: rename InputPort and OutputPort to InputPortBase and OutputPortBase
// since they can not be ctrd publicly: protected ctr
class InputPort;

class OutputPort : public PortBase, public std::enable_shared_from_this<OutputPort> {
public:
    bool bind(std::weak_ptr<InputPort> inputPort);
    void unbind(std::weak_ptr<InputPort> inputPort);
    size_t numBinds() const;
    void reset();
    void purgeInvalidInputs();
    // NOTE: no need for connection break with the input port
    // since this shared_ptr is invalidated during (just before) dctr
    virtual ~OutputPort() = default;
protected:
    OutputPort(ComputeModule& parent);
    virtual bool compatible(std::weak_ptr<InputPort> input) const = 0;
    virtual void cleanOnReset() = 0;
    std::vector<std::weak_ptr<InputPort>> m_destinations;
    size_t m_numInputServed = 0;
};

class InputPort : public PortBase, public std::enable_shared_from_this<InputPort> {
    friend class OutputPort;
public:
    virtual void fetch() = 0;
    bool connected() const;
    // TODO: test this function
    std::weak_ptr<OutputPort> getSource() const;
    virtual ~InputPort();
protected:
    InputPort(ComputeModule& parent);
    std::weak_ptr<OutputPort> m_source;
};

class InputPortCollectionBase {
public:
    InputPortCollectionBase(ComputeModule& parent);
    virtual void fetch() = 0;
    virtual std::weak_ptr<InputPort> get(size_t portId) = 0;
    virtual size_t size() const = 0;
    virtual ~InputPortCollectionBase() = default;
protected:
    ComputeModule& m_parent;
};

class OutputPortCollectionBase {
public:
    OutputPortCollectionBase(ComputeModule& parent);
    virtual std::weak_ptr<OutputPort> get(size_t portId) = 0;
    virtual size_t size() const = 0;
    virtual ~OutputPortCollectionBase() = default;
protected:
    ComputeModule& m_parent;
};

class InputPortCollection : public InputPortCollectionBase {
public:
    // TODO: rename this class to EmptyInputPortCollection
    InputPortCollection(ComputeModule& parent);
    void fetch() override;
    std::weak_ptr<InputPort> get(size_t portId) override;
    size_t size() const override;
};

class OutputPortCollection : public OutputPortCollectionBase {
public:
    // TODO: rename this class to EmptyOutputPortCollection
    OutputPortCollection(ComputeModule& parent);
    std::weak_ptr<OutputPort> get(size_t portId) override;
    size_t size() const override;
};

}}

#endif
