#include "IcsAdapter.h"

using namespace core;
using namespace core::io_utils;

bool IcsAdapter::open(const std::string& filename)
{
    // TODO:
    return false;
}

std::size_t IcsAdapter::dataType() const
{
    // TODO:
    return typeid(this).hash_code();
}

bool IcsAdapter::valid() const
{
    // TODO:
    return false;
}

void IcsAdapter::close()
{
    // TODO:
}