#include "BackendParameter.h"

#include <memory>

using namespace std;
using namespace core::compute_platform;

BackendParameter::BackendParameter(std::weak_ptr<InputPort> source, int portId, int parentUid)
    : m_source(source), m_portId(portId), m_parentUid(parentUid)
{
    // auto& platform = sourceModule.platform();
    // m_source = sourceModule.inputPort((size_t)portId);
    // // TODO:
    // // m_interfaceModule = make_shared<ParameterInterfaceModule>(platform, m_source);
    // m_interfaceModule->outputPort(0).lock()->bind(m_source);
}

int BackendParameter::uid() const
{
    return m_portId;
}

int BackendParameter::parentUid() const
{
    return m_parentUid;
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
    return QString("int");
}

int BackendParameter::status() const
{
    return 0;
}

QVariant BackendParameter::value() const
{
    return m_interfaceModule->data();
}