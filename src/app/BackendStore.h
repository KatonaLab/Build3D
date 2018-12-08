#ifndef _app_BackendStore_h_
#define _app_BackendStore_h_

#include <QAbstractListModel>
#include <QSortFilterProxyModel>
#include <core/compute_platform/ComputeModule.h>
#include <core/high_platform/PythonComputeModule.h>
#include <memory>
#include <QString>
#include <QUrl>
#include <QJsonObject>
#include <QFutureWatcher>

#include "BackendStoreItem.h"
#include "GlobalSettings.h"
#include <utility>
#include <atomic>
#include <core/compute_platform/ports.h>

class BackendStore;

class BackendStoreSerializer {
public:
    void write(QString filename, BackendStore& store);
    void read(QString filename, BackendStore& store);
};

class BackendStore: public QAbstractListModel {
    Q_OBJECT
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    typedef core::compute_platform::PortBase PortBase;
    Q_PROPERTY(QVariantList availableModules READ availableModules NOTIFY availableModulesChanged)
    Q_PROPERTY(QVariantList availableWorkflows READ availableWorkflows NOTIFY availableWorkflowsChanged)
    Q_PROPERTY(bool editorMode READ editorMode NOTIFY editorModeChanged)
    Q_PROPERTY(bool unsaved READ unsaved WRITE setUnsaved NOTIFY unsavedChanged)
    Q_PROPERTY(bool smoothTextures READ smoothTextures WRITE setSmoothTextures NOTIFY smoothTexturesChanged)
    Q_PROPERTY(bool hasActiveAsyncTask READ hasActiveAsyncTask NOTIFY hasActiveAsyncTaskChanged)

    friend class BackendStoreSerializer;
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

    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    QVariantList availableModules() const;
    QVariantList availableWorkflows() const;
    Q_INVOKABLE void refreshAvailableModules();
    Q_INVOKABLE void refreshAvailableWorkflows();

    Q_INVOKABLE void addModule(const QString& scriptPath);
    Q_INVOKABLE void removeModule(int uid);
    Q_INVOKABLE QVariant get(int row) const;
    Q_INVOKABLE int count() const;
    Q_INVOKABLE bool connect(int outModuleUid, int outPortUid, int inModuleUid, int inPortUid);
    // TODO:
    // Q_INVOKABLE bool disconnect(int outModuleUid, int outPortUid, int inModuleUid, int inPortUid);
    Q_INVOKABLE void startAsyncEvaluate(int uid = -1);
    Q_INVOKABLE void startAsyncEvaluateBatch();
    Q_INVOKABLE void stopAsync();
    Q_INVOKABLE void readWorkflow(const QUrl& url);
    Q_INVOKABLE void writeWorkflow(const QUrl& url);
    Q_INVOKABLE void newWorkflow();

    std::pair<int, int> findPort(std::weak_ptr<PortBase> port) const;
    BackendStoreItem* getItem(int parentUid, int uid, QString category);

    // TODO: definition goes to .cpp
    bool editorMode()
    {
        return m_editorMode;
    }

    bool unsaved()
    {
        return m_unsaved;
    }

    bool smoothTextures()
    {
        return m_smoothTextures;
    }

    void setSmoothTextures(bool value)
    {
        if (m_smoothTextures != value) {
            m_smoothTextures = value;
            Q_EMIT smoothTexturesChanged();
        }
    }

    bool hasActiveAsyncTask()
    {
        return m_asyncTaskFutureWatcher.isRunning();
    }
Q_SIGNALS:
    void availableModulesChanged();
    void availableWorkflowsChanged();
    void editorModeChanged();
    void unsavedChanged();
    void smoothTexturesChanged();
    void hasActiveAsyncTaskChanged();

protected:
    std::vector<std::unique_ptr<BackendStoreItem>> m_items;
    ComputePlatform m_platform;
    QVariantList m_availableModules;
    QVariantList m_availableWorkflows;
    bool m_editorMode;
    int m_uidCounter = 0;
    bool m_unsaved = false;
    bool m_smoothTextures = false;
    std::map<QString, int> m_moduleTypeCounter;
    QFutureWatcher<void> m_asyncTaskFutureWatcher;
    std::atomic<bool> m_asyncTaskShouldRun;

    void addBackendStoreItem(std::unique_ptr<BackendStoreItem>&& item);
    void itemChanged(const BackendStoreItem* item, ModuleRoles role);
    void addAvailableNativeModules();
    QString generateModuleName(const QString &type);
    void setUnsaved(bool value = true);
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

QString pathToModuleName(const QString& path);

#endif
