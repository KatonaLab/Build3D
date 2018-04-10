#ifndef _core_compute_platform_ports_h_
#define _core_compute_platform_ports_h_

#include <memory>
#include <string>

namespace core {
namespace compute_platform {

class InputPort;
class ComputeModule;

class OutputPort : public std::enable_shared_from_this<OutputPort> {
public:
    OutputPort(ComputeModule& parent);
    std::string name() const;
    void setName(const std::string& name);
    bool bind(std::weak_ptr<InputPort> inputPort);
    void unbind(std::weak_ptr<InputPort> inputPort);
    size_t numBinds() const;
    void reset();
    ComputeModule& parent();
    virtual size_t typeHash() const = 0;
    virtual ~OutputPort();
protected:
    virtual bool compatible(std::weak_ptr<InputPort> input) const = 0;
    virtual void cleanOnReset() = 0;
    size_t m_numBinds = 0;
    size_t m_numInputServed = 0;
    ComputeModule& m_parent;
    std::string m_name;
};

class InputPort {
    friend class OutputPort;
public:
    InputPort(ComputeModule& parent);
    std::string name() const;
    void setName(const std::string& name);
    virtual void fetch() = 0;
    ComputeModule& parent();
    virtual size_t typeHash() const = 0;
    bool connected() const;
    virtual ~InputPort();
protected:
    std::weak_ptr<OutputPort> m_source;
    ComputeModule& m_parent;
    std::string m_name;
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
