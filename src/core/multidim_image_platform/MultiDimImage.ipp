
template <typename T>
void Meta::add(const std::string& tag, T value)
{
    // TODO
}

template <typename T>
T Meta::get(const std::string& tag)
{
    // TODO
    return T();
}

template <typename T>
bool Meta::has(const std::string& tag)
{
    // TODO
}

template <typename T>
void Meta::remove(const std::string& tag)
{
    // TODO
}

// =======================================

template <typename T>
bool MultiDimImage<T>::View::empty() const
{

}

template <typename T>
size_t MultiDimImage<T>::View::size() const
{

}

template <typename T>
size_t MultiDimImage<T>::View::dims() const
{

}

template <typename T>
size_t MultiDimImage<T>::View::dim(size_t d) const
{

}

template <typename T>
size_t MultiDimImage<T>::View::byteSize() const
{

}

template <typename T>
bool MultiDimImage<T>::View::valid() const
{

}

template <typename T>
T& MultiDimImage<T>::View::at(std::initializer_list<size_t> coords)
{

}

template <typename T>
MultiDimImage<T>* MultiDimImage<T>::View::parent()
{

}

// =======================================

template <typename T>
MultiDimImage<T>::MultiDimImage(std::initializer_list<size_t> dims)
{

}

template <typename T>
MultiDimImage<T>::MultiDimImage(const MultiDimImage<T>& other)
{

}

template <typename T>
MultiDimImage<T>& MultiDimImage<T>::operator=(const MultiDimImage<T>& other)
{

}

template <typename T>
template <typename U>
MultiDimImage<T>& MultiDimImage<T>::operator=(const MultiDimImage<U>& other)
{
    // TODO: indicate that it can not be done, static
}

template <typename T>
template <typename U>
void MultiDimImage<T>::convertCopy(const MultiDimImage<U>& other)
{

}

template <typename T>
bool MultiDimImage<T>::empty() const
{

}

template <typename T>
size_t MultiDimImage<T>::size() const
{

}

template <typename T>
size_t MultiDimImage<T>::dims() const
{

}

template <typename T>
size_t MultiDimImage<T>::dim(size_t d) const
{

}

template <typename T>
size_t MultiDimImage<T>::byteSize() const
{

}

template <typename T>
Type MultiDimImage<T>::type() const
{

}

template <typename T>
void MultiDimImage<T>::clear()
{

}

template <typename T>
T& MultiDimImage<T>::at(std::initializer_list<size_t> coords)
{

}

template <typename T>
typename MultiDimImage<T>::View MultiDimImage<T>::plane(std::initializer_list<size_t> coords)
{

}

template <typename T>
typename MultiDimImage<T>::View MultiDimImage<T>::volume(std::initializer_list<size_t> coords)
{

}

template <typename T>
void MultiDimImage<T>::reorderDims(std::initializer_list<size_t> dims)
{

}

template <typename T>
MultiDimImage<T>::~MultiDimImage()
{

}
