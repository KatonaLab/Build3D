#ifndef _app_BackendStore_h_
#define _app_BackendStore_h_

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QDebug>
#include <core/compute_platform/ComputeModule.h>
#include <memory>

#include "BackendStoreItem.h"

class BackendStoreRootItem : public BackendStoreItem {
public:
    int uid() const override { return -1; }
    QString category() const override { return QString("root"); }
    QString name() const override { return QString("root"); }
    QString type() const override { return QString("root"); }
    int status() const override { return 0; }
    QVariant value() const override { return QVariant(); }
};

class BackendStoreDummyItem : public BackendStoreItem {
public:
    BackendStoreDummyItem(int id, QString name, QString type)
        : m_id(id), m_name(name), m_type(type)
    {}
    int uid() const override { return m_id; }
    QString category() const override { return QString("dummy"); }
    QString name() const override { return m_name; }
    QString type() const override { return m_type; }
    int status() const override { return 0; }
    QVariant value() const override { return QVariant(); }
protected:
    int m_id;
    QString m_name;
    QString m_type;
};

class BackendStore: public QAbstractItemModel {
    Q_OBJECT
public:
    enum ModuleRoles {
        UidRole = Qt::UserRole,
        CategoryRole,
        NameRole,
        TypeRole,
        StatusRole,
        ValueRole
    };
    explicit BackendStore(QObject* parent = Q_NULLPTR);
    virtual ~BackendStore();

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column,
        const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    Q_INVOKABLE void addModule(QString name, QString type);
protected:
    // TODO: use shared/unique_ptr
    BackendStoreItem* m_root;
};

class BackendStoreProxy: public QSortFilterProxyModel {
    Q_OBJECT
public:
    explicit BackendStoreProxy(QObject* parent = Q_NULLPTR);
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
};

#endif
