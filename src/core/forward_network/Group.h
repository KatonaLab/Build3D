#ifndef _core_forward_network_Group_h_
#define _core_forward_network_Group_h_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace core {

    class Node;

    class Group : public std::enable_shared_from_this<Group> {
    public:
        typedef std::shared_ptr<Group> Ptr;
    private:
        typedef std::shared_ptr<Node> NodePtr;
    public:
        NodePtr addNode(const std::string& name = "");
        Ptr addGroup(const std::string& name = "");
        static Ptr create(const std::string& name = "");
        bool empty() const;
        size_t size() const;
        void remove(const NodePtr& node);
        void remove(const Ptr& group);
        void clear();
        virtual bool valid() const;
        std::string name() const;
        virtual ~Group();
        const std::vector<NodePtr>& nodes() const;
        const std::vector<Ptr>& groups() const;
    protected:
        Group(const std::string& name = "");
    private:
        std::string m_name;
        std::vector<NodePtr> m_nodes;
        std::vector<Ptr> m_groups;
        std::weak_ptr<Group> m_parent;
    };
}

#endif
