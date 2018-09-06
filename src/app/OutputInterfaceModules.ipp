template <typename T>
TypedImageOutputInterfaceModule<T>::TypedImageOutputInterfaceModule(ComputePlatform& parent)
    : ImageOutputInterfaceModule(parent, m_inputs, m_outputs),
    m_inputs(*this), m_outputs(*this)
{}

template <typename T>
void TypedImageOutputInterfaceModule<T>::execute(ModuleContext&)
{
    if (auto imPtr = m_inputs.template input<0>()->inputPtr().lock()) {
        m_result = std::make_shared<core::multidim_image_platform::MultiDimImage<float>>();
        m_result->convertCopyFrom(*imPtr);
        if (m_onExecuteHandler) {
            m_onExecuteHandler();
        }
    } else {
        m_result.reset();
    }
}
