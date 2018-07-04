#define BENCHPRESS_CONFIG_MAIN
#include <benchpress.hpp>

#include <core/multidim_image_platform/MultiDimImage.hpp>

BENCHMARK("create MultiDimImage 100x100x16x3", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::multidim_image_platform::MultiDimImage<uint8_t> image({100, 100, 16, 3});
        benchpress::escape(&image);
	}
})

BENCHMARK("create MultiDimImage 2048x2048x16x3", [](benchpress::context* ctx)
{
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		core::multidim_image_platform::MultiDimImage<uint8_t> image({2048, 2048, 16, 3});
        benchpress::escape(&image);
	}
})

BENCHMARK("reordering MultiDimImage 100x100x16x3", [](benchpress::context* ctx)
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

BENCHMARK("copy convert MultiDimImage 100x100x16x3, uint8_t->uint64_t", [](benchpress::context* ctx)
{
    core::multidim_image_platform::MultiDimImage<uint8_t> imFrom({100, 100, 16, 3});
    core::multidim_image_platform::MultiDimImage<uint64_t> imTo;
    ctx->reset_timer();
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		imTo.convertCopyFrom(imFrom);
	}
})

BENCHMARK("copy convert MultiDimImage 100x100x16x3, uint8_t->double", [](benchpress::context* ctx)
{
    core::multidim_image_platform::MultiDimImage<uint8_t> imFrom({100, 100, 16, 3});
    core::multidim_image_platform::MultiDimImage<double> imTo;
    ctx->reset_timer();
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		imTo.convertCopyFrom(imFrom);
	}
})

BENCHMARK("copy convert MultiDimImage 100x100x16x3, float->uint8_t", [](benchpress::context* ctx)
{
    core::multidim_image_platform::MultiDimImage<float> imFrom({100, 100, 16, 3});
    core::multidim_image_platform::MultiDimImage<uint8_t> imTo;
    ctx->reset_timer();
    for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		imTo.convertCopyFrom(imFrom);
	}
})
