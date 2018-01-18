#ifndef _core_directed_acyclic_graph_Graph_h_
#define _core_directed_acyclic_graph_Graph_h_

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "types.h"

namespace core {
namespace directed_acyclic_graph {

    // TODO: rethink the class for the following scenarios:
    // 1) multiple DependencyTraversal instance walking
    // 2) graph is modified during the walk
    class DependencyTraversal {
    public:
        DependencyTraversal(GraphPtr graph);
        bool hasNext();
        NodePtr next();
    protected:
        GraphPtr m_graph;
        std::list<NodePtr> m_readyList;
        std::set<NodePtr> m_waitingList;
    };

    class Graph : public std::enable_shared_from_this<Graph> {
        friend class DependencyTraversal;
    public:
        static GraphPtr create(const std::string& name = "");
        NodePtr add(const std::string& name = "");
        NodePtr add(NodePtr node);
        void remove(NodePtr node);
        void clear();
        DependencyTraversal traverse();
        bool empty() const;
        size_t size() const;
        std::string name() const;
        virtual ~Graph();
    protected:
        Graph(const std::string& name = "");
    private:
        std::string m_name;
        std::vector<NodePtr> m_nodes;
    };
}}

#endif
