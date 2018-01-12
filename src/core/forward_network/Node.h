#ifndef _core_forward_network_Node_h_
#define _core_forward_network_Node_h_

#include <memory>
#include <string>
#include <vector>

namespace core {

    class Group;
    class ForwardNetwork;

    class Node {
        friend class Group;
        friend class ForwardNetwork;
    public:
        typedef std::shared_ptr<Node> Ptr;
    public:
        static Ptr create(const std::string& name = "");
        const std::vector<Ptr>& inputs() const;
        const std::vector<Ptr>& outputs() const;
        const std::string& name() const;
        bool valid() const;
    protected:
        Node(const std::string& name = "");
        void addIntput(Ptr& node);
        void addOutput(Ptr& node);
    private:
        std::string m_name;
        std::weak_ptr<Group> m_parent;
        std::vector<Ptr> m_inputs;
        std::vector<Ptr> m_outputs;
    };
}

#endif
