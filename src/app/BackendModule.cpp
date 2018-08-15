#include "BackendModule.h"

using namespace core::high_platform;

BackendModule::BackendModule(std::shared_ptr<PythonComputeModule> sourceModule, int uid)
    : m_source(sourceModule), m_uid(uid)
{
    // TODO: make status notifications
    Q_EMIT nameChanged();
    Q_EMIT statusChanged();
}

int BackendModule::uid() const
{
    return m_uid;
}

int BackendModule::parentUid() const
{
    return -1;
}

QString BackendModule::category() const
{
    return QString("module");
}

QString BackendModule::name() const
{
    return QString::fromStdString(m_source->name());
}

QString BackendModule::type() const
{
    return QString::fromStdString(m_source->moduleTypeName());
}

int BackendModule::status() const
{
    return m_status;
}

QVariant BackendModule::value() const
{
    return QVariant();
}

QVariant BackendModule::hints() const
{
    // TODO:
    return QVariant();
}

void BackendModule::setName(const QString& name)
{
    m_source->setName(name.toStdString());
    Q_EMIT nameChanged();
}

void BackendModule::setStatus(int status)
{
    m_status = status;
    Q_EMIT statusChanged();
}

bool BackendModule::setValue(QVariant value)
{
    return false;
}