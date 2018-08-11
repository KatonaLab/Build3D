#include "BackendParameter.h"

#include <memory>

using namespace std;
using namespace core::compute_platform;

BackendParameter::BackendParameter(ComputeModule& sourceModule, int portId)
    : m_portId(portId)
{
    auto& platform = sourceModule.platform();
    m_source = sourceModule.inputPort((size_t)portId);
    // TODO:
    // m_interfaceModule = make_shared<ParameterInterfaceModule>(platform, m_source);
    m_interfaceModule->outputPort(0).lock()->bind(m_source);
}

int BackendParameter::uid() const
{
    return m_portId;
}

QString BackendParameter::category() const
{
    return QString("parameter");
}

QString BackendParameter::name() const
{
    return QString::fromStdString(m_source.lock()->name());
}

QString BackendParameter::type() const
{
    // TODO:
    return QString();
}

int BackendParameter::status() const
{
    return 0;
}

QVariant BackendParameter::value() const
{
    return m_interfaceModule->data();
}