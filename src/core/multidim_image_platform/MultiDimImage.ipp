
template <typename T>
MultiDimImage<T>::View::View(MultiDimImage<T>* parent,
    std::vector<std::size_t> offsets,
    std::vector<std::size_t> dims)
    : m_parent(parent), m_offsets(offsets), m_dims(dims)
{
    if (offsets.size() != dims.size()) {
        throw std::invalid_argument("number of offsets differes form the number of dims");
    }

    m_numTrueDims = std::count_if(m_dims.begin(), m_dims.end(),
        [](std::size_t x) { return x > 1;});

    // TODO: check that the region is valid
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
    return m_dims.size();
}

template <typename T>
std::size_t MultiDimImage<T>::View::dim(std::size_t d) const
{
    if (d >= m_dims.size()) {
        throw std::range_error("no such dimension");
    }
    return m_dims[d];
}

template <typename T>
std::size_t MultiDimImage<T>::View::byteSize() const
{
    return size() * sizeof(T);
}

template <typename T>
bool MultiDimImage<T>::View::valid() const
{
    return m_valid;
}

template <typename T>
T& MultiDimImage<T>::View::at(std::vector<std::size_t> coords)
{
    if (coords.size() != m_numTrueDims) {
        throw std::length_error("number of coordinates not equals with the number of true dimensions");
    }
    std::vector<std::size_t> newCoords;
    return m_parent->at(newCoords);
}

template <typename T>
MultiDimImage<T>* MultiDimImage<T>::View::parent()
{
    return m_parent;
}

// =======================================

template <typename T>
MultiDimImage<T>::MultiDimImage(std::vector<std::size_t> dims)
    : m_dims(dims)
{
    int i = 0;
    for (std::size_t x : dims) {
        if (i < 2) {
            m_planeDims.push_back(x);
        } else {
            m_restDims.push_back(x);
        }
        i++;
    }
    std::size_t planeSize = std::accumulate(m_planeDims.begin(), m_planeDims.end(), 1, std::multiplies<std::size_t>());
    std::size_t restSize = std::accumulate(m_restDims.begin(), m_restDims.end(), 1, std::multiplies<std::size_t>());
    std::vector<std::vector<T>> data(restSize, std::vector<T>(planeSize, T()));
    std::swap(data, m_planes);
    m_type = GetType<T>();
}

template <typename T>
template <typename U>
void MultiDimImage<T>::convertCopy(const MultiDimImage<U>& other)
{

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

}

template <typename T>
typename MultiDimImage<T>::View MultiDimImage<T>::volume(std::vector<std::size_t> coords)
{

}

template <typename T>
void MultiDimImage<T>::reorderDims(std::vector<std::size_t> dims)
{

}

template <typename T>
MultiDimImage<T>::~MultiDimImage()
{

}
