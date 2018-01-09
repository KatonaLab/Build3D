#ifndef _core_Group_h_
#define _core_Group_h_

#include <cstddef>
#include <string>

#include "Node.h"

namespace core {
    
    class Group;
    typedef std::shared_ptr<Group> GroupPtr;

    class Group {
    public:
        Group(std::string name = "");
        bool empty() const;
        size_t size() const;
        NodePtr addNode(const std::string &name = "");
        GroupPtr addGroup(const std::string &name = "");
        void remove(const NodePtr &node);
        void remove(const GroupPtr &node);
        void clear();
        bool valid() const;
    };   
}

#endif
