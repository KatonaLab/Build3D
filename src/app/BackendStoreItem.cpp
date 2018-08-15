#include "BackendStoreItem.h"

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