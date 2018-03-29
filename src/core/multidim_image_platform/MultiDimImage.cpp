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

void Meta::clear()
{
    m_items.clear();
}

std::size_t detail::flatCoordinate(
    const std::vector<std::size_t>& coords,
    const std::vector<std::size_t>& dims)
{
    if (coords.empty()) {
        return 0;
    }
    auto itD = dims.begin();
    auto itC = coords.begin();
    std::size_t x = 0;
    std::size_t dimMult = 1;

    for (std::size_t i = 0; i < coords.size(); ++i, ++itD, ++itC) {
        x += dimMult * (*itC);
        dimMult *= (*itD);
    }
    return x;
}

bool detail::stepCoords(std::vector<std::size_t>& coords,
    const std::vector<std::size_t>& limits)
{
    // increments the coordinates with one step in 'coords'
    // respecting the given 'limits'
    // e.g.:
    //       in:  coords={2, 77}, limits={5, 80}
    //       out: coords={2, 78}, return false
    //
    //       in:  coords={2, 79}, limits={5, 80}
    //       out: coords={3, 0}, return false
    //
    //       in:  coords={2, 79, 49}, limits={5, 80, 50}
    //       out: coords={3, 0, 0}, return false
    //
    //       in:  coords={4, 79, 49}, limits={5, 80, 50}
    //       out: coords={5, 0, 0}, return true

    std::size_t carry = 1;
    for (int i = (int)coords.size() - 1; i >= 0; --i) {
        coords[i] += carry;
        if (coords[i] < limits[i]) {
            carry = 0;
            break;
        } else {
            coords[i] = 0;
            carry = 1;
        }
    }
    return carry;
}