#include "catch.hpp"

#include <algorithm>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include <core/directed_acyclic_graph/Graph.h>
#include <core/directed_acyclic_graph/Node.h>

using namespace core::directed_acyclic_graph;
using namespace std;

SCENARIO("two nodes can be connected", "[core/directed_acyclic_graph]")
{
    GIVEN("two nodes, a, b") {
        NodePtr a = Node::create("a");
        NodePtr b = Node::create("b");
        REQUIRE(a->name() == "a");
        REQUIRE(b->name() == "b");
        REQUIRE(a->numberOfInputs() == 0);
        REQUIRE(a->numberOfOutputs() == 0);
        REQUIRE(b->numberOfInputs() == 0);
        REQUIRE(b->numberOfOutputs() == 0);
        WHEN("connected, a -> b") {
            REQUIRE(a->connect(b) == true);
            THEN("the connection is made") {
                REQUIRE(a->numberOfOutputs() == 1);
                REQUIRE(a->output(0) == b);
                REQUIRE(b->numberOfInputs() == 1);
                REQUIRE(b->input(0) == a);
            }

            AND_WHEN("trying to connect again, a -> b") {
                REQUIRE(a->connect(b) == true);
                THEN("nothing happens") {
                    REQUIRE(a->numberOfOutputs() == 1);
                    REQUIRE(a->output(0) == b);
                    REQUIRE(b->numberOfInputs() == 1);
                    REQUIRE(b->input(0) == a);
                }
            }

            AND_WHEN("b requests disconnection") {
                b->disconnect(a);
                THEN("the connection is removed") {
                    REQUIRE(a->numberOfOutputs() == 0);
                    REQUIRE(b->numberOfInputs() == 0);
                }
            }

            AND_WHEN("a requests disconnection") {
                a->disconnect(b);
                THEN("nothing happens") {
                    REQUIRE(a->numberOfOutputs() == 0);
                    REQUIRE(b->numberOfInputs() == 0);
                }
            }
        }
    }
}

SCENARIO("multiple output connections can be made", "[core/directed_acyclic_graph]")
{
    GIVEN("three nodes: a, b, c") {
        NodePtr a = Node::create();
        NodePtr b = Node::create();
        NodePtr c = Node::create();

        WHEN("a -> b, a -> c") {
            REQUIRE(a->connect(b) == true);
            REQUIRE(a->connect(c) == true);
            THEN("the connections are made") {
                REQUIRE(a->numberOfOutputs() == 2);
                REQUIRE(a->output(0) == b);
                REQUIRE(a->output(1) == c);
                REQUIRE(b->numberOfInputs() == 1);
                REQUIRE(b->input(0) == a);
                REQUIRE(c->numberOfInputs() == 1);
                REQUIRE(c->input(0) == a);
            }

            AND_WHEN("a -> b is removed") {
                a->disconnect(b);
                THEN("only a -> c remains") {
                    REQUIRE(a->numberOfOutputs() == 1);
                    REQUIRE(a->output(0) == c);
                    REQUIRE(b->numberOfInputs() == 0);
                    REQUIRE(c->numberOfInputs() == 1);
                    REQUIRE(c->input(0) == a);
                }
            }
        }
    }
}

SCENARIO("multiple input connections can be made", "[core/directed_acyclic_graph]")
{
    GIVEN("three nodes: a, b, c") {
        NodePtr a = Node::create();
        NodePtr b = Node::create();
        NodePtr c = Node::create();

        WHEN("a -> c, b -> c") {
            REQUIRE(a->connect(c) == true);
            REQUIRE(b->connect(c) == true);
            THEN("the connections are made") {
                REQUIRE(a->numberOfOutputs() == 1);
                REQUIRE(a->output(0) == c);
                REQUIRE(b->numberOfOutputs() == 1);
                REQUIRE(b->output(0) == c);
                REQUIRE(c->numberOfInputs() == 2);
                REQUIRE(c->input(0) == a);
                REQUIRE(c->input(1) == b);
            }

            AND_WHEN("a -> c is removed") {
                a->disconnect(c);
                THEN("only b -> c remains") {
                    REQUIRE(a->numberOfOutputs() == 0);
                    REQUIRE(b->numberOfOutputs() == 1);
                    REQUIRE(b->output(0) == c);
                    REQUIRE(c->numberOfInputs() == 1);
                    REQUIRE(c->input(0) == b);
                }
            }
        }
    }
}

SCENARIO("nodes can be connected in chain", "[core/directed_acyclic_graph]")
{
    GIVEN("three nodes: a, b, c") {
        NodePtr a = Node::create();
        NodePtr b = Node::create();
        NodePtr c = Node::create();
        WHEN("a -> b, b -> c") {
            REQUIRE(a->connect(b) == true);
            REQUIRE(b->connect(c) == true);
            THEN("the connections are ok") {
                REQUIRE(a->numberOfOutputs() == 1);
                REQUIRE(b->numberOfInputs() == 1);
                REQUIRE(b->numberOfOutputs() == 1);
                REQUIRE(c->numberOfInputs() == 1);
                REQUIRE(a->output(0) == b);
                REQUIRE(b->input(0) == a);
                REQUIRE(b->output(0) == c);
                REQUIRE(c->input(0) == b);
            }
        }
    }
}

SCENARIO("trivial cycle can not be made", "[core/directed_acyclic_graph]")
{
    GIVEN("two connected nodes, a -> b") {
        NodePtr a = Node::create();
        NodePtr b = Node::create();
        REQUIRE(a->connect(b) == true);
        REQUIRE(a->numberOfOutputs() == 1);
        REQUIRE(a->output(0) == b);
        REQUIRE(b->numberOfInputs() == 1);
        REQUIRE(b->input(0) == a);
        WHEN("trying to connect b -> a") {
            REQUIRE(b->connect(a) == false);
            THEN("everything remains the same") {
                REQUIRE(a->numberOfOutputs() == 1);
                REQUIRE(a->output(0) == b);
                REQUIRE(b->numberOfInputs() == 1);
                REQUIRE(b->input(0) == a);
            }
        }

        WHEN("trying to connect a -> a") {
            REQUIRE(a->connect(a) == false);
            THEN("everything remains the same") {
                REQUIRE(a->numberOfOutputs() == 1);
                REQUIRE(a->output(0) == b);
                REQUIRE(b->numberOfInputs() == 1);
                REQUIRE(b->input(0) == a);
            }
        }
    }
}

SCENARIO("non-trivial cycle can not be made", "[core/directed_acyclic_graph]")
{
    GIVEN("a chain connection") {
        NodePtr first = Node::create("first");
        NodePtr last = first;
        for (int i = 0; i < 4; ++i) {
            NodePtr node = Node::create("node" + to_string(i));
            REQUIRE(last->connect(node) == true);
            last = node;
        }

        WHEN("trying to make a cycle") {
            REQUIRE(last->connect(first) == false);
            THEN("everything remains the same") {
                REQUIRE(last->numberOfOutputs() == 0);
                REQUIRE(first->numberOfInputs() == 0);
            }
        }
    }
}

SCENARIO("nodes are managed by graph object", "[core/directed_acyclic_graph]")
{
    GIVEN("a graph and separated connected nodes, g, a -> b") {
        GraphPtr graph = Graph::create("graph");
        REQUIRE(graph->name() == "graph");
        NodePtr a = Node::create("a");
        NodePtr b = Node::create("b");
        REQUIRE(a->hasOwner() == false);
        REQUIRE(b->hasOwner() == false);
        REQUIRE(graph->empty() == true);
        REQUIRE(graph->size() == 0);
        a->connect(b);
        
        WHEN("adding node a to g") {
            REQUIRE(graph->add(a) == a);
            THEN("node a added") {
                REQUIRE(a->hasOwner());
                REQUIRE(a->owner().lock() == graph);
            }
            AND_THEN("node b is also added") {
                REQUIRE(b->hasOwner());
                REQUIRE(b->owner().lock() == graph);
            }
            AND_THEN("the graph size is increased") {
                REQUIRE(graph->empty() == false);
                REQUIRE(graph->size() == 2);
            }
        }

        WHEN("adding node b to g") {
            REQUIRE(graph->add(b) == b);
            THEN("node b added") {
                REQUIRE(b->hasOwner());
                REQUIRE(b->owner().lock() == graph);
            }
            AND_THEN("node a is also added") {
                REQUIRE(b->hasOwner());
                REQUIRE(b->owner().lock() == graph);
            }
            AND_THEN("the graph size is increased") {
                REQUIRE(graph->empty() == false);
                REQUIRE(graph->size() == 2);
            }
        }
    }
}

SCENARIO("connecting independent nodes to a graph modifis their owners", "[core/directed_acyclic_graph]")
{
    GIVEN("a graph g with node a and an independent node b") {
        GraphPtr graph = Graph::create("graph");
        NodePtr a = graph->add("graph node");
        NodePtr b = Node::create();
        REQUIRE(a->hasOwner() == true);
        REQUIRE(a->owner().lock() == graph);
        REQUIRE(b->hasOwner() == false);
        REQUIRE(graph->empty() == false);
        REQUIRE(graph->size() == 1);
        WHEN("a -> b") {
            REQUIRE(a->connect(b) == true);
            THEN("b changes owner") {
                REQUIRE(b->hasOwner() == true);
                REQUIRE(b->owner().lock() == graph);
                REQUIRE(graph->empty() == false);
                REQUIRE(graph->size() == 2);
            }
        }
    }

    GIVEN("graph g1 with node n1 and graph g2 with node n2") {
        GraphPtr g1 = Graph::create();
        NodePtr n1 = g1->add();
        GraphPtr g2 = Graph::create();
        NodePtr n2 = g2->add();
        REQUIRE(n1->hasOwner() == true);
        REQUIRE(n2->hasOwner() == true);
        WHEN("n1 -> n2") {
            REQUIRE(n1->connect(n2) == true);
            THEN("n2 gets a new owner (g1)") {
                REQUIRE(n2->hasOwner() == true);
                REQUIRE(n2->owner().lock() == g1);
            }
            AND_THEN("g1 size increase") {
                REQUIRE(g1->empty() == false);
                REQUIRE(g1->size() == 2);
            }
            AND_THEN("g2 size decrease") {
                REQUIRE(g2->empty() == true);
                REQUIRE(g2->size() == 0);
            }
        }
    }
}

SCENARIO("nodes are removed with their connections", "[core/directed_acyclic_graph]")
{
    GIVEN("a graph, g, a -> b") {
        GraphPtr g = Graph::create("graph");
        NodePtr a = g->add();
        NodePtr b = Node::create();
        a->connect(b);
        REQUIRE(g->empty() == false);
        REQUIRE(g->size() == 2);
        WHEN("a is removed") {
            g->remove(a);
            THEN("the graph size is decreased") {
                REQUIRE(g->empty() == false);
                REQUIRE(g->size() == 1);
            }
            AND_THEN("a has no owner") {
                REQUIRE(a->hasOwner() == false);
                REQUIRE(a->owner().lock() == nullptr);
            }
            AND_THEN("a -> b connection is also removed") {
                REQUIRE(a->numberOfOutputs() == 0);
                REQUIRE(b->numberOfInputs() == 0);
            }

            AND_WHEN("trying to remove the same node again") {
                g->remove(a);
                THEN("nothing happens") {
                    REQUIRE(g->empty() == false);
                    REQUIRE(g->size() == 1);
                    REQUIRE(a->hasOwner() == false);
                    REQUIRE(a->owner().lock() == nullptr);
                    REQUIRE(a->numberOfOutputs() == 0);
                    REQUIRE(b->numberOfInputs() == 0);
                }
            }
        }
    }
}

SCENARIO("graph can be walked", "[core/directed_acyclic_graph]")
{
    GIVEN("a graph") {
        GraphPtr g = Graph::create();
        NodePtr nodes[6];
        for (int i = 0; i < 6; ++i) {
            nodes[i] = g->add(to_string(i));
        }
        nodes[0]->connect(nodes[1]);
        nodes[1]->connect(nodes[2]);
        nodes[1]->connect(nodes[3]);
        nodes[3]->connect(nodes[4]);
        nodes[0]->connect(nodes[5]);
        nodes[5]->connect(nodes[3]);
        nodes[3]->connect(nodes[2]);

        std::vector<int> order {0, 1, 5, 3, 4, 2};

        WHEN("traversing") {
            DependencyTraversal t(g);
            THEN("the node order is as expected") {
                int i = 0;
                while (t.hasNext()) {
                    NodePtr node = t.next();
                    REQUIRE(node == nodes[order[i++]]);
                }
                REQUIRE((size_t)i == order.size());
            }
        }

        WHEN("traversing again") {
            DependencyTraversal t(g);
            THEN("the node order is as expected again") {
                int i = 0;
                while (t.hasNext()) {
                    NodePtr node = t.next();
                    REQUIRE(node == nodes[order[i++]]);
                }
                REQUIRE((size_t)i == order.size());
            }
        }
    }
}

SCENARIO("graph cleans up for itself", "[core/directed_acyclic_graph]")
{
    GIVEN("a graph, g, a -> b") {
        GraphPtr graph = Graph::create("graph");
        std::weak_ptr<Graph> wpg(graph);
        std::weak_ptr<Node> wpa;
        std::weak_ptr<Node> wpb;
        {
            NodePtr a = Node::create();
            NodePtr b = Node::create();
            wpa = a;
            wpb = b;
            a->connect(b);
        } // a, b are destroyed, only graph has references to its nodes

        WHEN("graph is cleared") {
            graph->clear();
            THEN("all the nodes are destroyed too") {
                REQUIRE(graph->size() == 0);
                REQUIRE(graph->empty() == true);
                REQUIRE(wpa.expired() == true);
                REQUIRE(wpb.expired() == true);
            }
        }

        WHEN("graph is destroyed") {
            graph.reset();
            THEN("all the nodes are destroyed too") {
                REQUIRE(wpa.expired() == true);
                REQUIRE(wpb.expired() == true);
            }
        }
    }
}
