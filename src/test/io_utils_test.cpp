#include "catch.hpp"
#include <core/io_utils/IcsAdapter.h>
#include <core/multidim_image_platform/MultiDimImage.hpp>

#include <fstream>

using namespace core::io_utils;
using namespace core::multidim_image_platform;
using namespace std;

namespace io_utils_test {

// REQUIRE(ics.open("/Users/fodorbalint/Sandbox/testset/new/A15_1_a_DAPI_TH__vGluT1_20x.ics") == true);
// REQUIRE(ics.open("/Users/fodorbalint/Sandbox/testset/old/128_128_128_R15_RGB_2.ics") == true);
// REQUIRE(ics.open("/Users/fodorbalint/Sandbox/testset/old/K32_bassoon_TH_vGluT1_c01_cmle.ics") == true);
// REQUIRE(ics.open("/Users/fodorbalint/Sandbox/testset/ics/B23_4_b_DAPI_TH_vGluT1_bassoon_60x_cmle.ics") == true);

// for (size_t i = 0; i < 32; ++i) {
//     ofstream myfile;
//     myfile.open("plane" + to_string(i) + ".pgm");
//     myfile << "P2\n";
//     myfile << "128 128\n";
//     myfile << "255\n";
//     for (size_t v = 0; v < 128; ++v) {
//         for (size_t u = 0; u < 128; ++u) {
//             auto x = im.at({v, u, i, 0, 0});
//             myfile << (uint32_t)(x*255) << " ";
//         }
//         myfile << "\n";
//     }
//     myfile.close();
// }

SCENARIO("ics read assets/128x128x32_c1_t1_float32.ics", "[core/io_utils]")
{
    IcsAdapter ics;
    REQUIRE(ics.open("assets/128x128x32_c1_t1_float32.ics") == true);

    REQUIRE(ics.valid() == true);
    REQUIRE(ics.dataType() == type_index(typeid(float)));
    REQUIRE(ics.dataType() != type_index(typeid(double)));
    REQUIRE(ics.dataType() != type_index(typeid(int8_t)));
    REQUIRE(ics.dataType() != type_index(typeid(int16_t)));
    REQUIRE(ics.dataType() != type_index(typeid(int32_t)));
    REQUIRE(ics.dataType() != type_index(typeid(int64_t)));
    REQUIRE(ics.dataType() != type_index(typeid(uint8_t)));
    REQUIRE(ics.dataType() != type_index(typeid(uint16_t)));
    REQUIRE(ics.dataType() != type_index(typeid(uint32_t)));
    REQUIRE(ics.dataType() != type_index(typeid(uint64_t)));
    
    REQUIRE_THROWS(ics.read<uint8_t>());
    REQUIRE_THROWS(ics.read<uint16_t>());
    REQUIRE_THROWS(ics.read<int8_t>());
    REQUIRE_THROWS(ics.read<double>());
    REQUIRE_THROWS(ics.read<int64_t>());

    MultiDimImage<float> im = ics.read<float>();

    REQUIRE(im.dims() == 5);
    REQUIRE(im.dim(0) == 128);
    REQUIRE(im.dim(1) == 128);
    REQUIRE(im.dim(2) == 32);
    REQUIRE(im.dim(3) == 1);
    REQUIRE(im.dim(4) == 1);
    
    REQUIRE(im.at({5, 4, 0, 0, 0}) == 0.0f);
    REQUIRE(im.at({5, 4, 1, 0, 0}) == 0.0f);
    REQUIRE(im.at({5, 4, 15, 0, 0}) == 0.0f);
    REQUIRE(im.at({5, 4, 31, 0, 0}) == 0.0f);

    REQUIRE(im.at({69, 26, 0, 0, 0}) == 1.0f);
    REQUIRE(im.at({69, 26, 1, 0, 0}) == 1.0f);
    REQUIRE(im.at({69, 26, 15, 0, 0}) == 1.0f);
    REQUIRE(im.at({69, 26, 31, 0, 0}) == 1.0f);

    REQUIRE(im.at({49, 67, 0, 0, 0}) == 0.5234375f);
    REQUIRE(im.at({49, 67, 1, 0, 0}) == 0.5234375f);
    REQUIRE(im.at({49, 67, 7, 0, 0}) == 0.5234375f);
    REQUIRE(im.at({49, 67, 29, 0, 0}) == 0.5234375f);

    REQUIRE(im.at({36, 91, 0, 0, 0}) == 0.7109375f);
    REQUIRE(im.at({36, 91, 1, 0, 0}) == 0.7109375f);
    REQUIRE(im.at({36, 91, 9, 0, 0}) == 0.7109375f);
    REQUIRE(im.at({36, 91, 17, 0, 0}) == 0.7109375f);

    REQUIRE(im.at({45, 46, 4, 0, 0}) == 0.359375f);
    REQUIRE(im.at({45, 46, 5, 0, 0}) == 1.0f);
    REQUIRE(im.at({44, 38, 8, 0, 0}) == 0.296875f);
    REQUIRE(im.at({44, 38, 13, 0, 0}) == 1.0f);

    MultiDimImage<double> imDouble = ics.readScaledConvert<double>();

    REQUIRE(imDouble.dims() == 5);
    REQUIRE(imDouble.dim(0) == 128);
    REQUIRE(imDouble.dim(1) == 128);
    REQUIRE(imDouble.dim(2) == 32);
    REQUIRE(imDouble.dim(3) == 1);
    REQUIRE(imDouble.dim(4) == 1);

    REQUIRE(imDouble.at({5, 4, 0, 0, 0}) == 0.0);
    REQUIRE(imDouble.at({5, 4, 1, 0, 0}) == 0.0);
    REQUIRE(imDouble.at({5, 4, 15, 0, 0}) == 0.0);
    REQUIRE(imDouble.at({5, 4, 31, 0, 0}) == 0.0);

    REQUIRE(imDouble.at({69, 26, 0, 0, 0}) == 1.0);
    REQUIRE(imDouble.at({69, 26, 1, 0, 0}) == 1.0);
    REQUIRE(imDouble.at({69, 26, 15, 0, 0}) == 1.0);
    REQUIRE(imDouble.at({69, 26, 31, 0, 0}) == 1.0);

    REQUIRE(imDouble.at({49, 67, 0, 0, 0}) == 0.5234375);
    REQUIRE(imDouble.at({49, 67, 1, 0, 0}) == 0.5234375);
    REQUIRE(imDouble.at({49, 67, 7, 0, 0}) == 0.5234375);
    REQUIRE(imDouble.at({49, 67, 29, 0, 0}) == 0.5234375);

    REQUIRE(imDouble.at({36, 91, 0, 0, 0}) == 0.7109375);
    REQUIRE(imDouble.at({36, 91, 1, 0, 0}) == 0.7109375);
    REQUIRE(imDouble.at({36, 91, 9, 0, 0}) == 0.7109375);
    REQUIRE(imDouble.at({36, 91, 17, 0, 0}) == 0.7109375);

    REQUIRE(imDouble.at({45, 46, 4, 0, 0}) == 0.359375);
    REQUIRE(imDouble.at({45, 46, 5, 0, 0}) == 1.0);
    REQUIRE(imDouble.at({44, 38, 8, 0, 0}) == 0.296875);
    REQUIRE(imDouble.at({44, 38, 13, 0, 0}) == 1.0);

    MultiDimImage<uint8_t> imUint8 = ics.readScaledConvert<uint8_t>();

    REQUIRE(imUint8.dims() == 5);
    REQUIRE(imUint8.dim(0) == 128);
    REQUIRE(imUint8.dim(1) == 128);
    REQUIRE(imUint8.dim(2) == 32);
    REQUIRE(imUint8.dim(3) == 1);
    REQUIRE(imUint8.dim(4) == 1);
    REQUIRE(imUint8.at({5, 4, 0, 0, 0}) == 0.0);
    REQUIRE(imUint8.at({5, 4, 1, 0, 0}) == 0.0);
    REQUIRE(imUint8.at({5, 4, 15, 0, 0}) == 0.0);
    REQUIRE(imUint8.at({5, 4, 31, 0, 0}) == 0.0);
    REQUIRE(imUint8.at({69, 26, 0, 0, 0}) == static_cast<uint8_t>(255));
    REQUIRE(imUint8.at({69, 26, 1, 0, 0}) == static_cast<uint8_t>(255));
    REQUIRE(imUint8.at({69, 26, 15, 0, 0}) == static_cast<uint8_t>(255));
    REQUIRE(imUint8.at({69, 26, 31, 0, 0}) == static_cast<uint8_t>(255));
    REQUIRE(imUint8.at({49, 67, 0, 0, 0}) == static_cast<uint8_t>(0.5234375 * 255));
    REQUIRE(imUint8.at({49, 67, 1, 0, 0}) == static_cast<uint8_t>(0.5234375 * 255));
    REQUIRE(imUint8.at({49, 67, 7, 0, 0}) == static_cast<uint8_t>(0.5234375 * 255));
    REQUIRE(imUint8.at({49, 67, 29, 0, 0}) == static_cast<uint8_t>(0.5234375 * 255));
    REQUIRE(imUint8.at({36, 91, 0, 0, 0}) == static_cast<uint8_t>(0.7109375 * 255));
    REQUIRE(imUint8.at({36, 91, 1, 0, 0}) == static_cast<uint8_t>(0.7109375 * 255));
    REQUIRE(imUint8.at({36, 91, 9, 0, 0}) == static_cast<uint8_t>(0.7109375 * 255));
    REQUIRE(imUint8.at({36, 91, 17, 0, 0}) == static_cast<uint8_t>(0.7109375 * 255));
    REQUIRE(imUint8.at({45, 46, 4, 0, 0}) == static_cast<uint8_t>(0.359375 * 255));
    REQUIRE(imUint8.at({45, 46, 5, 0, 0}) == static_cast<uint8_t>(1.0 * 255));
    REQUIRE(imUint8.at({44, 38, 8, 0, 0}) == static_cast<uint8_t>(0.296875 * 255));
    REQUIRE(imUint8.at({44, 38, 13, 0, 0}) == static_cast<uint8_t>(1.0 * 255));

    ics.close();

    REQUIRE(ics.valid() == false);
}

SCENARIO("ics read assets/A15_1_a_DAPI_TH__vGluT1_20x.ics", "[core/io_utils]")
{
    IcsAdapter ics;
    REQUIRE(ics.open("assets/A15_1_a_DAPI_TH__vGluT1_20x.ics") == true);

    REQUIRE(ics.valid() == true);
    REQUIRE(ics.dataType() == type_index(typeid(uint16_t)));
    
    REQUIRE(ics.dataType() != type_index(typeid(float)));
    REQUIRE(ics.dataType() != type_index(typeid(double)));
    REQUIRE(ics.dataType() != type_index(typeid(int8_t)));
    REQUIRE(ics.dataType() != type_index(typeid(int16_t)));
    REQUIRE(ics.dataType() != type_index(typeid(int32_t)));
    REQUIRE(ics.dataType() != type_index(typeid(int64_t)));
    REQUIRE(ics.dataType() != type_index(typeid(uint8_t)));
    REQUIRE(ics.dataType() != type_index(typeid(uint32_t)));
    REQUIRE(ics.dataType() != type_index(typeid(uint64_t)));

    MultiDimImage<float> im = ics.read<float>();

    // REQUIRE(im.dims() == 5);
    // REQUIRE(im.dim(0) == 128);
    // REQUIRE(im.dim(1) == 128);
    // REQUIRE(im.dim(2) == 32);
    // REQUIRE(im.dim(3) == 1);
    // REQUIRE(im.dim(4) == 1);
    
    // REQUIRE(im.at({5, 4, 0, 0, 0}) == 0.0f);
    // REQUIRE(im.at({5, 4, 1, 0, 0}) == 0.0f);
    // REQUIRE(im.at({5, 4, 15, 0, 0}) == 0.0f);
    // REQUIRE(im.at({5, 4, 31, 0, 0}) == 0.0f);

    // REQUIRE(im.at({69, 26, 0, 0, 0}) == 1.0f);
    // REQUIRE(im.at({69, 26, 1, 0, 0}) == 1.0f);
    // REQUIRE(im.at({69, 26, 15, 0, 0}) == 1.0f);
    // REQUIRE(im.at({69, 26, 31, 0, 0}) == 1.0f);

    // REQUIRE(im.at({49, 67, 0, 0, 0}) == 0.5234375f);
    // REQUIRE(im.at({49, 67, 1, 0, 0}) == 0.5234375f);
    // REQUIRE(im.at({49, 67, 7, 0, 0}) == 0.5234375f);
    // REQUIRE(im.at({49, 67, 29, 0, 0}) == 0.5234375f);

    // REQUIRE(im.at({36, 91, 0, 0, 0}) == 0.7109375f);
    // REQUIRE(im.at({36, 91, 1, 0, 0}) == 0.7109375f);
    // REQUIRE(im.at({36, 91, 9, 0, 0}) == 0.7109375f);
    // REQUIRE(im.at({36, 91, 17, 0, 0}) == 0.7109375f);

    // REQUIRE(im.at({45, 46, 4, 0, 0}) == 0.359375f);
    // REQUIRE(im.at({45, 46, 5, 0, 0}) == 1.0f);
    // REQUIRE(im.at({44, 38, 8, 0, 0}) == 0.296875f);
    // REQUIRE(im.at({44, 38, 13, 0, 0}) == 1.0f);

    ics.close();

    REQUIRE(ics.valid() == false);
}

}