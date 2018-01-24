
template <typename T>
MultiDimImage<T>::View::View(MultiDimImage<T>* parent,
    std::vector<std::size_t> offsets,
    std::vector<std::size_t> dims)
    : m_parent(parent), m_offsets(offsets), m_dims(dims)
{
    if (offsets.size() != dims.size()) {
        throw std::invalid_argument("number of offsets differes form the number of dims");
    }

    std::copy_if(m_dims.begin(), m_dims.end(), 
        std::back_inserter(m_trueDims),
        [](std::size_t x) { return x > 1; });

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
        m_parent->unregisterView(this);
    }
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
    return subDimView(coords, 2);
}

template <typename T>
typename MultiDimImage<T>::View MultiDimImage<T>::volume(std::vector<std::size_t> coords)
{
    return subDimView(coords, 3);
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
    View view(this, offsets, dims);
    return view;
}

template <typename T>
void MultiDimImage<T>::reorderDims(std::vector<std::size_t> dims)
{

}

template <typename T>
MultiDimImage<T>::~MultiDimImage()
{
    for (auto v : m_views) {
        unregisterView(v);
    }
}

template <typename T>
void MultiDimImage<T>::registerView(View* view)
{
    m_views.push_back(view);
    view->m_parent = this;
}

template <typename T>
void MultiDimImage<T>::unregisterView(View* view)
{
    auto it = std::find(m_views.begin(), m_views.end(), view);
    if (it != m_views.end()) {
        view->m_parent = nullptr;
        m_views.erase(it);
    }
}