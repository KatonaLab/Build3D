template <typename T>
TypedImageOutputHelperModule(ComputePlatform& parent)
    : ImageOutputInterfaceModule(parent, m_inputs, m_outputs),
    m_inputs(*this), m_outputs(*this)
{}

template <typename T>
void TypedImageOutputHelperModule::execute() override
{
    if (auto imPtr = m_inputs.template input<0>()->inputPtr().lock()) {
        m_result = std::make_shared<core::multidim_image_platform::MultiDimImage<float>>();
        m_result->convertCopyFrom(*imPtr);
    } else {
        m_result.reset();
    }
}

TypedImageOutputHelperModule<float>::TypedImageOutputHelperModule(ComputePlatform& parent)
    : ImageOutputInterfaceModule(parent, m_inputs, m_outputs),
    m_inputs(*this), m_outputs(*this)
{}

void TypedImageOutputHelperModule<float>::execute() override
{
    m_result = m_inputs.input<0>()->inputPtr().lock();
}
