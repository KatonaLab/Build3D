#include "OutputInterfaceModules.hpp"

using namespace core::multidim_image_platform;

ImageOutputInterfaceModule::ImageOutputInterfaceModule(ComputePlatform& parent,
    InputPortCollectionBase& inputs, OutputPortCollectionBase& outputs,
    const std::string& name)
    : ComputeModule(parent, inputs, outputs, name)
{}

std::shared_ptr<MultiDimImage<float>> ImageOutputInterfaceModule::getImage()
{
    return m_result;
}

TypedImageOutputInterfaceModule<float>::TypedImageOutputInterfaceModule(ComputePlatform& parent)
    : ImageOutputInterfaceModule(parent, m_inputs, m_outputs),
    m_inputs(*this), m_outputs(*this)
{}

void TypedImageOutputInterfaceModule<float>::execute()
{
    m_result = m_inputs.input<0>()->inputPtr().lock();
    if (m_onExecuteHandler) {
        m_onExecuteHandler();
    }
}
