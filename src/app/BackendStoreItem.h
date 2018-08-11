#ifndef _app_BackendStoreItem_h_
#define _app_BackendStoreItem_h_

#include <QList>

class BackendStoreItem {
public:
    virtual int uid() const = 0;
    virtual QString category() const = 0;
    virtual QString name() const = 0;
    virtual QString type() const = 0;
    virtual int status() const = 0;
    virtual QVariant value() const = 0;

    void add(BackendStoreItem* child);
    BackendStoreItem* parent();
    BackendStoreItem* child(int row);
    int row();
    virtual ~BackendStoreItem();
protected:
    QList<BackendStoreItem*> m_children;
    BackendStoreItem* m_parent;
};

#endif
