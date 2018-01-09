#ifndef _core_Node_h_
#define _core_Node_h_

#include <memory>
#include <string>

namespace core {
    
    class Node;
    typedef std::shared_ptr<Node> NodePtr;

    class Node {
    public:
        bool valid() const;
        const std::vector<NodePtr>& inputs() const;
        const std::vector<NodePtr>& outputs() const;
        const std::string& name() const;
    private:
        std::string m_name;
    };
}

#endif
