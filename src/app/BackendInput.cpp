#include "BackendInput.h"
#include "BackendStore.h"

#include <QDebug>

using namespace core::compute_platform;
using namespace std;

BackendInput::BackendInput(std::weak_ptr<InputPort> source, int portId,
    int parentUid, const BackendStore& store)
    : m_source(source), m_portId(portId),
    m_parentUid(parentUid), m_store(store)
{
    if (m_source.lock() == nullptr) {
        throw std::runtime_error("invalid source port");
    }

    using namespace details;

    m_hints = propertyMapToQVariantMap(m_source.lock()->properties());

    static vector<pair<string, string>> types = {
        make_pair("uint8_t", "int"),
        make_pair("uint16_t", "int"),
        make_pair("uint32_t", "int"),
        make_pair("uint64_t", "int"),
        make_pair("int8_t", "int"),
        make_pair("int16_t", "int"),
        make_pair("int32_t", "int"),
        make_pair("int64_t", "int"),
        make_pair("float", "float"),
        make_pair("double", "float"),
        make_pair("bool", "bool"),
        make_pair("string", "string"),
        make_pair("float-image", "float-image"),
        make_pair("int-image", "int-image"),
        make_pair("py-object", "py-object")
    };

    const auto& t = m_source.lock()->traits();
    for (auto& pr: types) {
        if (t.hasTrait(pr.first)) {
            m_type = QString::fromStdString(pr.second);
            return;
        }
    }

    string errorMsg = "unknown input type for port '"
        + m_source.lock()->name() + "' (info:["
        + portTypeTraitsToString(m_source.lock()->traits()) + "])";
    throw std::runtime_error(errorMsg);
}

int BackendInput::uid() const
{
    return m_portId;
}

int BackendInput::parentUid() const
{
    return m_parentUid;
}

QString BackendInput::category() const
{
    return QString("input");
}

QString BackendInput::name() const
{
    // TODO: check for nullptr
    return QString::fromStdString(m_source.lock()->name());
}

QString BackendInput::type() const
{
    return m_type;
}

int BackendInput::status() const
{
    return 0;
}

QVariant BackendInput::value() const
{
    QVariantMap vmap;
    vmap["parentUid"] = -1;
    vmap["uid"] = -1;

    if (auto p = m_source.lock()) {
        if (p->connected()) {
            auto uidPair = m_store.findPort(p->getSource());
            vmap["parentUid"] = uidPair.first;
            vmap["uid"] = uidPair.second;
        }
    }
    return vmap;
}

QVariant BackendInput::hints() const
{
    return m_hints;
}

void BackendInput::setName(const QString& name)
{}

void BackendInput::setStatus(int status)
{}

bool BackendInput::setValue(QVariant value)
{
    return false;
}

std::weak_ptr<InputPort> BackendInput::source()
{
    return m_source;
}
