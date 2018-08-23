#include "BackendModule.h"

#include <QDebug>

using namespace core::compute_platform;

BackendModule::BackendModule(std::shared_ptr<ComputeModule> sourceModule, int uid)
    : m_source(sourceModule), m_uid(uid)
{}

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
    // TODO: check for nullptr
    return QString::fromStdString(m_source->name());
}

QString BackendModule::type() const
{
    // TODO: check for nullptr
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
    auto p = m_source;
    if (p && p->name() != name.toStdString()) {
        p->setName(name.toStdString());
        Q_EMIT nameChanged();
    }
}

void BackendModule::setStatus(int status)
{
    if (m_status != status) {
        m_status = status;
        Q_EMIT statusChanged();
    }
}

bool BackendModule::setValue(QVariant value)
{
    return false;
}

std::shared_ptr<ComputeModule> BackendModule::source()
{
    return m_source;
}
