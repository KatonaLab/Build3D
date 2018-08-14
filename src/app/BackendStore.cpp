#include "BackendStore.h"

#include <tuple>
#include <vector>

using namespace std;

BackendStore::BackendStore(QObject* parent)
    : QAbstractListModel(parent)
{}

void BackendStore::addModule(int uid, int parentUid, QString category,
    QString name, QString type, int status)
{
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items.push_back(make_unique<BackendStoreDummyItem>(uid, parentUid, category, name, type, status));
    endInsertRows();
}

QHash<int, QByteArray> BackendStore::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        {UidRole, "uid"},
        {ParentUidRole, "parentUid"},
        {CategoryRole, "category"},
        {NameRole, "name"},
        {TypeRole, "type"},
        {StatusRole, "status"},
        {ValueRole, "value"}
    };
    return roles;
}

QVariant BackendStore::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto& item = m_items[index.row()];

    switch (role) {
        case UidRole: return item->uid();
        case ParentUidRole: return item->parentUid();
        case CategoryRole: return item->category();
        case NameRole: return item->name();
        case TypeRole: return item->type();
        case StatusRole: return item->status();
        case ValueRole: return item->value();
        default: return QVariant();
    }
}

int BackendStore::rowCount(const QModelIndex& parent) const
{
    return m_items.size();
}

BackendStoreFilter::BackendStoreFilter(QObject* parent)
    :  QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

bool BackendStoreFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (!index.isValid()) {
        return false;
    }

    vector<tuple<const QList<int>*, BackendStore::ModuleRoles, bool>> intTri = {
        make_tuple(&m_includeUid, BackendStore::UidRole, true),
        make_tuple(&m_excludeUid, BackendStore::UidRole, false),
        make_tuple(&m_includeParentUid, BackendStore::ParentUidRole, true),
        make_tuple(&m_excludeParentUid, BackendStore::ParentUidRole, false)
    };

    for (auto& tri: intTri) {
        if (get<0>(tri)->empty()) {
            continue;
        }
        int value = sourceModel()->data(index, get<1>(tri)).toInt();
        // "contains?" XOR "should be contained?"
        if ((bool)get<0>(tri)->contains(value) != (bool)get<2>(tri)) {
            return false;
        }
    }

    vector<tuple<const QList<QString>*, BackendStore::ModuleRoles, bool>> stringTri = {
        make_tuple(&m_includeCategory, BackendStore::CategoryRole, true),
        make_tuple(&m_excludeCategory, BackendStore::CategoryRole, false),
        make_tuple(&m_includeType, BackendStore::TypeRole, true),
        make_tuple(&m_excludeType, BackendStore::TypeRole, false)
    };

    for (auto& tri: stringTri) {
        if (get<0>(tri)->empty()) {
            continue;
        }
        QString value = sourceModel()->data(index, get<1>(tri)).toString();
        // "contains?" XOR "should be contained?"
        if ((bool)get<0>(tri)->contains(value) != (bool)get<2>(tri)) {
            return false;
        }
    }

    return true;
}

QList<int> BackendStoreFilter::includeUid() const
{
    return m_includeUid;
}

QList<int> BackendStoreFilter::excludeUid() const
{
    return m_excludeUid;
}

QList<int> BackendStoreFilter::includeParentUid() const
{
    return m_includeParentUid;
}

QList<int> BackendStoreFilter::excludeParentUid() const
{
    return m_excludeParentUid;
}

QList<QString> BackendStoreFilter::includeCategory() const
{
    return m_includeCategory;
}

QList<QString> BackendStoreFilter::excludeCategory() const
{
    return m_excludeCategory;
}

QList<QString> BackendStoreFilter::includeType() const
{
    return m_includeType;
}

QList<QString> BackendStoreFilter::excludeType() const
{
    return m_excludeType;
}

void BackendStoreFilter::setIncludeUid(QList<int> list)
{
    m_includeUid = list;
}

void BackendStoreFilter::setExcludeUid(QList<int> list)
{
    m_excludeUid = list;
}

void BackendStoreFilter::setIncludeParentUid(QList<int> list)
{
    m_includeParentUid = list;
}

void BackendStoreFilter::setExcludeParentUid(QList<int> list)
{
    m_excludeParentUid = list;
}

void BackendStoreFilter::setIncludeCategory(QList<QString> list)
{
    m_includeCategory = list;
}

void BackendStoreFilter::setExcludeCategory(QList<QString> list)
{
    m_excludeCategory = list;
}

void BackendStoreFilter::setIncludeType(QList<QString> list)
{
    m_includeType = list;
}

void BackendStoreFilter::setExcludeType(QList<QString> list)
{
    m_excludeType = list;
}

BackendStoreMatch::BackendStoreMatch(QObject* parent)
    :  QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}