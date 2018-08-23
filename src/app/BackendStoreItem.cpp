#include "BackendStoreItem.h"

#include <algorithm>
#include <string>

using namespace core::compute_platform;
using namespace std;

QVariantMap details::propertyMapToQVariantMap(const PropertyMap& properties)
{
    QVariantMap vmap;
    auto ks = properties.keys();
    for (const string& key: ks) {
        QString vmapKey = QString::fromStdString(key);
        switch (properties.getType(key)) {
            case PropertyMap::Type::Int:
                vmap[vmapKey] = QVariant(properties.asInt(key));
                break;
            case PropertyMap::Type::Bool:
                vmap[vmapKey] = QVariant(properties.asBool(key));
                break;
            case PropertyMap::Type::String:
                vmap[vmapKey] = QVariant(QString::fromStdString(properties.asString(key)));
                break;
            case PropertyMap::Type::Float:
                vmap[vmapKey] = QVariant(properties.asFloat(key));
                break;
        }
    }
    return vmap;
}

string details::propertyMapToString(const PropertyMap& properties)
{
    auto ks = properties.keys();
    return accumulate(ks.begin(), ks.end(), string(),
        [](const std::string& acc, const std::string& key) {
            return acc + ", " + key;
        });
}

string details::portTypeTraitsToString(const PortTypeTraitsBase& traits)
{
    auto c = traits.getAll();
    return accumulate(c.begin(), c.end(), string(),
        [](const std::string& acc, const std::string& key) {
            return acc + ", " + key;
        });
}
