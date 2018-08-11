#include "BackendStoreItem.h"

void BackendStoreItem::add(BackendStoreItem* child)
{
    m_children.append(child);
}

BackendStoreItem* BackendStoreItem::parent()
{
    m_parent;
}

BackendStoreItem* BackendStoreItem::child(int row)
{
    m_children[row];
}

int BackendStoreItem::row()
{
    if (m_parent) {
        return m_parent->m_children.indexOf(this);
    }
    return 0;
}
BackendStoreItem::~BackendStoreItem()
{
    qDeleteAll(m_children);
}