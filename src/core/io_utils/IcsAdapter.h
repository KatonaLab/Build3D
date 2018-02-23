#ifndef _core_high_platform_IcsAdapter_h_
#define _core_high_platform_IcsAdapter_h_

#include <core/multidim_image_platform/MultiDimImage.hpp>

namespace md = core::multidim_image_platform;

namespace core {
namespace io_utils {

class FileAdapter {
public:
    // template <typename T>
    // virtual md::MultiDimImage<T> read(const std::string& filename);
    // template <typename T>
    // virtual write(const std::string& filename, md::MultiDimImage<T>& data);
};

class IcsAdapter : public FileAdapter {
public:
    // template <typename T>
    // virtual md::MultiDimImage<T> read(const std::string& filename);
    // template <typename T>
    // virtual write(const std::string& filename, md::MultiDimImage<T>& data);
};

}
}

#endif
