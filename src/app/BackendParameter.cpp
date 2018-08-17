#include "BackendParameter.h"

#include <string>
#include <tuple>
#include <vector>
#include <QDebug>

using namespace std;
using namespace core::compute_platform;

BackendParameter::BackendParameter(std::weak_ptr<InputPort> source,
    ComputePlatform& platform, int portId, int parentUid)
    : m_source(source), m_portId(portId), m_parentUid(parentUid)
{
    if (m_source.lock() == nullptr) {
        throw std::runtime_error("invalid source port");
    }

    using namespace details;

    m_hints = propertyMapToQVariantMap(m_source.lock()->properties());

    static vector<tuple<string, BuildParamFunction, string>> types = {
        make_tuple("uint8_t", buildParam<uint8_t>, "int"),
        make_tuple("uint16_t", buildParam<uint16_t>, "int"),
        make_tuple("uint32_t", buildParam<uint32_t>, "int"),
        make_tuple("uint64_t", buildParam<uint64_t>, "int"),
        make_tuple("int8_t", buildParam<int8_t>, "int"),
        make_tuple("int16_t", buildParam<int16_t>, "int"),
        make_tuple("int32_t", buildParam<int32_t>, "int"),
        make_tuple("int64_t", buildParam<int64_t>, "int"),
        make_tuple("float", buildParam<float>, "float"),
        make_tuple("double", buildParam<double>, "float"),
        make_tuple("bool", buildParam<bool>, "bool"),
        make_tuple("string", buildParam<QString>, "string")
    };

    const auto& t = m_source.lock()->traits();
    for (auto& tri: types) {
        if (t.hasTrait(get<0>(tri))) {
            m_interfaceModule = get<1>(tri)(platform);
            m_interfaceModule->outputPort(0).lock()->bind(m_source);
            m_type = QString::fromStdString(get<2>(tri));
            return;
        }
    }

    string errorMsg = "unknown parameter type for port '"
        + m_source.lock()->name() + "' (info:["
        + portTypeTraitsToString(m_source.lock()->traits()) + "])";
    throw std::runtime_error(errorMsg);
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
    return m_type;
}

int BackendParameter::status() const
{
    return 0;
}

QVariant BackendParameter::value() const
{
    return m_interfaceModule->data();
}

QVariant BackendParameter::hints() const
{
    return m_hints;
}

void BackendParameter::setName(const QString& name)
{
    // TODO: check for nullptr
    m_source.lock()->setName(name.toStdString());
    Q_EMIT nameChanged();
}

void BackendParameter::setStatus(int status)
{}

bool BackendParameter::setValue(QVariant value)
{
    bool changed = m_interfaceModule->setData(value);
    if (changed) {
        Q_EMIT valueChanged();
    }
    return changed;
}
