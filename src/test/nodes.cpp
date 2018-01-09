#include <catch.hpp>
#include "../core/Node.h"

SCENARIO("network can be assembled")
{
    using namespace core;

    GIVEN("a network") {
        ForwardNetwork net;

        REQUIRE(net.size() == 0);

        WHEN("n")
    }

    GIVEN("separated nodes and a network") {
        Node a, b, c;
        ForwardNetwork net;

        REQUIRE(net.size() == 0)

        WHEN("nodes are added to the network") {
            net.add(b);
            net.connect(a, b);
            THEN("the network size increases") {
                REQUIRE(net.size() == 3)
            }
        }

        WHEN("nodes are connected") {

            REQUIRE(a.inputs().size() == 0)
            REQUIRE(b.inputs().size() == 0)
            REQUIRE(a.outputs().size() == 0)
            REQUIRE(b.outputs().size() == 0)

            a.connect(b);

            THEN("the node input/output list populates") {
                REQUIRE(a.outputs().size() == 1)
                REQUIRE(a.outputs()[0].get() == &b)
                REQUIRE(b.inputs().size() == 1)
                REQUIRE(b.inputs()[0] == &a)
            }
        }

        REQUIRE("")
    }
}
