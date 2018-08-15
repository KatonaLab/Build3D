#ifndef _core_compute_platform_ports_h_
#define _core_compute_platform_ports_h_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <iostream>

namespace core {
namespace compute_platform {

class InputPort;
class ComputeModule;

// TODO: write tests
// TODO: move to a utility file/folder
class PropertyMap {
public:
    enum class Type {Int, Bool, String, Float};
    void setInt(const std::string& key, int value)
    {
        m_map[key].m_intData = value;
        m_map[key].m_type = Type::Int;
    }
    void setBool(const std::string& key, bool value)
    {
        m_map[key].m_boolData = value;
        m_map[key].m_type = Type::Bool;
    }
    void setString(const std::string& key, const std::string& value)
    {
        m_map[key].m_stringData = value;
        m_map[key].m_type = Type::String;
    }
    void setFloat(const std::string& key, float value)
    {
        m_map[key].m_floatData = value;
        m_map[key].m_type = Type::Float;
    }
    bool hasKey(const std::string& key) const
    {
        return m_map.count(key);
    }
    int asInt(const std::string& key) const
    {
        // TODO: handle no such key error
        return m_map.at(key).m_intData;
    }
    bool asBool(const std::string& key) const
    {
        // TODO: handle no such key error
        return m_map.at(key).m_boolData;
    }
    std::string asString(const std::string& key) const
    {
        // TODO: handle no such key error
        return m_map.at(key).m_stringData;
    }
    float asFloat(const std::string& key) const
    {
        // TODO: handle no such key error
        return m_map.at(key).m_floatData;
    }
    Type getType(const std::string& key) const
    {
        // TODO: handle no such key error
        return m_map.at(key).m_type;
    }
    void remove(const std::string& key)
    {
        auto it = m_map.find(key);
        if (it != m_map.end()) {
            m_map.erase(it);
        }
    }
    std::vector<std::string> keys() const
    {
        std::vector<std::string> ks;
        for (const auto& kv: m_map) {
            ks.push_back(kv.first);
        }
        return ks;
    }
private:
    struct Data {
        // TODO: this is a wasteful solution, should be using c++17 std::any or std::variant
        Type m_type;
        int m_intData;
        bool m_boolData;
        std::string m_stringData;
        float m_floatData;
    };
    std::map<std::string, Data> m_map;
};

class PortTypeTraitsBase {
public:
    // TODO: naming can be confusing -> we only want type equality not
    // all traits equality, rename it like typeEquals or so
    bool equals(const PortTypeTraitsBase& ptt) const
    {
        return hash() == ptt.hash();
    }
    virtual bool hasTrait(const std::string& trait) const = 0;
    virtual std::set<std::string> getAll() const = 0;
    virtual std::string typeName() const = 0;
protected:
    virtual std::size_t hash() const = 0;
};

template <typename T>
struct TraitSet {
    // NOTE: if you ended up here with a compile time error, that
    // means you are trying to access a TraitSet of a type that
    // was not registered with the PORT_TYPE_TRAITS macro.
    // Also check your includes, chances are you forgot
    // the include where you call PORT_TYPE_TRAITS on your
    // types
    // TraitSet() = delete;
    // static const std::set<std::string>& set() {
    //     static std::set<std::string> empty;
    //     return empty;
    // }
    // TraitSet() = delete;
    std::set<std::string> set() const = delete;
};

template <typename T>
class PortTypeTraits : public PortTypeTraitsBase {
public:
    bool hasTrait(const std::string& trait) const override
    {
        return m_traits.set().find(trait) != m_traits.set().end();
    }
    std::set<std::string> getAll() const override
    {
        return m_traits.set();
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

#define PORT_TYPE_TRAITS(Type, ...) template <> struct core::compute_platform::TraitSet<Type> { \
    std::set<std::string> set() const { return {__VA_ARGS__}; } };

// TODO: remove the exact type traits, it's a hack that should be fixed with a project-wide static-dynamic typeing system
PORT_TYPE_TRAITS(uint8_t, "int-like", "uint8_t");
PORT_TYPE_TRAITS(uint16_t, "int-like", "uint16_t");
PORT_TYPE_TRAITS(uint32_t, "int-like", "uint32_t");
PORT_TYPE_TRAITS(uint64_t, "int-like", "uint64_t");
PORT_TYPE_TRAITS(int8_t, "int-like", "int8_t");
PORT_TYPE_TRAITS(int16_t, "int-like", "int16_t");
PORT_TYPE_TRAITS(int32_t, "int-like", "int32_t");
PORT_TYPE_TRAITS(int64_t, "int-like", "int64_t");
PORT_TYPE_TRAITS(float, "float-like", "float");
PORT_TYPE_TRAITS(double, "float-like", "double");
PORT_TYPE_TRAITS(bool, "bool-like", "bool");
PORT_TYPE_TRAITS(std::string, "string");

// --------------------------

class PortBase {
public:
    PortBase(ComputeModule& parent);
    std::string name() const;
    void setName(const std::string& name);
    const PropertyMap& properties() const;
    PropertyMap& properties();
    ComputeModule& parent();
    virtual const PortTypeTraitsBase& traits() const = 0;
    virtual ~PortBase() = default;
protected:
    std::string m_name;
    PropertyMap m_propertyMap;
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
