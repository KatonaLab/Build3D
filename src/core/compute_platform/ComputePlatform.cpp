#include "ComputePlatform.h"

using namespace core::compute_platform;
using namespace std;

void ComputePlatform::addModule(ComputeModule* module)
{
    m_modules.push_back(module);
}

size_t ComputePlatform::size() const
{
    return m_modules.size();
}

void ComputePlatform::run()
{
    // TODO:
}