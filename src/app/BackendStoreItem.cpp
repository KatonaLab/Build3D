#include "BackendStoreItem.h"

void BackendStoreItem::add(BackendStoreItem* child)
{
    child->m_parent = this;
    m_children.append(child);
}

BackendStoreItem* BackendStoreItem::parent()
{
    return m_parent;
}

BackendStoreItem* BackendStoreItem::child(int row)
{
    if (row >= 0 && row < m_children.size()) {
        return m_children.at(row);
    } else {
        return nullptr;
    }
}

int BackendStoreItem::numChildren() const
{
    return m_children.size();
}

int BackendStoreItem::columnCount() const
{
    return 1;
}

int BackendStoreItem::row()
{
    if (m_parent) {
        return m_parent->m_children.indexOf(const_cast<BackendStoreItem*>(this));
    }
    return 0;
}
BackendStoreItem::~BackendStoreItem()
{
    qDeleteAll(m_children);
}