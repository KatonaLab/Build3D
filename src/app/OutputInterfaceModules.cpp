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
