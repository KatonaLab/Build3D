#include "ParameterInterfaceModules.hpp"

using namespace core::compute_platform;
using namespace std;

ParameterInterfaceModule::ParameterInterfaceModule(ComputePlatform& parent,
    const std::string& name, OutputPortCollectionBase& outputs)
    : ComputeModule(parent, m_inputs, outputs, name), m_inputs(*this)
{}

// ---

TypedParameterInterfaceModule<EnumPair>::TypedParameterInterfaceModule(ComputePlatform& parent, EnumPair initialValue)
    :
    ParameterInterfaceModule(parent, "ParameterInterface-EnumPair", m_outputs),
    m_outputs(*this),
    m_data(std::make_shared<EnumPair>(initialValue))
{}

bool TypedParameterInterfaceModule<EnumPair>::setData(QVariant var)
{
    QVariantMap map = var.toMap();
    if (map.contains("first") && map["first"].canConvert<int>() &&
        map.contains("second") && map["second"].canConvert<int>()) {
        auto p = make_pair(map["first"].toInt(), map["second"].toInt());
        if (*m_data != p) {
            *m_data = p;
            return true;
        }
    } else {
        throw std::runtime_error("can not convert from "
            + std::string(var.typeName()) + " to EnumPair in parameter input " + name());
    }
    return false;
}

QVariant TypedParameterInterfaceModule<EnumPair>::data()
{
    QVariantMap map;
    map["first"] = m_data->first;
    map["second"] = m_data->second;
    return map;
}

void TypedParameterInterfaceModule<EnumPair>::execute(ModuleContext&)
{
    m_outputs.template output<0>()->forwardFromSharedPtr(m_data);
}

// ---

TypedParameterInterfaceModule<QUrl>::TypedParameterInterfaceModule(ComputePlatform& parent, QUrl initialValue)
    :
    ParameterInterfaceModule(parent, "ParameterInterface-QUrl", m_outputs),
    m_outputs(*this),
    m_data(std::make_shared<Url>(initialValue.toLocalFile().toStdString()))
{}

bool TypedParameterInterfaceModule<QUrl>::setData(QVariant var)
{
    if (var.canConvert<QUrl>()) {
        string s = var.value<QUrl>().toLocalFile().toStdString();
        if (m_data->path != s) {
            m_data->path = s;
            return true;
        }
    } else {
        throw std::runtime_error("can not convert from "
            + std::string(var.typeName()) + " to QUrl in parameter input " + name());
    }
    return false;
}

QVariant TypedParameterInterfaceModule<QUrl>::data()
{
    return QUrl::fromLocalFile(QString::fromStdString(m_data->path));
}

void TypedParameterInterfaceModule<QUrl>::execute(ModuleContext&)
{
    m_outputs.template output<0>()->forwardFromSharedPtr(m_data);
}

// ---

TypedParameterInterfaceModule<QString>::TypedParameterInterfaceModule(core::compute_platform::ComputePlatform& parent, QString initialValue)
    :
    ParameterInterfaceModule(parent, "ParameterInterface-QString", m_outputs),
    m_outputs(*this),
    m_data(std::make_shared<std::string>(initialValue.toStdString()))
{}

bool TypedParameterInterfaceModule<QString>::setData(QVariant var)
{
    if (var.canConvert<QString>()) {
        string s = var.value<QString>().toStdString();
        if (*m_data != s) {
            *m_data = s;
            return true;
        }
    } else {
        throw std::runtime_error("can not convert from "
            + std::string(var.typeName()) + " to QString in parameter input " + name());
    }
    return false;
}

QVariant TypedParameterInterfaceModule<QString>::data()
{
    // TODO: check nullptr
    return QString::fromStdString(*m_data);
}

void TypedParameterInterfaceModule<QString>::execute(ModuleContext&)
{
    m_outputs.template output<0>()->forwardFromSharedPtr(m_data);
}
