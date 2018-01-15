#ifndef _core_directed_acyclic_graph_types_h_
#define _core_directed_acyclic_graph_types_h_

#include <memory>

namespace core {
namespace directed_acyclic_graph {

    class Graph;
    
    typedef std::shared_ptr<Graph> GraphPtr;
    typedef std::shared_ptr<const Graph> ConstGraphPtr;
    typedef std::weak_ptr<Graph> WeakGraphPtr;

    class Node;
    
    typedef std::shared_ptr<Node> NodePtr;
    typedef std::shared_ptr<const Node> ConstNodePtr;
    typedef std::weak_ptr<Node> WeakNodePtr;

    enum class TraversalMode {InputsOnly, OutputsOnly};
}}

#endif