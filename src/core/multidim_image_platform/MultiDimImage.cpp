#include "MultiDimImage.hpp"

using namespace core::multidim_image_platform;

void Meta::add(const std::string& tag, const std::string& value)
{
    m_items[tag] = value;
}

std::string Meta::get(const std::string& tag)
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

std::vector<std::size_t> detail::reorderCoords(std::vector<std::size_t>& coords,
    const std::vector<std::size_t>& order)
{
    std::vector<std::size_t> newCoords(order.size(), 0);
    for (std::size_t i = 0; i < order.size(); ++i) {
        newCoords[i] = coords[order[i]];
    }
    return newCoords;
}

bool detail::stepCoords(std::vector<std::size_t>& coords,
    const std::vector<std::size_t>& limits)
{
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
