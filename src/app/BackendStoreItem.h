#ifndef _app_BackendStoreItem_h_
#define _app_BackendStoreItem_h_

#include <QString>
#include <QVariant>
#include <QObject>

class BackendStoreItem: public QObject {
    Q_OBJECT
    Q_PROPERTY(int uid READ uid)
    Q_PROPERTY(int parentUid READ parentUid)
    Q_PROPERTY(QString category READ category)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type)
    Q_PROPERTY(int status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariant value READ value NOTIFY valueChanged)
public:
    virtual int uid() const = 0;
    virtual int parentUid() const = 0;
    virtual QString category() const = 0;
    virtual QString name() const = 0;
    virtual QString type() const = 0;
    virtual int status() const = 0;
    virtual QVariant value() const = 0;
    virtual ~BackendStoreItem() = default;
Q_SIGNALS:
    void nameChanged();
    void statusChanged();
    void valueChanged();
};

#endif
