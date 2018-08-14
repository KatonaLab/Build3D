#ifndef _app_BackendStoreItem_h_
#define _app_BackendStoreItem_h_

#include <QString>
#include <QVariant>

class BackendStoreItem {
public:
    virtual int uid() const = 0;
    virtual int parentUid() const = 0;
    virtual QString category() const = 0;
    virtual QString name() const = 0;
    virtual QString type() const = 0;
    virtual int status() const = 0;
    virtual QVariant value() const = 0;
    virtual ~BackendStoreItem() = default;
};

#endif
