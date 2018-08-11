template <typename T>
TypedParameterInterfaceModule<T>::TypedParameterInterfaceModule(ComputePlatform& parent, T initialValue)
    :
    ParameterInterfaceModule(parent, "ParameterInterface-" + std::string(typeid(T).name()), m_outputs),
    m_outputs(*this),
    m_data(std::make_shared<T>(initialValue))
{}

template <typename T>
bool TypedParameterInterfaceModule<T>::setData(QVariant var)
{
    if (var.canConvert<T>()) {
        *m_data = var.value<T>();
        return true;
    } else {
        throw std::runtime_error("can not convert from "
            + std::string(var.typeName()) + " to " + typeid(T).name()
            + " in parameter input " + name());
    }
    return false;
}

template <typename T>
QVariant TypedParameterInterfaceModule<T>::data()
{
    // TODO:
    return QVariant();
}

template <typename T>
void TypedParameterInterfaceModule<T>::execute()
{
    m_outputs.template output<0>()->forwardFromSharedPtr(m_data);
}
