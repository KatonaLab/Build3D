#ifndef _core_compute_platform_ports_h_
#define _core_compute_platform_ports_h_

#include <memory>

namespace core {
namespace compute_platform {

class InputPort;
class ComputeModule;

class OutputPort : public std::enable_shared_from_this<OutputPort> {
public:
    OutputPort(ComputeModule& parent);
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
};

class InputPort {
    friend class OutputPort;
public:
    InputPort(ComputeModule& parent);
    virtual void fetch() = 0;
    ComputeModule& parent();
    virtual size_t typeHash() const = 0;
    virtual ~InputPort();
protected:
    std::weak_ptr<OutputPort> m_source;
    ComputeModule& m_parent;
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
