#ifndef _core_compute_platform_ModuleContext_h_
#define _core_compute_platform_ModuleContext_h_

#include <string>

namespace core {
namespace compute_platform {

struct ModuleContext {
    std::string name;
    int runId = 0;
    bool hasNext = false;
    int statusIndicator = -1;
    int statusIndicatorMax = -1;
};

}}

#endif
