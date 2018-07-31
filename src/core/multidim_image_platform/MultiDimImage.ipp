
template <typename T>
MultiDimImage<T>::MultiDimImage(std::vector<std::size_t> dims)
    : m_dims(dims)
{
    initUtilsFromDim();

    std::vector<std::vector<T>> data(m_restSize, std::vector<T>(m_planeSize, T()));
    m_planes = std::move(data);
    m_type = GetType<T>();
}

template <typename T>
void MultiDimImage<T>::initUtilsFromDim()
{
    int i = 0;
    std::size_t prodPlane = 1, prodRest = 1;
    for (std::size_t x : m_dims) {
        if (i < 2) {
            m_planeDims.push_back(x);
            m_planeDimsProducts.push_back(prodPlane);
            prodPlane *= x;
        } else {
            m_restDims.push_back(x);
            m_restDimsProducts.push_back(prodRest);
            prodRest *= x;
        }
        i++;
    }
    if (m_planeDims.empty()) {
        m_planeSize = 0;
    } else {
        m_planeSize = prodPlane;
    }

    if (m_restDims.empty() && m_planeDims.empty()) {
        m_restSize = 0;
    } else {
        m_restSize = prodRest;
    }
}

template <typename T>
template <typename U>
void MultiDimImage<T>::transformCopy(const MultiDimImage<U>& other,
    std::function<T(const U&)> unary)
{
    MultiDimImage<T> newImage;
    newImage.m_dims = other.m_dims;
    newImage.initUtilsFromDim();

    newImage.m_planes.reserve(newImage.m_restSize);
    auto itEnd = other.m_planes.end();
    for (auto it = other.m_planes.begin(); it != itEnd; ++it) {
        newImage.m_planes.push_back(std::vector<T>());
        auto& plane = newImage.m_planes.back();
        plane.reserve(newImage.m_planeSize);
        std::transform(it->begin(), it->end(),
            std::back_inserter(plane), unary);
    }
    *this = std::move(newImage);
}

template <typename T>
template <typename U>
void MultiDimImage<T>::convertCopyFrom(const MultiDimImage<U>& other)
{
    std::function<T(const U&)> f = [](const U& x) -> T {
            return static_cast<T>(x);
        };
    transformCopy(other, f);
}

template <typename T>
template <typename U>
void MultiDimImage<T>::saturateCopyFrom(const MultiDimImage<U>& other,
    T minValue, T maxValue)
{
    std::function<T(const U&)> f = [minValue, maxValue](const U& x) -> T {
        return detail::SafeLessThanCompare<T, U>::less(maxValue, x) ?
            maxValue : (detail::SafeLessThanCompare<T, U>::less(minValue, x) ? x : minValue);
    };
    transformCopy(other, f);
}

template <typename T>
template <typename U>
void MultiDimImage<T>::scaledCopyFrom(const MultiDimImage<U>& other)
{
    std::function<T(const U&)> f = detail::TypeScaleHelper<U, T>::scale;
    transformCopy(other, f);
}

template <typename T>
bool MultiDimImage<T>::empty() const
{
    return m_dims.empty() || std::any_of(m_dims.begin(), m_dims.end(),
        [](std::size_t x) { return x == 0; });
}

template <typename T>
std::size_t MultiDimImage<T>::size() const
{
    if (m_dims.empty()) {
        return 0;
    } else {
        return m_planeSize * m_restSize;
    }
}

template <typename T>
std::size_t MultiDimImage<T>::dims() const
{
    return m_dims.size();
}

template <typename T>
std::size_t MultiDimImage<T>::dim(std::size_t d) const
{
    if (d >= m_dims.size()) {
        throw std::range_error("no such dimension");
    }
    return m_dims[d];
}

template <typename T>
std::vector<std::size_t> MultiDimImage<T>::dimList() const
{
    return m_dims;
}

template <typename T>
std::size_t MultiDimImage<T>::byteSize() const
{
    return size() * sizeof(T);
}

template <typename T>
Type MultiDimImage<T>::type() const
{
    return m_type;
}

template <typename T>
void MultiDimImage<T>::clear()
{
    MultiDimImage<T> empty;
    *this = std::move(empty);
}

template <typename T>
T& MultiDimImage<T>::at(std::vector<std::size_t> coords)
{
    if (coords.size() != m_dims.size()) {
        throw std::length_error("number of coordinates not equals with the number of dimensions");
    }

    auto itD = m_dims.begin();
    auto itC = coords.begin();
    for (std::size_t i = 0; i < m_dims.size(); ++i, ++itD, ++itC) {
        if (*itC < 0 || *itD <= *itC ) {
            throw std::out_of_range("out of MultiDimImage range");
        }
    }

    return unsafeAt(coords);
}

template <typename T>
std::pair<std::size_t, std::size_t> MultiDimImage<T>::planeCoordinatePair(const std::vector<std::size_t>& coords)
{
    std::size_t second = 0;
    if (coords.size() > 2) {
        second = std::inner_product(
            coords.begin() + 2,
            coords.end(),
            m_restDimsProducts.begin(), 0);
    }

    return std::make_pair(
        std::inner_product(
            coords.begin(),
            coords.begin() + std::min((std::size_t)2, coords.size()),
            m_planeDimsProducts.begin(), 0),
            second
    );
}

template <typename T>
T& MultiDimImage<T>::unsafeAt(std::vector<std::size_t> coords)
{
    auto c = planeCoordinatePair(coords);
    return m_planes[c.second][c.first];
}

template <typename T>
std::vector<std::vector<T>>& MultiDimImage<T>::unsafeData()
{
    return m_planes;
}

template <typename T>
const std::vector<std::vector<T>>& MultiDimImage<T>::unsafeData() const
{
    return m_planes;
}

template <typename T>
void MultiDimImage<T>::reorderDims(std::vector<std::size_t> dimOrder)
{
    if (dimOrder.size() != m_dims.size()) {
        throw std::invalid_argument("number of dimensions in the argument does not match with the number of data dimensions");
    }

    std::vector<std::size_t> identity(m_dims.size());
    std::iota(identity.begin(), identity.end(), 0);
    
    std::vector<std::size_t> dimOrderSorted(dimOrder);
    std::sort(dimOrderSorted.begin(), dimOrderSorted.end());

    if (!std::equal(dimOrderSorted.begin(), dimOrderSorted.end(), identity.begin())) {
        throw std::invalid_argument("bad parameters for dimension reordering");
    }
    
    if (std::equal(dimOrder.begin(), dimOrder.end(), identity.begin())) {
        // nothing to do
        return;
    }

    std::vector<std::size_t> newDims(m_dims.size(), 0);
    for (std::size_t j = 0; j < m_dims.size(); ++j) {
        newDims[j] = m_dims[dimOrder[j]];
    }

    std::size_t d = std::min((std::size_t)2, m_planeDims.size());

    std::vector<std::size_t> coord(newDims.size(), 0);
    MultiDimImage<T> newImage(newDims);
    for (auto& oldPlane : m_planes) {
        for (auto& oldValue : oldPlane) {
            auto coordPair = newImage.planeCoordinatePair(detail::reorderCoords(coord, dimOrder));
            newImage.m_planes[coordPair.second][coordPair.first] = oldValue;
            detail::stepCoords(coord.begin(), coord.begin() + d, m_planeDims.begin());
        }
        if (m_restSize) {
            detail::stepCoords(coord.begin() + 2, coord.end(), m_restDims.begin());
        }
    }

    *this = std::move(newImage);
}

template <typename T>
void MultiDimImage<T>::removeDims(std::vector<std::size_t> dims)
{
    using namespace std;

    sort(dims.begin(), dims.end());

    vector<size_t> newDims(m_dims);
    size_t k = 0;
    for (size_t d : dims) {
        if (d >= m_dims.size()) {
            throw std::range_error("no such dimension: " + std::to_string(d));
        }
        newDims.erase(newDims.begin() + (d - k++));
    }

    MultiDimImage<T> newImage(newDims);
    std::vector<std::size_t> newCoord(newDims.size(), 0);
    std::vector<std::size_t> oldCoord(m_dims.size(), 0);

    if (newImage.empty()) {
        *this = std::move(newImage);
        return;
    }

    auto skipDim = [&oldCoord, &dims]() {
        return any_of(dims.begin(), dims.end(), [&oldCoord](size_t x) {
            return (bool)oldCoord[x];
        });
    };

    for (auto& oldPlane : m_planes) {
        for (auto& oldValue : oldPlane) {
            if (!skipDim()) {
                auto coordPair = newImage.planeCoordinatePair(newCoord);
                newImage.m_planes[coordPair.second][coordPair.first] = oldValue;
                detail::stepCoords(newCoord.begin(), newCoord.end(), newDims.begin());
            }
            detail::stepCoords(oldCoord.begin(), oldCoord.end(), m_dims.begin());
        }
    }

    *this = std::move(newImage);
}

template <typename T>
std::vector<MultiDimImage<T>> MultiDimImage<T>::splitDim(std::size_t dim)
{
    using namespace std;

    if (dim >= m_dims.size()) {
        throw std::range_error("no such dimension: " + std::to_string(dim));
    }

    vector<size_t> newDims(m_dims);
    newDims.erase(newDims.begin() + dim);
    vector<MultiDimImage<T>> result(m_dims[dim], MultiDimImage<T>(newDims));

    vector<size_t> oldCoord(m_dims.size(), 0);
    vector<vector<size_t>> newCoordsList(m_dims[dim], vector<size_t>(newDims.size(), 0));
    for (auto& oldPlane : m_planes) {
        for (auto& oldValue : oldPlane) {
            size_t x = oldCoord[dim];
            auto coordPair = result[x].planeCoordinatePair(newCoordsList[x]);
            result[x].m_planes[coordPair.second][coordPair.first] = oldValue;
            detail::stepCoords(oldCoord.begin(), oldCoord.end(), m_dims.begin());
            detail::stepCoords(newCoordsList[x].begin(), newCoordsList[x].end(), newDims.begin());
        }
    }

    return result;
}

template <typename T>
MultiDimImage<T>::~MultiDimImage()
{}

namespace detail {

    template <typename It1, typename It2>
    void stepCoords(It1 begin, It1 end, It2 limitsBegin)
    {
        bool c = true;
        auto it = begin;
        auto limitsIt = limitsBegin;
        while (c && it != end) {
            if (++(*it) >= *limitsIt) {
                *it = 0;
            } else {
                c = false;
            }
            ++it;
            ++limitsIt;
        }
    }

    template <typename X, typename Y>
    struct TypeScaleHelper<X, Y, true, false> {
        static constexpr double typeScaleParamA()
        {
            return 1.0 /
                ((double)std::numeric_limits<X>::max() - (double)std::numeric_limits<X>::min());
        }
        static constexpr double typeScaleParamB()
        {
            return 0.5 -0.5 * TypeScaleHelper<X, Y>::typeScaleParamA()
                * ((double)std::numeric_limits<X>::min() + (double)std::numeric_limits<X>::max());
        }
        static Y scale(X x)
        {
            const double a = TypeScaleHelper<X, Y>::typeScaleParamA();
            const double b = TypeScaleHelper<X, Y>::typeScaleParamB();
            return static_cast<Y>(x * a + b);
        }
    };

    template <typename X, typename Y>
    struct TypeScaleHelper<X, Y, false, true> {
        static constexpr double typeScaleParamA()
        {
            return ((double)std::numeric_limits<Y>::max() - (double)std::numeric_limits<Y>::min());
        }
        static constexpr double typeScaleParamB()
        {
            return 0.5 * ((double)std::numeric_limits<Y>::min() + (double)std::numeric_limits<Y>::max()
                - TypeScaleHelper<X, Y>::typeScaleParamA());
        }
        [[deprecated("MultiDimImage - scaling a floating point type to an integral type can be numerically unstable, avoid this type of scaling")]]
        static Y scale(X x)
        {
            // NOTE: in cases when sizeof(X) is comparable to sizeof(Y) the division and the
            // multiplication precision will not be enough, it can lead false results
            // NOTE: using 'long double' for all calculations might solve this issue but
            // unfortunately msvc don't support higher precision than double, since 'long double'
            // is just a typedef to double in that compiler.
            const double a = TypeScaleHelper<X, Y>::typeScaleParamA();
            const double b = TypeScaleHelper<X, Y>::typeScaleParamB();
            return static_cast<Y>(x * a + b);
        }
    };

    template <typename X, typename Y>
    struct TypeScaleHelper<X, Y, false, false> {
        static constexpr double typeScaleParamA()
        {
            return 1.0;
        }
        static constexpr double typeScaleParamB()
        {
            return 0.0;
        }
        static Y scale(X x)
        {
            return static_cast<Y>(x);
        }
    };

    template <typename X, typename Y>
    Y typeScale(const X& x)
    {
        return TypeScaleHelper<X, Y>::scale(x);
    }
}