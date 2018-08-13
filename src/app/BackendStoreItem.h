#ifndef _app_BackendStoreItem_h_
#define _app_BackendStoreItem_h_

#include <QList>

// TODO: change * to shared/weak_ptr

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
    int numChildren() const;
    int row();
    int columnCount() const;
    virtual ~BackendStoreItem();
protected:
    QList<BackendStoreItem*> m_children;
    BackendStoreItem* m_parent = nullptr;
};

#endif
