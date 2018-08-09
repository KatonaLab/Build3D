#include "BackendStore.h"

#include <iostream>
#include <iterator>
#include <algorithm>

using namespace std;

ModuleStore::ModuleStore(QObject* parent)
    : QAbstractListModel(parent)
{}

int ModuleStore::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_items.size();
}

QVariant ModuleStore::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= (int)m_items.size()) {
        return QVariant();
    }

    auto& item = m_items[index.row()];

    switch (role) {
        case UidRole: return item->uid();
        case NameRole: return QVariant("Mario");
        case TypeRole: return QVariant("plumber");
        case StatusRole: return QVariant("on quest");
        case IntputsRole: return QVariant();
        case ParametersRole: return QVariant();
        case OutputsRole: return QVariant();
        default: return QVariant();
    }
}

QHash<int, QByteArray> ModuleStore::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        {UidRole, "uid"},
        {NameRole, "name"},
        {TypeRole, "type"},
        {StatusRole, "status"},
        {IntputsRole, "inputs"},
        {ParametersRole, "parameters"},
        {OutputsRole, "outputs"}};
    return roles;
}

int ModuleStore::addModule(const QString& typeName)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    // TODO: call factory
    m_items.emplace_back(new ModuleStoreItem);
    endInsertRows();
    return m_items.back()->uid();
}

void ModuleStore::removeModule(int uid)
{
    auto it = find_if(m_items.begin(), m_items.end(),
        [uid](const unique_ptr<ModuleStoreItem>& item) {
            return item->uid() == uid;
        });

    if (it != m_items.end()) {
        int pos = distance(m_items.begin(), it);
        beginRemoveRows(QModelIndex(), pos, pos);
        m_items.erase(it);
        endRemoveRows();
    }
}

ModuleStore::~ModuleStore()
{
    beginResetModel();
    m_items.clear();
    endResetModel();
}