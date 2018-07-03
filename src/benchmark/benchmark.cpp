#define BENCHPRESS_CONFIG_MAIN
#include <benchpress.hpp>

BENCHMARK("example", [](benchpress::context* ctx) {
	for (size_t i = 0; i < ctx->num_iterations(); ++i) {
		std::cout << "hello" << std::endl;
	}
})