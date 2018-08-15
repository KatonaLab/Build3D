#include "BackendOutput.h"

using namespace core::compute_platform;

BackendOutput::BackendOutput(std::weak_ptr<OutputPort> source, int portId, int parentUid)
    : m_source(source), m_portId(portId), m_parentUid(parentUid)
{
    // auto& platform = sourceModule.platform();
    // m_source = sourceModule.outputPort((size_t)portId);
    // // TODO:
    // // m_interfaceModule = make_shared<OutputInterfaceModule>(platform, m_source);
    // m_source.lock()->bind(m_interfaceModule->inputPort(0));
}

int BackendOutput::uid() const
{
    return m_portId;
}

int BackendOutput::parentUid() const
{
    return m_parentUid;
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

QVariant BackendOutput::hints() const
{
    // TODO:
    return QVariant();
}

void BackendOutput::setName(const QString& name)
{

}

void BackendOutput::setStatus(int status)
{

}

bool BackendOutput::setValue(QVariant value)
{

}