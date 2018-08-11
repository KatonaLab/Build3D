#include "BackendOutput.h"

using namespace core::compute_platform;

BackendOutput::BackendOutput(ComputeModule& sourceModule, int portId)
    : m_portId(portId)
{
    auto& platform = sourceModule.platform();
    m_source = sourceModule.outputPort((size_t)portId);
    // TODO:
    // m_interfaceModule = make_shared<OutputInterfaceModule>(platform, m_source);
    m_source.lock()->bind(m_interfaceModule->inputPort(0));
}

int BackendOutput::uid() const
{
    return m_portId;
}
QString BackendOutput::category() const
{
    return QString("output");
}
QString BackendOutput::name() const
{
    return QString::fromStdString(m_source.lock()->name());
}
QString BackendOutput::type() const
{
    // TODO:
    return QString();
}
int BackendOutput::status() const
{
    return 0;
}
QVariant BackendOutput::value() const
{
    // TODO:
    // return m_interfaceModule->data();
    return QVariant();
}