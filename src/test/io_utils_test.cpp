#include "catch.hpp"
#include <core/io_utils/IcsAdapter.h>
#include <core/multidim_image_platform/MultiDimImage.hpp>

using namespace core::io_utils;
using namespace core::multidim_image_platform;
using namespace std;

namespace io_utils_test {

SCENARIO("ics read", "[core/io_utils]")
{
    IcsAdapter ics;
    REQUIRE(ics.open("assets/128x128x32_c1_t1_8bit.ics") == true);

    REQUIRE(ics.valid() == true);
    REQUIRE(ics.dataType() == typeid(uint8_t).hash_code());
    
    MultiDimImage<int8_t> im = ics.read<int8_t>();

    REQUIRE(im.dims() == 3);
    REQUIRE(im.dim(0) == 128);
    REQUIRE(im.dim(1) == 128);
    REQUIRE(im.dim(2) == 32);

    ics.close();

    REQUIRE(ics.valid() == false);
}

}