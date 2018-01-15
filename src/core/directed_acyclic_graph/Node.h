#ifndef _core_directed_acyclic_graph_Node_h_
#define _core_directed_acyclic_graph_Node_h_

#include <memory>
#include <list>
#include <set>
#include <string>
#include <vector>

#include "types.h"

namespace core {
namespace directed_acyclic_graph {

    template <typename NodePtrType>
    class TraversalTmpl {
    public:
        TraversalTmpl(NodePtrType start, TraversalMode mode);
        bool hasNext();
        NodePtrType next();
    private:
        NodePtrType m_start;
        TraversalMode m_mode;
        std::list<NodePtrType> m_nodeList;
    };

    typedef TraversalTmpl<NodePtr> Traversal;
    typedef TraversalTmpl<ConstNodePtr> ConstTraversal;

    class DependencyTraversal;

    class Node : public std::enable_shared_from_this<Node> {
        friend class Graph;
        friend class TraversalTmpl<NodePtr>;
        friend class TraversalTmpl<ConstNodePtr>;
        friend class DependencyTraversal;
    public:
        static NodePtr create(const std::string& name = "");
        NodePtr input(size_t k);
        NodePtr output(size_t k);
        size_t numberOfInputs() const;
        size_t numberOfOutputs() const;
        bool hasOwner() const;
        bool nodeInOutputs(const ConstNodePtr& node) const;
        bool nodeInInputs(const ConstNodePtr& node) const;
        bool nodeInSubgraph(const ConstNodePtr& node, TraversalMode mode) const;
        Traversal traverse(TraversalMode mode);
        ConstTraversal constTraverse(TraversalMode mode) const;
        WeakGraphPtr& owner();
        const std::string& name() const;
        bool connect(NodePtr &to);
        void disconnect(NodePtr &with);
        virtual void notified();
        virtual ~Node();
    protected:
        Node(const std::string& name = "");
    private:
        std::vector<NodePtr>::iterator findOutputIterator(const ConstNodePtr& node);
        std::vector<WeakNodePtr>::iterator findInputIterator(const ConstNodePtr& node);
        std::vector<NodePtr>::const_iterator findOutputConstIterator(const ConstNodePtr& node) const;
        std::vector<WeakNodePtr>::const_iterator findInputConstIterator(const ConstNodePtr& node) const;

        std::string m_name;
        WeakGraphPtr m_owner;
        std::vector<WeakNodePtr> m_inputs;
        std::vector<NodePtr> m_outputs;
        bool m_ready;
    };
    
    template <typename NodePtrType>
    TraversalTmpl<NodePtrType>::TraversalTmpl(NodePtrType start, TraversalMode mode)
        : m_start(start), m_mode(mode), m_nodeList({m_start})
    {}

    template <typename NodePtrType>
    bool TraversalTmpl<NodePtrType>::hasNext()
    {
        return !m_nodeList.empty();
    }

    template <typename NodePtrType>
    NodePtrType TraversalTmpl<NodePtrType>::next()
    {
        if (m_nodeList.empty()) {
            return NodePtrType();
        }

        auto current = m_nodeList.front();
        m_nodeList.pop_front();

        if (m_mode == TraversalMode::OutputsOnly) {
            copy(current->m_outputs.begin(), current->m_outputs.end(), back_inserter(m_nodeList));
        }

        if (m_mode == TraversalMode::InputsOnly) {
            for (auto& w : current->m_inputs) {
                m_nodeList.push_back(w.lock());
            }
        }

        return current;
    }
    
}}

#endif
