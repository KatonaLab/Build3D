#ifndef _core_ForwardNetwork_h_
#define _core_ForwardNetwork_h_

#include "Group.h"

#include <functional>

namespace core {
    
    class ForwardNetwork : public Group {
    public:
        ForwardNetwork(std::string name = "");
        bool connect(NodePtr &from, NodePtr &to);
        void walk(std::function<void(const NodePtr &)>);
    };
}

#endif
