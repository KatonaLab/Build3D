#include "ParameterInterfaceModules.hpp"

ParameterInterfaceModule::ParameterInterfaceModule(ComputePlatform& parent,
    const std::string& name, OutputPortCollectionBase& outputs)
    : ComputeModule(parent, m_inputs, outputs, name), m_inputs(*this)
{}

TypedParameterInterfaceModule<QString>::TypedParameterInterfaceModule(core::compute_platform::ComputePlatform& parent, QString initialValue)
    :
    ParameterInterfaceModule(parent, "ParameterInterface-QString", m_outputs),
    m_outputs(*this),
    m_data(std::make_shared<std::string>(initialValue.toStdString()))
{}

bool TypedParameterInterfaceModule<QString>::setData(QVariant var)
{
    if (var.canConvert<QString>()) {
        *m_data = var.value<QString>().toStdString();
        return true;
    } else {
        throw std::runtime_error("can not convert from "
            + std::string(var.typeName()) + " to QString in parameter input " + name());
    }
    return false;
}

QVariant TypedParameterInterfaceModule<QString>::data()
{
    // TODO:
    return QVariant();
}

void TypedParameterInterfaceModule<QString>::execute()
{
    m_outputs.template output<0>()->forwardFromSharedPtr(m_data);
}
