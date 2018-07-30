#define BENCHPRESS_CONFIG_MAIN
#include <benchpress.hpp>

#include <core/multidim_image_platform/MultiDimImage.hpp>
#include <core/io_utils/IcsAdapter.h>

BENCHMARK("create MultiDimImage 100x100x16x3 #multidimimage #create", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::multidim_image_platform::MultiDimImage<uint8_t> image({100, 100, 16, 3});
        benchpress::escape(&image);
	}
})

BENCHMARK("create MultiDimImage 3x16x100x100 #multidimimage #create", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::multidim_image_platform::MultiDimImage<uint8_t> image({3, 16, 100, 100});
        benchpress::escape(&image);
	}
})

BENCHMARK("create MultiDimImage 2048x2048x16x3 #multidimimage #create", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::multidim_image_platform::MultiDimImage<uint8_t> image({2048, 2048, 16, 3});
        benchpress::escape(&image);
	}
})

BENCHMARK("create MultiDimImage 16x3x2048x2048 #multidimimage #create", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::multidim_image_platform::MultiDimImage<uint8_t> image({16, 3, 2048, 2048});
        benchpress::escape(&image);
	}
})

BENCHMARK("reordering MultiDimImage 100x100x16x3 #multidimimage #reorder", [](benchpress::context* ctx)
{
    core::multidim_image_platform::MultiDimImage<uint8_t> image({100, 100, 16, 3});
    ctx->reset_timer();
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		image.reorderDims({0, 1, 2, 3});
        image.reorderDims({1, 0, 2, 3});
        image.reorderDims({1, 2, 0, 3});
        image.reorderDims({1, 2, 3, 0});
        image.reorderDims({3, 2, 1, 0});
	}
})

BENCHMARK("copy convert MultiDimImage 100x100x16x3, uint8_t->uint64_t #multidimimage #reorder #copyconvert", [](benchpress::context* ctx)
{
    core::multidim_image_platform::MultiDimImage<uint8_t> imFrom({100, 100, 16, 3});
    core::multidim_image_platform::MultiDimImage<uint64_t> imTo;
    ctx->reset_timer();
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		imTo.convertCopyFrom(imFrom);
	}
})

BENCHMARK("copy convert MultiDimImage 100x100x16x3, uint8_t->double #multidimimage #reorder #copyconvert", [](benchpress::context* ctx)
{
    core::multidim_image_platform::MultiDimImage<uint8_t> imFrom({100, 100, 16, 3});
    core::multidim_image_platform::MultiDimImage<double> imTo;
    ctx->reset_timer();
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		imTo.convertCopyFrom(imFrom);
	}
})

BENCHMARK("copy convert MultiDimImage 100x100x16x3, float->uint8_t #multidimimage #reorder #copyconvert", [](benchpress::context* ctx)
{
    core::multidim_image_platform::MultiDimImage<float> imFrom({100, 100, 16, 3});
    core::multidim_image_platform::MultiDimImage<uint8_t> imTo;
    ctx->reset_timer();
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		imTo.convertCopyFrom(imFrom);
	}
})

BENCHMARK("read<native type> small .ics file #multidimimage #read #ics", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::io_utils::IcsAdapter a;
        a.open("assets/128x128x32_c1_t1_float32.ics");
        auto im = a.read<float>(false);
        benchpress::escape(&im);
	}
})

BENCHMARK("read<native type> small .ics file, reorder #multidimimage #reorder #read #ics", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::io_utils::IcsAdapter a;
        a.open("assets/128x128x32_c1_t1_float32.ics");
        auto im = a.read<float>(true);
        benchpress::escape(&im);
	}
})

BENCHMARK("readScaledConvert<native type> small .ics file #multidimimage #ics #read #scaledconvert", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::io_utils::IcsAdapter a;
        a.open("assets/128x128x32_c1_t1_float32.ics");
        auto im = a.readScaledConvert<float>(false);
        benchpress::escape(&im);
	}
})

BENCHMARK("readScaledConvert<native type> small .ics file, reorder #multidimimage #ics #read #scaledconvert #reorder", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::io_utils::IcsAdapter a;
        a.open("assets/128x128x32_c1_t1_float32.ics");
        auto im = a.readScaledConvert<float>(true);
        benchpress::escape(&im);
	}
})

BENCHMARK("readScaledConvert<int32_t> small .ics file, reorder #multidimimage #ics #read #scaledconvert #reorder", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::io_utils::IcsAdapter a;
        a.open("assets/128x128x32_c1_t1_float32.ics");
        auto im = a.readScaledConvert<int32_t>(true);
        benchpress::escape(&im);
	}
})

BENCHMARK("readScaledConvert<float> large, 2x1024x1024x20 .ics file #multidimimage #ics #read #scaledconvert", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::io_utils::IcsAdapter a;
        a.open("assets/A15_1_a_DAPI_TH__vGluT1_20x.ics");
        auto im = a.readScaledConvert<float>(false);
        benchpress::escape(&im);
	}
})

BENCHMARK("readScaledConvert<float> large, 2048x2048x15x4 .ics file #multidimimage #ics #read #scaledconvert", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::io_utils::IcsAdapter a;
        a.open("assets/B23_4_b_DAPI_TH_vGluT1_bassoon_60x_cmle.ics");
        auto im = a.readScaledConvert<float>(false);
        benchpress::escape(&im);
	}
})

BENCHMARK("readScaledConvert<float> large, 2x1024x1024x20 .ics file, reorder #multidimimage #ics #read #scaledconvert #reorder", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::io_utils::IcsAdapter a;
        a.open("assets/A15_1_a_DAPI_TH__vGluT1_20x.ics");
        auto im = a.readScaledConvert<float>(true);
        benchpress::escape(&im);
	}
})

BENCHMARK("readScaledConvert<float> large, 2048x2048x15x4 .ics file, reorder #multidimimage #ics #read #scaledconvert #reorder", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::io_utils::IcsAdapter a;
        a.open("assets/B23_4_b_DAPI_TH_vGluT1_bassoon_60x_cmle.ics");
        auto im = a.readScaledConvert<float>(true);
        benchpress::escape(&im);
	}
})