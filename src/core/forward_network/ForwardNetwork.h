#ifndef _core_forward_network_ForwardNetwork_h_
#define _core_forward_network_ForwardNetwork_h_

#include "Group.h"
#include "Node.h"

#include <functional>
#include <memory>

namespace core {
    
    class ForwardNetwork : public Group {
    public:
        typedef std::shared_ptr<ForwardNetwork> Ptr;
    public:
        static Ptr create(const std::string& name = "");
        bool connect(Node::Ptr &from, Node::Ptr &to);
        void walk(std::function<void(const Node::Ptr &)>);
        bool valid() const override;
    protected:
        ForwardNetwork(const std::string& name = "");
    };
}

#endif
