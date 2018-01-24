#include "MultiDimImage.hpp"

using namespace core::multidim_image_platform;

void Meta::add(const std::string& tag, const std::string& value)
{
    m_items[tag] = value;
}

const std::string& Meta::get(const std::string& tag)
{
    if (has(tag)) {
        return m_items[tag];
    }
    throw std::runtime_error("no such tag int the meta list");
}

bool Meta::has(const std::string& tag)
{
    return (bool)m_items.count(tag);
}

void Meta::remove(const std::string& tag)
{
    if (has(tag)) {
        m_items.erase(tag);
    }
}

std::size_t core::multidim_image_platform::detail::flatCoordinate(
    const std::vector<std::size_t>& coords,
    const std::vector<std::size_t>& dims)
{
    if (coords.empty()) {
        return 0;
    }
    auto itD = dims.begin();
    auto itC = coords.begin();
    std::size_t x = (*itC);

    ++itC;
    for (std::size_t i = 1; i < coords.size(); ++i, ++itD, ++itC) {
        x = x * (*itD) + (*itC);
    }
    return x;
}