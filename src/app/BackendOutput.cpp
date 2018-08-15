#include "BackendOutput.h"
#include "VolumeTexture.h"

#include <tuple>
#include <string>

using namespace core::compute_platform;
using namespace std;

BackendOutput::BackendOutput(std::weak_ptr<OutputPort> source,
    ComputePlatform& platform, int portId, int parentUid)
    : m_source(source), m_portId(portId), m_parentUid(parentUid)
{
    if (m_source.lock() == nullptr) {
        throw std::runtime_error("invalid source port");
    }

    using namespace details;

    m_hints = propertyMapToQVariantMap(m_source.lock()->properties());

    static BuildOutputFunction empty;
    static vector<tuple<string, BuildOutputFunction, string>> types = {
        make_tuple("uint8_t", empty, "int"),
        make_tuple("uint16_t", empty, "int"),
        make_tuple("uint32_t", empty, "int"),
        make_tuple("uint64_t", empty, "int"),
        make_tuple("int8_t", empty, "int"),
        make_tuple("int16_t", empty, "int"),
        make_tuple("int32_t", empty, "int"),
        make_tuple("int64_t", empty, "int"),
        make_tuple("float", empty, "float"),
        make_tuple("double", empty, "float"),
        make_tuple("bool", empty, "bool"),
        make_tuple("float-image", buildImageOutput<float>, "float-image"),
        make_tuple("int-image", buildImageOutput<uint32_t>, "int-image"),
        make_tuple("py-object", empty, "py-object")
    };

    const auto& t = m_source.lock()->traits();
    for (auto& tri: types) {
        if (t.hasTrait(get<0>(tri))) {
            if (get<1>(tri)) {
                m_interfaceModule = get<1>(tri)(platform);
                m_source.lock()->bind(m_interfaceModule->inputPort(0));
            }
            m_type = QString::fromStdString(get<2>(tri));
            return;
        }
    }

    string errorMsg = "unknown output type for port '"
        + m_source.lock()->name() + "' (info:["
        + portTypeTraitsToString(m_source.lock()->traits()) + "])";
    throw std::runtime_error(errorMsg);
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
    return m_type;
}

int BackendOutput::status() const
{
    return (int)(m_interfaceModule && m_interfaceModule->getImage());
}

QVariant BackendOutput::value() const
{
    QVariantMap vmap;
    auto im = m_interfaceModule->getImage();
    if (im == nullptr) {
        vmap["valid"] = false;
    } else {
        vmap["valid"] = true;
        VolumeTexture* tex = new VolumeTexture;
        tex->init(*im);
        vmap["texture"] = QVariant::fromValue(tex);
        // TODO:
        // vmap["histogram"]
    }
    return vmap;
}

QVariant BackendOutput::hints() const
{
    return m_hints;
}

void BackendOutput::setName(const QString& name)
{}

void BackendOutput::setStatus(int status)
{}

bool BackendOutput::setValue(QVariant value)
{
    return false;
}

std::weak_ptr<OutputPort> BackendOutput::source()
{
    return m_source;
}