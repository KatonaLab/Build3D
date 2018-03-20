#include "catch.hpp"
#include <core/io_utils/IcsAdapter.h>
#include <core/multidim_image_platform/MultiDimImage.hpp>

#include <iomanip>
#include <fstream>

using namespace core::io_utils;
using namespace core::multidim_image_platform;
using namespace std;

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

    REQUIRE(im.at({26, 69, 0, 0, 0}) == 1.0f);
    REQUIRE(im.at({26, 69, 1, 0, 0}) == 1.0f);
    REQUIRE(im.at({26, 69, 15, 0, 0}) == 1.0f);
    REQUIRE(im.at({26, 69, 31, 0, 0}) == 1.0f);

    REQUIRE(im.at({67, 49, 0, 0, 0}) == 0.5234375f);
    REQUIRE(im.at({67, 49, 1, 0, 0}) == 0.5234375f);
    REQUIRE(im.at({67, 49, 7, 0, 0}) == 0.5234375f);
    REQUIRE(im.at({67, 49, 29, 0, 0}) == 0.5234375f);

    REQUIRE(im.at({91, 36, 0, 0, 0}) == 0.7109375f);
    REQUIRE(im.at({91, 36, 1, 0, 0}) == 0.7109375f);
    REQUIRE(im.at({91, 36, 9, 0, 0}) == 0.7109375f);
    REQUIRE(im.at({91, 36, 17, 0, 0}) == 0.7109375f);

    REQUIRE(im.at({46, 45, 4, 0, 0}) == 0.359375f);
    REQUIRE(im.at({46, 45, 5, 0, 0}) == 1.0f);
    REQUIRE(im.at({38, 44, 8, 0, 0}) == 0.296875f);
    REQUIRE(im.at({38, 44, 13, 0, 0}) == 1.0f);

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

    REQUIRE(imDouble.at({26, 69, 0, 0, 0}) == 1.0);
    REQUIRE(imDouble.at({26, 69, 1, 0, 0}) == 1.0);
    REQUIRE(imDouble.at({26, 69, 15, 0, 0}) == 1.0);
    REQUIRE(imDouble.at({26, 69, 31, 0, 0}) == 1.0);

    REQUIRE(imDouble.at({67, 49, 0, 0, 0}) == 0.5234375);
    REQUIRE(imDouble.at({67, 49, 1, 0, 0}) == 0.5234375);
    REQUIRE(imDouble.at({67, 49, 7, 0, 0}) == 0.5234375);
    REQUIRE(imDouble.at({67, 49, 29, 0, 0}) == 0.5234375);

    REQUIRE(imDouble.at({91, 36, 0, 0, 0}) == 0.7109375);
    REQUIRE(imDouble.at({91, 36, 1, 0, 0}) == 0.7109375);
    REQUIRE(imDouble.at({91, 36, 9, 0, 0}) == 0.7109375);
    REQUIRE(imDouble.at({91, 36, 17, 0, 0}) == 0.7109375);

    REQUIRE(imDouble.at({46, 45, 4, 0, 0}) == 0.359375);
    REQUIRE(imDouble.at({46, 45, 5, 0, 0}) == 1.0);
    REQUIRE(imDouble.at({38, 44, 8, 0, 0}) == 0.296875);
    REQUIRE(imDouble.at({38, 44, 13, 0, 0}) == 1.0);

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
    REQUIRE(imUint8.at({26, 69, 0, 0, 0}) == static_cast<uint8_t>(255));
    REQUIRE(imUint8.at({26, 69, 1, 0, 0}) == static_cast<uint8_t>(255));
    REQUIRE(imUint8.at({26, 69, 15, 0, 0}) == static_cast<uint8_t>(255));
    REQUIRE(imUint8.at({26, 69, 31, 0, 0}) == static_cast<uint8_t>(255));
    REQUIRE(imUint8.at({67, 49, 0, 0, 0}) == static_cast<uint8_t>(0.5234375 * 255));
    REQUIRE(imUint8.at({67, 49, 1, 0, 0}) == static_cast<uint8_t>(0.5234375 * 255));
    REQUIRE(imUint8.at({67, 49, 7, 0, 0}) == static_cast<uint8_t>(0.5234375 * 255));
    REQUIRE(imUint8.at({67, 49, 29, 0, 0}) == static_cast<uint8_t>(0.5234375 * 255));
    REQUIRE(imUint8.at({91, 36, 0, 0, 0}) == static_cast<uint8_t>(0.7109375 * 255));
    REQUIRE(imUint8.at({91, 36, 1, 0, 0}) == static_cast<uint8_t>(0.7109375 * 255));
    REQUIRE(imUint8.at({91, 36, 9, 0, 0}) == static_cast<uint8_t>(0.7109375 * 255));
    REQUIRE(imUint8.at({91, 36, 17, 0, 0}) == static_cast<uint8_t>(0.7109375 * 255));
    REQUIRE(imUint8.at({46, 45, 4, 0, 0}) == static_cast<uint8_t>(0.359375 * 255));
    REQUIRE(imUint8.at({46, 45, 5, 0, 0}) == static_cast<uint8_t>(1.0 * 255));
    REQUIRE(imUint8.at({38, 44, 8, 0, 0}) == static_cast<uint8_t>(0.296875 * 255));
    REQUIRE(imUint8.at({38, 44, 13, 0, 0}) == static_cast<uint8_t>(1.0 * 255));

    ics.close();

    REQUIRE(ics.valid() == false);
}

template <typename T>
void writePGM(MultiDimImage<T>& image, std::string fname)
{
    // MultiDimImage<uint8_t> tmp;
    // tmp.scaledCopyFrom(image);

    ofstream ofile;
    ofile.open(fname + ".pgm");

    auto& planes = image.unsafeData();

    ofile << "P2\n" << image.dim(0) << " " << image.dim(1) << "\n255\n";
    int c = 0;
    for (int i = 0; i < image.dim(0); ++i) {
        for (int j = 0; j < image.dim(1); ++j) {
            // uint8_t z = std::max(std::min((uint8_t)round(planes[0][c++]), (uint8_t)255), (uint8_t)0);
            // z = (uint8_t)i;
            ofile << (int)(planes[0][c++]*0.01) << " ";
        }
        // cout << (int)z << " ";
        ofile << "\n";
    }

    // ofile << "P2\n256 256\n255\n";

    // int c = 0;
    // for (int i = 0; i < 256; ++i) {
    //     for (int j = 0; j < 256; ++j) {
    //         uint8_t z = (uint8_t)i;
    //         ofile << "127 ";
    //     }
    //     ofile << "\n";
    // }

    // std::ostream_iterator<std::string> oi(ofile, " ");
    // std::copy(planes[0].begin(), planes[0].end(), oi);

    // ofile.write(reinterpret_cast<const char*>(planes[0].data()), sizeof(char) * planes[0].size());
    // for (auto& plane : planes) {
    //     ofile.write(reinterpret_cast<const char*>(plane.data()), sizeof(T) * plane.size());
    // }

    ofile.close();
}

SCENARIO("ics read assets/B23_4_b_DAPI_TH_vGluT1_bassoon_60x_cmle.ics", "[core/io_utils]")
{
    IcsAdapter ics;
    REQUIRE(ics.open("assets/B23_4_b_DAPI_TH_vGluT1_bassoon_60x_cmle.ics") == true);

    REQUIRE(ics.valid() == true);
    REQUIRE(ics.dataType() == type_index(typeid(float)));

    REQUIRE(ics.dataType() != type_index(typeid(double)));
    REQUIRE(ics.dataType() != type_index(typeid(int8_t)));
    REQUIRE(ics.dataType() != type_index(typeid(int16_t)));
    REQUIRE(ics.dataType() != type_index(typeid(int32_t)));
    REQUIRE(ics.dataType() != type_index(typeid(int64_t)));
    REQUIRE(ics.dataType() != type_index(typeid(uint8_t)));
    REQUIRE(ics.dataType() != type_index(typeid(uint32_t)));
    REQUIRE(ics.dataType() != type_index(typeid(uint64_t)));

    REQUIRE_THROWS(ics.read<double>());
    REQUIRE_THROWS(ics.read<int8_t>());
    REQUIRE_THROWS(ics.read<int64_t>());
    REQUIRE_THROWS(ics.read<uint8_t>());
    REQUIRE_THROWS(ics.read<uint64_t>());

    MultiDimImage<float> im = ics.read<float>();

    REQUIRE(im.dims() == 4);
    REQUIRE(im.dim(0) == 2048);
    REQUIRE(im.dim(1) == 2048);
    REQUIRE(im.dim(2) == 15);
    REQUIRE(im.dim(3) == 4);

    REQUIRE(im.at({0, 0, 0, 0}) == 206.9238433837890625);
    REQUIRE(im.at({1, 0, 0, 0}) == 256.381744384765625);
    REQUIRE(im.at({0, 1, 0, 0}) == 165.5158843994140625);
    REQUIRE(im.at({0, 0, 1, 0}) == 340.086639404296875);
    REQUIRE(im.at({0, 0, 0, 1}) == 162.71307373046875);
    REQUIRE(im.at({2047, 0, 0, 0}) == 29.1736927032470703125);
    REQUIRE(im.at({0, 2047, 0, 0}) == 282.6539306640625);
    REQUIRE(im.at({2047, 2047, 0, 0}) == 112.78113555908203125);

    im = ics.read<float>(true);

    REQUIRE(im.dims() == 4);
    REQUIRE(im.dim(0) == 2048);
    REQUIRE(im.dim(1) == 2048);
    REQUIRE(im.dim(2) == 15);
    REQUIRE(im.dim(3) == 4);

    REQUIRE(im.at({0, 0, 0, 0}) == 206.9238433837890625);
    REQUIRE(im.at({1, 0, 0, 0}) == 256.381744384765625);
    REQUIRE(im.at({0, 1, 0, 0}) == 165.5158843994140625);
    REQUIRE(im.at({0, 0, 1, 0}) == 340.086639404296875);
    REQUIRE(im.at({0, 0, 0, 1}) == 162.71307373046875);
    REQUIRE(im.at({2047, 0, 0, 0}) == 29.1736927032470703125);
    REQUIRE(im.at({0, 2047, 0, 0}) == 282.6539306640625);
    REQUIRE(im.at({2047, 2047, 0, 0}) == 112.78113555908203125);

    ics.close();

    REQUIRE(ics.valid() == false);
}

// template <typename T>
// void writeBinary(MultiDimImage<T>& image, std::string fname)
// {
//     ofstream ofile;
//     ofile.open(fname, fstream::out | fstream::binary);
//     auto& planes = image.unsafeData();
//     for (auto& plane : planes) {
//         ofile.write(reinterpret_cast<const char*>(plane.data()), sizeof(T) * plane.size());
//     }
//     ofile.close();
// }

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

    REQUIRE_THROWS(ics.read<float>());
    REQUIRE_THROWS(ics.read<double>());
    REQUIRE_THROWS(ics.read<int8_t>());
    REQUIRE_THROWS(ics.read<int64_t>());
    REQUIRE_THROWS(ics.read<uint8_t>());
    REQUIRE_THROWS(ics.read<uint64_t>());

    MultiDimImage<uint16_t> im = ics.read<uint16_t>();

    REQUIRE(im.dims() == 4);
    REQUIRE(im.dim(0) == 2);
    REQUIRE(im.dim(1) == 1024);
    REQUIRE(im.dim(2) == 1024);
    REQUIRE(im.dim(3) == 20);

    REQUIRE(im.at({0, 0, 0, 0}) == 216);
    REQUIRE(im.at({1, 0, 0, 0}) == 96);
    REQUIRE(im.at({0, 1, 0, 0}) == 53);
    REQUIRE(im.at({0, 0, 1, 0}) == 227);
    REQUIRE(im.at({0, 0, 0, 1}) == 121);

    REQUIRE(im.at({0, 378, 709, 19}) == 1954);
    REQUIRE(im.at({0, 738, 805, 4}) == 773);
    REQUIRE(im.at({0, 375, 708, 15}) == 2238);
    REQUIRE(im.at({0, 667, 684, 15}) == 501);
    REQUIRE(im.at({1, 686, 671, 15}) == 1028);
    REQUIRE(im.at({1, 336, 760, 15}) == 4095);
    REQUIRE(im.at({1, 331, 724, 15}) == 2702);
    REQUIRE(im.at({1, 328, 752, 9}) == 2032);

    im = ics.read<uint16_t>(true);

    REQUIRE(im.dims() == 4);
    REQUIRE(im.dim(0) == 1024);
    REQUIRE(im.dim(1) == 1024);
    REQUIRE(im.dim(2) == 20);
    REQUIRE(im.dim(3) == 2);

    REQUIRE(im.at({0, 0, 0, 0}) == 216);
    REQUIRE(im.at({0, 0, 0, 1}) == 96);
    REQUIRE(im.at({1, 0, 0, 0}) == 53);
    REQUIRE(im.at({0, 1, 0, 0}) == 227);
    REQUIRE(im.at({0, 0, 1, 0}) == 121);

    REQUIRE(im.at({378, 709, 19, 0}) == 1954);
    REQUIRE(im.at({738, 805, 4 , 0}) == 773);
    REQUIRE(im.at({375, 708, 15, 0}) == 2238);
    REQUIRE(im.at({667, 684, 15, 0}) == 501);
    REQUIRE(im.at({686, 671, 15, 1}) == 1028);
    REQUIRE(im.at({336, 760, 15, 1}) == 4095);
    REQUIRE(im.at({331, 724, 15, 1}) == 2702);
    REQUIRE(im.at({328, 752, 9 , 1}) == 2032);

    ics.close();

    REQUIRE(ics.valid() == false);
}