#include "BackendInput.h"

using namespace core::compute_platform;

BackendInput::BackendInput(std::weak_ptr<InputPort> source, int portId)
    : m_source(source), m_portId(portId)
{}

int BackendInput::uid() const
{
    return m_portId;
}

QString BackendInput::category() const
{
    return QString("input");
}

QString BackendInput::name() const
{
    return QString::fromStdString(m_source.lock()->name());
}

QString BackendInput::type() const
{
    // TODO:
    return QString();
}

int BackendInput::status() const
{
    return 0;
}

QVariant BackendInput::value() const
{
    return QVariant();
}
