#ifndef _core_directed_acyclic_graph_Graph_h_
#define _core_directed_acyclic_graph_Graph_h_

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "types.h"

namespace core {
namespace directed_acyclic_graph {

    class DependencyTraverse {
        friend class Graph;
    public:
        DependencyTraverse(GraphPtr graph) {}
        bool hasNext() {}
        NodePtr next() {}
    };

    class Graph : public std::enable_shared_from_this<Graph> {
    public:
        static GraphPtr create(const std::string& name = "");
        NodePtr add(const std::string& name = "");
        NodePtr add(NodePtr& node);
        void remove(NodePtr& node);
        void clear();
        DependencyTraverse traverse();
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
