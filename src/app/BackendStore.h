#ifndef _app_BackendStore_h_
#define _app_BackendStore_h_

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <core/compute_platform/ComputeModule.h>
#include <memory>

#include "BackendStoreItem.h"

class BackendStoreDummyItem : public BackendStoreItem {
public:
    BackendStoreDummyItem(
        int uid,
        int parentUid,
        QString category,
        QString name,
        QString type,
        int status)
        :
        m_uid(uid),
        m_parentUid(parentUid),
        m_category(category),
        m_name(name),
        m_type(type),
        m_status(status)
    {}
    int uid() const override { return m_uid; }
    int parentUid() const override { return m_parentUid; }
    QString category() const override { return m_category; }
    QString name() const override { return m_name; }
    QString type() const override { return m_type; }
    int status() const override { return m_status; }
    QVariant value() const override { return QVariant(); }
protected:
    int m_uid;
    int m_parentUid;
    QString m_category;
    QString m_name;
    QString m_type;
    int m_status;
};

class BackendStore: public QAbstractListModel {
    Q_OBJECT
public:
    enum ModuleRoles {
        UidRole = Qt::UserRole,
        ParentUidRole,
        CategoryRole,
        NameRole,
        TypeRole,
        StatusRole,
        ValueRole
    };
    explicit BackendStore(QObject* parent = Q_NULLPTR);
    virtual ~BackendStore() = default;

    QVariant data(const QModelIndex &index, int role) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addModule(int uid, int parentUid, QString category,
        QString name, QString type, int status);
protected:
    std::vector<std::unique_ptr<BackendStoreItem>> m_items;
};

class BackendStoreFilter: public QSortFilterProxyModel {
    Q_OBJECT
    Q_PROPERTY(QAbstractItemModel* source READ sourceModel WRITE setSourceModel)
    Q_PROPERTY(QList<int> includeUid READ includeUid WRITE setIncludeUid)
    Q_PROPERTY(QList<int> excludeUid READ excludeUid WRITE setExcludeUid)
    Q_PROPERTY(QList<int> includeParentUid READ includeParentUid WRITE setIncludeParentUid)
    Q_PROPERTY(QList<int> excludeParentUid READ excludeParentUid WRITE setExcludeParentUid)    
    Q_PROPERTY(QList<QString> includeCategory READ includeCategory WRITE setIncludeCategory)
    Q_PROPERTY(QList<QString> excludeCategory READ excludeCategory WRITE setExcludeCategory)
    Q_PROPERTY(QList<QString> includeType READ includeType WRITE setIncludeType)
    Q_PROPERTY(QList<QString> excludeType READ excludeType WRITE setExcludeType)
    // TODO: add more filters when needed
public:
    explicit BackendStoreFilter(QObject* parent = Q_NULLPTR);
    
    QList<int> includeUid() const;
    QList<int> excludeUid() const;
    QList<int> includeParentUid() const;
    QList<int> excludeParentUid() const;
    QList<QString> includeCategory() const;
    QList<QString> excludeCategory() const;
    QList<QString> includeType() const;
    QList<QString> excludeType() const;

    void setIncludeUid(QList<int> list);
    void setExcludeUid(QList<int> list);
    void setIncludeParentUid(QList<int> list);
    void setExcludeParentUid(QList<int> list);
    void setIncludeCategory(QList<QString> list);
    void setExcludeCategory(QList<QString> list);
    void setIncludeType(QList<QString> list);
    void setExcludeType(QList<QString> list);
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
    QList<int> m_includeUid;
    QList<int> m_excludeUid;
    QList<int> m_includeParentUid;
    QList<int> m_excludeParentUid;
    QList<QString> m_includeCategory;
    QList<QString> m_excludeCategory;
    QList<QString> m_includeType;
    QList<QString> m_excludeType;
};

class BackendStoreMatch: public QSortFilterProxyModel {
    Q_OBJECT
    // Q_PROPERTY(QAbstractItemModel* source READ sourceModel WRITE setSourceModel)
    // Q_PROPERTY(bool hasMatch READ hasMatch NOTIFY hasMatchChanged)
    // Q_PROPERTY(QString matchBy READ matchBy WRITE setMatchBy NOTIFY matchByChanged)
    // Q_PROPERTY(QVariant matchValue READ matchValue WRITE setMatchValue NOTIFY matchValueChanged)
    // Q_PROPERTY(int uid READ uid NOTIFY uidChanged)
    // Q_PROPERTY(int parentUid READ parentUid NOTIFY parentUidChanged)
    // Q_PROPERTY(QString category READ category NOTIFY categoryChanged)
    // Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    // Q_PROPERTY(QString type READ type NOTIFY typeChanged)
    // Q_PROPERTY(int status READ status NOTIFY statusChanged)
    // Q_PROPERTY(QVariant value READ value NOTIFY valueChanged)
public:
    explicit BackendStoreMatch(QObject* parent = Q_NULLPTR);
protected:
    // bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
};

#endif
