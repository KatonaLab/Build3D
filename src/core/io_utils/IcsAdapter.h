#ifndef _core_high_platform_IcsAdapter_h_
#define _core_high_platform_IcsAdapter_h_

#include <core/multidim_image_platform/MultiDimImage.hpp>

#include <type_traits>

namespace md = core::multidim_image_platform;

namespace core {
namespace io_utils {

class IcsAdapter {
public:
    bool open(const std::string& filename);
    template <typename T> md::MultiDimImage<T> read();
    template <typename T> void write(const std::string& filename, const md::MultiDimImage<T>& data);
    std::size_t dataType() const;
    bool valid() const;
    void close();
};

template <typename T>
md::MultiDimImage<T> IcsAdapter::read()
{
    // TODO:
    md::MultiDimImage<T> im;
    return im;
}

template <typename T>
void IcsAdapter::write(const std::string& filename, const md::MultiDimImage<T>& data)
{
    // TODO:
}

}
}

#endif
