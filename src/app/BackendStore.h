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
    Q_INVOKABLE QVariant get(int row);
    Q_INVOKABLE int count() const;
protected:
    std::vector<std::unique_ptr<BackendStoreItem>> m_items;
};

class BackendStoreFilter: public QSortFilterProxyModel {
    Q_OBJECT
    Q_PROPERTY(BackendStore* source READ sourceStore WRITE setSourceStore)
    Q_PROPERTY(QList<int> includeUid READ includeUid WRITE setIncludeUid)
    Q_PROPERTY(QList<int> excludeUid READ excludeUid WRITE setExcludeUid)
    Q_PROPERTY(QList<int> includeParentUid READ includeParentUid WRITE setIncludeParentUid)
    Q_PROPERTY(QList<int> excludeParentUid READ excludeParentUid WRITE setExcludeParentUid)    
    Q_PROPERTY(QList<QString> includeCategory READ includeCategory WRITE setIncludeCategory)
    Q_PROPERTY(QList<QString> excludeCategory READ excludeCategory WRITE setExcludeCategory)
    Q_PROPERTY(QList<QString> includeType READ includeType WRITE setIncludeType)
    Q_PROPERTY(QList<QString> excludeType READ excludeType WRITE setExcludeType)
    // TODO: add more filters when needed
    Q_PROPERTY(QVariant first READ first NOTIFY firstChanged)
public:
    explicit BackendStoreFilter(QObject* parent = Q_NULLPTR);

    BackendStore* sourceStore() const;
    void setSourceStore(BackendStore* store);

    QVariant first() const;

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

    Q_INVOKABLE QVariant get(int row) const;
    Q_INVOKABLE int count() const;

Q_SIGNALS:
    void firstChanged();
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
    BackendStore* m_store = nullptr;
    QList<int> m_includeUid;
    QList<int> m_excludeUid;
    QList<int> m_includeParentUid;
    QList<int> m_excludeParentUid;
    QList<QString> m_includeCategory;
    QList<QString> m_excludeCategory;
    QList<QString> m_includeType;
    QList<QString> m_excludeType;
};

#endif
