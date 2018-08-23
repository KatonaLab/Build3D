#ifndef _app_BackendStore_h_
#define _app_BackendStore_h_

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <core/compute_platform/ComputeModule.h>
#include <core/high_platform/PythonComputeModule.h>
#include <memory>
#include <QString>

#include "BackendStoreItem.h"
#include <utility>
#include <core/compute_platform/ports.h>

class BackendStore: public QAbstractListModel {
    Q_OBJECT
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    typedef core::compute_platform::PortBase PortBase;
    Q_PROPERTY(QVariantList availableModules READ availableModules NOTIFY availableModulesChanged)
public:
    enum ModuleRoles {
        UidRole = Qt::UserRole,
        ParentUidRole,
        CategoryRole,
        NameRole,
        TypeRole,
        StatusRole,
        HintsRole,
        ValueRole
    };
    explicit BackendStore(QObject* parent = Q_NULLPTR);
    virtual ~BackendStore() = default;

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    QVariantList availableModules() const;
    Q_INVOKABLE void refreshAvailableModules();

    Q_INVOKABLE void addModule(const QString& scriptPath);
    Q_INVOKABLE void removeModule(int uid);
    Q_INVOKABLE QVariant get(int row);
    Q_INVOKABLE int count() const;
    Q_INVOKABLE bool connect(int outModuleUid, int outPortUid, int inModuleUid, int inPortUid);
    // TODO:
    // Q_INVOKABLE bool disconnect(int outModuleUid, int outPortUid, int inModuleUid, int inPortUid);
    Q_INVOKABLE void evaluate(int uid = -1);

    std::pair<int, int> findPort(std::weak_ptr<PortBase> port) const;

Q_SIGNALS:
    void availableModulesChanged();

protected:
    std::vector<std::unique_ptr<BackendStoreItem>> m_items;
    ComputePlatform m_platform;
    QVariantList m_availableModules;
    int m_uidCounter = 0;
    void addBackendStoreItem(std::unique_ptr<BackendStoreItem>&& item);
    void itemChanged(const BackendStoreItem* item, ModuleRoles role);
    void addAvailableNativeModules();
};

class BackendStoreFilter: public QSortFilterProxyModel {
    Q_OBJECT
    Q_PROPERTY(BackendStore* source READ sourceStore WRITE setSourceStore)
    Q_PROPERTY(QList<int> includeUid READ includeUid WRITE setIncludeUid)
    Q_PROPERTY(QList<int> excludeUid READ excludeUid WRITE setExcludeUid)
    Q_PROPERTY(QList<int> includeParentUid READ includeParentUid WRITE setIncludeParentUid)
    Q_PROPERTY(QList<int> excludeParentUid READ excludeParentUid WRITE setExcludeParentUid)
    Q_PROPERTY(QList<int> includeStatus READ includeStatus WRITE setIncludeStatus)
    Q_PROPERTY(QList<int> excludeStatus READ excludeStatus WRITE setExcludeStatus)
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
    QList<int> includeStatus() const;
    QList<int> excludeStatus() const;
    QList<QString> includeCategory() const;
    QList<QString> excludeCategory() const;
    QList<QString> includeType() const;
    QList<QString> excludeType() const;

    void setIncludeUid(QList<int> list);
    void setExcludeUid(QList<int> list);
    void setIncludeParentUid(QList<int> list);
    void setExcludeParentUid(QList<int> list);
    void setIncludeStatus(QList<int> list);
    void setExcludeStatus(QList<int> list);
    void setIncludeCategory(QList<QString> list);
    void setExcludeCategory(QList<QString> list);
    void setIncludeType(QList<QString> list);
    void setExcludeType(QList<QString> list);

    Q_INVOKABLE QVariant get(int row) const;
    Q_INVOKABLE int count() const;

Q_SIGNALS:
    void firstChanged();
protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
    BackendStore* m_store = nullptr;
    QList<int> m_includeUid;
    QList<int> m_excludeUid;
    QList<int> m_includeParentUid;
    QList<int> m_excludeParentUid;
    QList<int> m_includeStatus;
    QList<int> m_excludeStatus;
    QList<QString> m_includeCategory;
    QList<QString> m_excludeCategory;
    QList<QString> m_includeType;
    QList<QString> m_excludeType;
};

#endif
