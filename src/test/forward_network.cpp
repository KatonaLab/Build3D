#include "catch.hpp"

#include <algorithm>
#include <list>
#include <string>

#include <core/forward_network/ForwardNetwork.h>
#include <core/forward_network/Group.h>
#include <core/forward_network/Node.h>

using namespace core;

SCENARIO("nodes can be added and removed", "[core/forward_network]")
{
    GIVEN("an empty network") {
        ForwardNetwork net;

        REQUIRE(net.empty() == true);
        REQUIRE(net.size() == 0);

        WHEN("some nodes added") {
            NodePtr node1 = net.addNode();
            NodePtr node2 = net.addNode();
            NodePtr node3 = net.addNode();

            REQUIRE(node1->valid() == true);
            REQUIRE(node2->valid() == true);
            REQUIRE(node3->valid() == true);

            THEN("the size increased") {
                REQUIRE(net.size() == 3);
                REQUIRE(net.empty() == false);
            }

            AND_WHEN("one removed") {
                net.remove(node2);

                THEN("the size decreased") {
                    REQUIRE(net.size() == 2);
                    REQUIRE(net.empty() == false);
                }
            }

            AND_WHEN("the net is cleared") {
                net.clear();
                THEN("the net becomes empty") {
                    REQUIRE(net.empty() == true);
                    REQUIRE(net.size() == 0);
                }
                AND_THEN("the nodes become invalid") {
                    REQUIRE(node1->valid() == false);
                    REQUIRE(node2->valid() == false);
                    REQUIRE(node3->valid() == false);
                }
            }
        }
    }
}

SCENARIO("node groups can be managed", "[core/forward_network]")
{
    GIVEN("a network without any node groups") {
        ForwardNetwork net;
        NodePtr node1 = net.addNode();
        NodePtr node2 = net.addNode();

        WHEN("a group added") {
            GroupPtr group = net.addGroup();
            
            REQUIRE(group->valid() == true);

            THEN("the size also increased") {
                REQUIRE(net.size() == 3);
                REQUIRE(net.empty() == false);
            }

            AND_WHEN("removed") {
                net.remove(group);

                THEN("the size decreased") {
                    REQUIRE(net.size() == 2);
                    REQUIRE(net.empty() == false);
                }

                AND_THEN("the group become invalid") {
                    REQUIRE(group->valid() == false);
                }
            }
        }
    }

    GIVEN("a network with some nodes and a group") {
        ForwardNetwork net;
        NodePtr node1 = net.addNode();
        NodePtr node2 = net.addNode();
        GroupPtr group = net.addGroup();
        
        REQUIRE(group->size() == 0);
        REQUIRE(group->empty() == true);

        WHEN("a group is populated with a node") {
            NodePtr node = group->addNode();
            
            THEN("the net size is increased") {
                REQUIRE(net.size() == 4);
                REQUIRE(net.empty() == false);
            }

            AND_THEN("the group size also increased") {
                REQUIRE(group->size() == 1);
                REQUIRE(group->empty() == false);
            }

            AND_WHEN("the group is removed") {
                net.remove(group);
                THEN("the size is decreased") {
                    REQUIRE(net.size() == 2);
                    REQUIRE(net.empty() == false);
                }

                AND_THEN("the group and its child node become invalid") {
                    REQUIRE(group->valid() == false);
                    REQUIRE(node->valid() == false);
                }
            }
        }

        WHEN("a group is populated with a group node") {
            GroupPtr subgroup = group->addGroup();
            
            THEN("the net size is increased") {
                REQUIRE(net.size() == 4);
                REQUIRE(net.empty() == false);
            }

            AND_THEN("the group size also increased") {
                REQUIRE(group->size() == 1);
                REQUIRE(group->empty() == false);
            }

            AND_WHEN("the group is removed") {
                net.remove(group);
                THEN("the size is decreased") {
                    REQUIRE(net.size() == 2);
                    REQUIRE(net.empty() == false);
                }

                AND_THEN("the group and its child node become invalid") {
                    REQUIRE(group->valid() == false);
                    REQUIRE(subgroup->valid() == false);
                }
            }
        }
    }
}

SCENARIO("nodes can be connected", "[core/forward_network]")
{
    GIVEN("a network") {
        ForwardNetwork net;
        NodePtr node1 = net.addNode("node #1");
        NodePtr node2 = net.addNode("node #2");
        GroupPtr group1 = net.addGroup("group #1");
        NodePtr node3 = group1->addNode("node #3");
        GroupPtr group2 = net.addGroup("group #2");
        GroupPtr group3 = group2->addGroup("group #3");
        NodePtr node4 = group3->addNode("node #4");

        WHEN("two nodes in the same group connected") {
            REQUIRE(net.connect(node1, node2) == true);

            THEN("the first node has the second one in its output list") {
                REQUIRE(node1->outputs()[0] == node2);
            }

            AND_THEN("the second node has the first one in its input list") {
                REQUIRE(node2->inputs()[0] == node1);
            }
        }

        WHEN("two nodes in different groups connected") {
            REQUIRE(net.connect(node1, node2) == true);
            REQUIRE(net.connect(node1, node3) == true);

            THEN("the first node has the second one in its output list") {
                REQUIRE(node1->outputs()[0] == node2);
                REQUIRE(node1->outputs()[1] == node3);
            }

            AND_THEN("the second node has the first one in its input list") {
                REQUIRE(node2->inputs()[0] == node1);
                REQUIRE(node3->inputs()[0] == node1);
            }
        }
    }
}

SCENARIO("forward network can detect cycles", "[core/forward_network]")
{
    GIVEN("a network") {
        ForwardNetwork net;
        NodePtr node1 = net.addNode();
        NodePtr node2 = net.addNode();
        NodePtr node3 = net.addNode();
        GroupPtr group1 = net.addGroup();
        NodePtr node4 = group1->addNode();

        REQUIRE(net.connect(node1, node2) == true);
        REQUIRE(net.connect(node2, node3) == true);
        REQUIRE(net.connect(node3, node4) == true);

        WHEN("trying to do a trivial cycle") {
            REQUIRE(net.connect(node2, node1) == false);
            THEN("nothing happens") {
                REQUIRE(node1->outputs()[0] == node2);
                REQUIRE(node2->inputs()[0] == node1);
            }
        }

        WHEN("trying to do a cycle in the same group") {
            REQUIRE(net.connect(node3, node1) == false);
            THEN("nothing happens") {
                REQUIRE(node1->inputs().size() == 0);
                REQUIRE(node1->outputs().size() == 1);
                REQUIRE(node2->inputs().size() == 1);
                REQUIRE(node2->outputs().size() == 1);
                REQUIRE(node3->inputs().size() == 1);
                REQUIRE(node3->outputs().size() == 1);
                REQUIRE(node4->inputs().size() == 1);
                REQUIRE(node4->outputs().size() == 0);

                REQUIRE(node1->outputs()[0] == node2);
                REQUIRE(node2->inputs()[0] == node1);
                REQUIRE(node2->outputs()[0] == node3);
                REQUIRE(node3->inputs()[0] == node2);
                REQUIRE(node3->outputs()[0] == node4);
                REQUIRE(node4->inputs()[0] == node3);
            }
        }

        WHEN("trying to do a cycle with nodes in different groups") {
            REQUIRE(net.connect(node4, node1) == false);
            THEN("nothing happens") {
                REQUIRE(node1->inputs().size() == 0);
                REQUIRE(node1->outputs().size() == 1);
                REQUIRE(node2->inputs().size() == 1);
                REQUIRE(node2->outputs().size() == 1);
                REQUIRE(node3->inputs().size() == 1);
                REQUIRE(node3->outputs().size() == 1);
                REQUIRE(node4->inputs().size() == 1);
                REQUIRE(node4->outputs().size() == 0);

                REQUIRE(node1->outputs()[0] == node2);
                REQUIRE(node2->inputs()[0] == node1);
                REQUIRE(node2->outputs()[0] == node3);
                REQUIRE(node3->inputs()[0] == node2);
                REQUIRE(node3->outputs()[0] == node4);
                REQUIRE(node4->inputs()[0] == node3);
            }
        }
    }
}

SCENARIO("forward network can be traversed", "[core/forward_network]")
{
    GIVEN("a network") {
        ForwardNetwork net("example_net");
        NodePtr nodes[14];
        for (int i = 0; i < 5; ++i) {
            nodes[i] = net.addNode("node " + std::to_string(i));
        }
        GroupPtr groups[3];
        for (int i = 0; i < 3; ++i) {
            groups[i] = net.addGroup("group " + std::to_string(i));
        }
        nodes[9] = groups[0]->addNode("node 9");
        nodes[10] = groups[0]->addNode("node 10");
        nodes[8] = groups[1]->addNode("node 8");
        nodes[11] = groups[2]->addNode("node 11");
        nodes[12] = net.addNode("node 12");
        nodes[13] = net.addNode("node 13");

        net.connect(nodes[0], nodes[2]);
        net.connect(nodes[1], nodes[2]);
        net.connect(nodes[2], nodes[3]);
        net.connect(nodes[2], nodes[5]);
        net.connect(nodes[4], nodes[8]);
        net.connect(nodes[8], nodes[11]);
        net.connect(nodes[11], nodes[12]);
        net.connect(nodes[11], nodes[13]);
        net.connect(nodes[3], nodes[9]);
        net.connect(nodes[9], nodes[10]);
        net.connect(nodes[10], nodes[13]);

        WHEN("walking the graph") {
            std::list<std::string> output;
            net.walk([&output](NodePtr node) { output.push_back(node->name()); });
            THEN("the order is correct") {
                REQUIRE(output.size() == 11);
                std::list<std::string> order = {
                    "node 0", "node 1", "node 2", "node 3",
                    "node 4", "node 9", "node 10", "node 8",
                    "node 11", "node 12", "node 13"
                    };
                REQUIRE(std::equal(output.begin(), output.end(), order.begin()) == true);
            }
        }
    }
}