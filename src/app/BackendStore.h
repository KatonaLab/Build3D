#ifndef _app_BackendStore_h_
#define _app_BackendStore_h_

#include <QtCore>
#include <core/compute_platform/ComputeModule.h>

// class ModuleIOModel: public QAbstractListModel {
//     Q_OBJECT
// public:
//     explicit ModuleIOModel(ModuleStoreItem* source, QObject* parent = Q_NULLPTR);
// };

class ModuleStoreItem: public QObject {
    Q_OBJECT
    Q_PROPERTY(int uid READ uid())
    Q_PROPERTY(int incr READ incr())
    Q_PROPERTY(QString hello READ str())
    // Q_PROPERTY(QAbstractItemModel* inputs READ inputsModel)
    // Q_PROPERTY(QAbstractItemModel* parameters READ parametersModel)
    // Q_PROPERTY(QAbstractItemModel* outputs READ outputsModel)
    // Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged);
    // Q_PROPERTY(QString type READ type);
    // Q_PROPERTY(bool modified READ modified NOTIFY modifiedChanged);
public:
    explicit ModuleStoreItem(QObject* parent = Q_NULLPTR);
    ModuleStoreItem(const ModuleStoreItem &other);
    ~ModuleStoreItem() = default;

    int uid() const { return m_uid; }
    int incr() const { return m_incr; }
    QString str() const { return m_str; }
    // core::compute_platform::ComputeModule& computeModule();
    // const core::compute_platform::ComputeModule& computeModule() const;

    // QAbstractItemModel* inputsModel();
    // QAbstractItemModel* parametersModel();
    // QAbstractItemModel* outputsModel();
    int m_incr = 0;
    QString m_str;
protected:
    int m_uid = 0;
};

Q_DECLARE_METATYPE(ModuleStoreItem)

class ModuleStoreItemFactory: public QObject {
    Q_OBJECT
public:
    static ModuleStoreItem* createPythonModule(const QString& scriptPath);
    static ModuleStoreItem* createIcsInputModule(const QString& icsFilePath);

    explicit ModuleStoreItemFactory(QObject* parent = Q_NULLPTR);
};

class ModuleStore: public QAbstractListModel {
    Q_OBJECT
public:
    Q_INVOKABLE int addModule(const QString& typeName);
    Q_INVOKABLE void removeModule(int uid);
    explicit ModuleStore(QObject* parent = Q_NULLPTR);
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
protected:
    std::vector<QVariant> m_items;
};

#endif
