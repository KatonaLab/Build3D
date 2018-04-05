
template <typename T>
MultiDimImage<T>::View::View(MultiDimImage<T>* parent,
    std::vector<std::size_t> offsets,
    std::vector<std::size_t> dims)
    : m_offsets(offsets), m_dims(dims), m_parent(parent)
{
    if (parent == nullptr) {
        throw std::invalid_argument("parent of the view can not be null");
    }

    if (offsets.size() != dims.size()) {
        throw std::invalid_argument("number of offsets differes form the number of dims");
    }

    std::copy_if(m_dims.begin(), m_dims.end(), 
        std::back_inserter(m_trueDims),
        [](std::size_t x) { return x > 1; });

    // TODO: check that the region is valid

    m_parent->m_viewRegistry.add(this);
}

template <typename T>
MultiDimImage<T>::View::View(const MultiDimImage<T>::View& other) : m_parent(nullptr)
{
    *this = other;
}

template <typename T>
typename MultiDimImage<T>::View& MultiDimImage<T>::View::operator=(const MultiDimImage<T>::View& other)
{
    m_offsets = other.m_offsets;
    m_dims = other.m_dims;
    m_trueDims = other.m_trueDims;
    if (m_parent) {
        m_parent->m_viewRegistry.remove(this);
        m_parent = other.m_parent;
        m_parent->m_viewRegistry.add(this);
    }
    return *this;
}

template <typename T>
bool MultiDimImage<T>::View::empty() const
{
    return m_dims.empty() || std::any_of(m_dims.begin(), m_dims.end(),
        [](std::size_t x) { return x == 0; });
}

template <typename T>
std::size_t MultiDimImage<T>::View::size() const
{
    if (m_dims.empty()) {
        return 0;
    } else {
        return std::accumulate(m_dims.begin(), m_dims.end(), 1, std::multiplies<std::size_t>());
    }
}

template <typename T>
std::size_t MultiDimImage<T>::View::dims() const
{
    return m_trueDims.size();
}

template <typename T>
std::size_t MultiDimImage<T>::View::dim(std::size_t d) const
{
    if (d >= m_trueDims.size()) {
        throw std::range_error("no such dimension");
    }
    
    return m_trueDims[d];
}

template <typename T>
std::size_t MultiDimImage<T>::View::byteSize() const
{
    return size() * sizeof(T);
}

template <typename T>
bool MultiDimImage<T>::View::valid() const
{
    return m_parent != nullptr;
}

template <typename T>
T& MultiDimImage<T>::View::at(std::vector<std::size_t> coords)
{
    if (coords.size() != m_trueDims.size()) {
        throw std::length_error("number of coordinates not equals with the number of true dimensions");
    }
    std::vector<std::size_t> newCoords(m_offsets);
    for (size_t i = 0, j = 0; i < newCoords.size(); ++i) {
        if (m_dims[i] > 1) {
            newCoords[i] += coords[j++];
        }
    }
    return m_parent->at(newCoords);
}

template <typename T>
MultiDimImage<T>* MultiDimImage<T>::View::parent()
{
    return m_parent;
}

template <typename T>
MultiDimImage<T>::View::~View()
{
    if (m_parent) {
        m_parent->m_viewRegistry.remove(this);
    }
}

template <typename T>
MultiDimImage<T>::MultiDimImage(std::vector<std::size_t> dims)
    : m_dims(dims), m_viewRegistry(this)
{
    initUtilsFromDim();

    std::vector<std::vector<T>> data(m_restSize, std::vector<T>(m_planeSize, T()));
    m_planes.swap(data);
    m_type = GetType<T>();
}

template <typename T>
void MultiDimImage<T>::initUtilsFromDim()
{
    int i = 0;
    for (std::size_t x : m_dims) {
        if (i < 2) {
            m_planeDims.push_back(x);
        } else {
            m_restDims.push_back(x);
        }
        i++;
    }
    if (m_planeDims.empty()) {
        m_planeSize = 0;
    } else {
        m_planeSize = std::accumulate(
            m_planeDims.begin(), m_planeDims.end(),
            1, std::multiplies<std::size_t>());
    }

    if (m_restDims.empty() && m_planeDims.empty()) {
        m_restSize = 0;
    } else {
        m_restSize = std::accumulate(
            m_restDims.begin(), m_restDims.end(),
            1, std::multiplies<std::size_t>());
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
    std::swap(*this, newImage);
    
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
        return std::accumulate(m_dims.begin(), m_dims.end(), 1, std::multiplies<std::size_t>());
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
    m_viewRegistry.clear();
    MultiDimImage<T> empty;
    std::swap(empty, *this);
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
T& MultiDimImage<T>::unsafeAt(std::vector<std::size_t> coords)
{
    std::vector<std::size_t> planeDimCoords;
    std::vector<std::size_t> restDimCoords;
    int i = 0;
    for (std::size_t x : coords) {
        if (i < 2) {
            planeDimCoords.push_back(x);
        } else {
            restDimCoords.push_back(x);
        }
        i++;
    }

    return m_planes[detail::flatCoordinate(restDimCoords, m_restDims)]
        [detail::flatCoordinate(planeDimCoords, m_planeDims)];
}

template <typename T>
typename MultiDimImage<T>::View MultiDimImage<T>::plane(std::vector<std::size_t> coords)
{
    return subDimView(coords, 2);
}

template <typename T>
typename MultiDimImage<T>::View MultiDimImage<T>::volume(std::vector<std::size_t> coords)
{
    return subDimView(coords, 3);
}

template <typename T>
std::vector<std::vector<T>>& MultiDimImage<T>::unsafeData()
{
    return m_planes;
}

template <typename T>
typename MultiDimImage<T>::View MultiDimImage<T>::subDimView(std::vector<std::size_t> coords, std::size_t firstNDims)
{
    if (m_dims.size() < firstNDims + 1) {
        throw std::invalid_argument("can not slice with the same dimensions as the original");
    }

    if (coords.size() != m_dims.size() - firstNDims) {
        throw std::invalid_argument("invalid number of coordinates");
    }

    std::vector<std::size_t> dims(m_dims);
    std::vector<std::size_t> offsets(m_dims.size());
    for (size_t i = 0; i < dims.size(); ++i) {
        if (i < firstNDims) {
            offsets[i] = 0;
        } else {
            dims[i] = 1;
            offsets[i] = coords[i - firstNDims];
        }
    }
    return View(this, offsets, dims);
}

template <typename T>
void MultiDimImage<T>::reorderDims(std::vector<std::size_t> dimOrder)
{
    // TODO: find a faster solution

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

    MultiDimImage<T> newImage(newDims);
    std::size_t n = size();
    std::vector<std::size_t> coords(m_dims.size(), 0);
    for (std::size_t i = 0; i < n; ++i) {
        std::vector<std::size_t> newCoords(m_dims.size(), 0);

        for (std::size_t j = 0; j < m_dims.size(); ++j) {
            newCoords[j] = coords[dimOrder[j]];
        }

        newImage.unsafeAt(newCoords) = unsafeAt(coords);
        detail::stepCoords(coords, m_dims);
    }

    m_viewRegistry.clear();
    std::swap(newImage, *this);
}

template <typename T>
void MultiDimImage<T>::removeDims(std::vector<std::size_t> dims)
{
    // TODO: find a faster solution
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

    std::size_t n = newImage.size();
    std::vector<std::size_t> newCoords(newImage.m_dims.size(), 0);
    
    for (std::size_t i = 0; i < n; ++i) {
        std::vector<std::size_t> oldCoords(newCoords);

        for (size_t d : dims) {
            oldCoords.insert(oldCoords.begin() + d, 0);
        }

        newImage.unsafeAt(newCoords) = unsafeAt(oldCoords);
        detail::stepCoords(newCoords, newImage.m_dims);
    }

    m_viewRegistry.clear();
    std::swap(newImage, *this);
}

template <typename T>
std::vector<MultiDimImage<T>> MultiDimImage<T>::splitDim(std::size_t dim)
{
    // TODO: find a faster solution
    using namespace std;

    if (dim >= m_dims.size()) {
        throw std::range_error("no such dimension: " + std::to_string(dim));
    }

    vector<MultiDimImage<T>> result;
    vector<size_t> newDims(m_dims);
    newDims.erase(newDims.begin() + dim);

    for (size_t k = 0; k < this->dim(dim); ++k) {
        result.emplace_back(newDims);
        vector<size_t> newCoords(newDims.size(), 0);
        size_t n = result.back().size();
        for (size_t i = 0; i < n; ++i) {
            vector<size_t> oldCoords(newCoords);
            oldCoords.insert(oldCoords.begin() + dim, k);
            
            result.back().unsafeAt(newCoords) = unsafeAt(oldCoords);
            detail::stepCoords(newCoords, result.back().m_dims);
        }
    }

    return result;
}

template <typename T>
MultiDimImage<T>::~MultiDimImage()
{}

namespace detail {

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