#define BENCHPRESS_CONFIG_MAIN
#include <benchpress.hpp>

#include <core/multidim_image_platform/MultiDimImage.hpp>

// BENCHMARK("example", [](benchpress::context* ctx) {
// 	for (size_t i = 0; i < ctx->num_iterations(); ++i) {
// 		std::cout << "hello" << std::endl;
// 	}
// })

BENCHMARK("100x100x16x3 reorder", [](benchpress::context* ctx)
{
    core::multidim_image_platform::MultiDimImage<uint8_t> image({100, 100, 16, 3});
    ctx->set_bytes(100*100*16*3*sizeof(uint8_t));
    ctx->reset_timer();
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		image.reorderDims({1, 0, 2, 3});
	}
})

// ---

// SCENARIO("converting between types", "[core/multidim_image_platform]")
// {
//     GIVEN("different types of images") {
//         MultiDimImage<std::int8_t> im8s({16, 24});
//         MultiDimImage<std::uint64_t> im64u({24, 32});
//         MultiDimImage<float> imf({64, 32});
//         MultiDimImage<double> imd({32, 64});

//         im8s.at({7, 9}) = -2;
//         im8s.at({7, 10}) = 127;
//         im8s.at({7, 11}) = 92;
//         im8s.at({7, 12}) = 0;

//         im64u.at({17, 13}) = 42000;
//         im64u.at({17, 14}) = 0;
//         im64u.at({17, 15}) = 18446744073709551615ULL;
//         im64u.at({17, 16}) = 78;

//         imf.at({17, 23}) = 0.0f;
//         imf.at({17, 24}) = 42.0f;
//         imf.at({17, 25}) = 42.4242f;
//         imf.at({17, 26}) = 1042.4242f;

//         imd.at({11, 47}) = 2024.0;
//         imd.at({11, 48}) = 0.0;
//         imd.at({11, 49}) = 2024.202420242024;
//         imd.at({11, 50}) = 42.0;

//         WHEN("copy converting from im8s to im64u") {
//             im64u.convertCopyFrom(im8s);
//             THEN("it is correct") {
//                 REQUIRE(im64u.size() == 16 * 24);
//                 REQUIRE(im64u.byteSize() == 16 * 24 * sizeof(std::uint64_t));
//                 REQUIRE(im64u.dims() == 2);
//                 REQUIRE(im64u.dim(0) == 16);
//                 REQUIRE(im64u.dim(1) == 24);
//                 REQUIRE(im64u.at({7, 10}) == 127);
//                 REQUIRE(im64u.at({7, 11}) == 92);
//                 REQUIRE(im64u.at({7, 12}) == 0);
//             }
//         }

//         WHEN("copy converting from im64u to im8s") {
//             im8s.convertCopyFrom(im64u);
//             THEN("it is correct") {
//                 REQUIRE(im8s.size() == 24 * 32);
//                 REQUIRE(im8s.byteSize() == 24 * 32 * sizeof(std::int8_t));
//                 REQUIRE(im8s.dims() == 2);
//                 REQUIRE(im8s.dim(0) == 24);
//                 REQUIRE(im8s.dim(1) == 32);
//                 REQUIRE(im8s.at({17, 14}) == 0);
//                 REQUIRE(im8s.at({17, 16}) == 78);
//             }
//         }

//         WHEN("copy converting from im8s to imf") {
//             imf.convertCopyFrom(im8s);
//             THEN("it is correct") {
//                 REQUIRE(imf.size() == 16 * 24);
//                 REQUIRE(imf.byteSize() == 16 * 24 * sizeof(float));
//                 REQUIRE(imf.dims() == 2);
//                 REQUIRE(imf.dim(0) == 16);
//                 REQUIRE(imf.dim(1) == 24);
//                 REQUIRE(imf.at({7, 9}) == -2.f);
//                 REQUIRE(imf.at({7, 10}) == 127.f);
//                 REQUIRE(imf.at({7, 11}) == 92.f);
//                 REQUIRE(imf.at({7, 12}) == 0.f);
//             }
//         }

//         WHEN("copy converting from imf to im8s") {
//             im8s.convertCopyFrom(imf);
//             THEN("it is correct") {
//                 REQUIRE(im8s.size() == 64 * 32);
//                 REQUIRE(im8s.byteSize() == 64 * 32 * sizeof(std::int8_t));
//                 REQUIRE(im8s.dims() == 2);
//                 REQUIRE(im8s.dim(0) == 64);
//                 REQUIRE(im8s.dim(1) == 32);
//                 REQUIRE(im8s.at({17, 23}) == 0);
//                 REQUIRE(im8s.at({17, 24}) == 42);
//                 REQUIRE(im8s.at({17, 25}) == 42);
//             }
//         }

//         WHEN("copy converting from imd to im8s") {
//             im8s.convertCopyFrom(imd);
//             THEN("it is correct") {
//                 REQUIRE(im8s.size() == 64 * 32);
//                 REQUIRE(im8s.byteSize() == 64 * 32 * sizeof(std::int8_t));
//                 REQUIRE(im8s.dims() == 2);
//                 REQUIRE(im8s.dim(0) == 32);
//                 REQUIRE(im8s.dim(1) == 64);
//                 REQUIRE(im8s.at({11, 48}) == 0);
//                 REQUIRE(im8s.at({11, 50}) == 42);
//             }
//         }

        
//     }
// }