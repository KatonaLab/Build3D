#include "BackendStore.h"

#include <iostream>
#include <iterator>
#include <algorithm>

using namespace std;

ModuleStoreItem::ModuleStoreItem(QObject* parent)
    : QObject(parent)
{}

ModuleStoreItem::ModuleStoreItem(const ModuleStoreItem &other)
{
    // TODO:
    m_uid = other.m_uid;
}

// ----

ModuleStore::ModuleStore(QObject* parent)
    : QAbstractListModel(parent)
{}

int ModuleStore::rowCount(const QModelIndex& parent) const
{
    return m_items.size();
}

QVariant ModuleStore::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole || index.row() >= m_items.size()) {
        return QVariant();
    }

    return m_items.at(index.row());
}

int ModuleStore::addModule(const QString& typeName)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    
    // TODO: call factory
    ModuleStoreItem* newItem = new ModuleStoreItem(this);
    newItem->m_incr = m_items.size() + 1;
    newItem->m_str = typeName;
    QVariant var;
    var.setValue(newItem);
    m_items.push_back(var);
    cout << "added " << m_items.size() << endl;
    
    endInsertRows();
    
    return newItem->uid();
}

void ModuleStore::removeModule(int uid)
{
    auto it = find_if(m_items.begin(), m_items.end(),
        [uid](const QVariant& var) {
            return qvariant_cast<ModuleStoreItem*>(var)->uid() == uid;
        });

    if (it != m_items.end()) {
        int pos = distance(m_items.begin(), it);
        beginRemoveRows(QModelIndex(), pos, pos);
        m_items.erase(it);
        endRemoveRows();
    }
}