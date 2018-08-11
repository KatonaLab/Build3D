#ifndef _app_BackendStore_h_
#define _app_BackendStore_h_

#include <QAbstractListModel>
#include <core/compute_platform/ComputeModule.h>
#include <memory>

// class BackendModule : public BackendStoreItem {
// public:
//     static BackendModule* createPythonModule(const QString& scriptPath);
// }

// class PortProxyStore: public QAbstractProxy

// class PortStore: public QAbstractListModel {
//     Q_OBJECT
// public:
//     enum PortRoles {
//         IdRole = Qt::UserRole,
//         NameRole,
//         TypeRole,
//         ValueRole
//     };
// };

// class ModuleStoreItem {
// public:
//     int uid() const;
//     QString name() const;
//     void setName(const QString& name);
//     QString type() const;
//     int status() const;

//     QAbstractItemModel* inputsModel();
//     QAbstractItemModel* parametersModel();
//     QAbstractItemModel* outputsModel();

//     core::compute_platform::ComputeModule& computeModule();
//     const core::compute_platform::ComputeModule& computeModule() const;
// protected:
//     int m_uid = -1;
// };

// // Q_DECLARE_METATYPE(ModuleStoreItem)

// // class ModuleStoreItemFactory: public QObject {
// //     Q_OBJECT
// // public:
// //     static ModuleStoreItem* createPythonModule(const QString& scriptPath);
// //     static ModuleStoreItem* createIcsInputModule(const QString& icsFilePath);

// //     explicit ModuleStoreItemFactory(QObject* parent = Q_NULLPTR);
// // };

// class ModuleStore: public QAbstractListModel {
//     Q_OBJECT
// public:
//     enum ModuleRoles {
//         UidRole = Qt::UserRole,
//         NameRole,
//         TypeRole,
//         StatusRole,
//         IntputsRole,
//         ParametersRole,
//         OutputsRole
//     };

//     explicit ModuleStore(QObject* parent = Q_NULLPTR);
//     ~ModuleStore() override;

//     int rowCount(const QModelIndex& parent = QModelIndex()) const override;
//     QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
//     QHash<int, QByteArray> roleNames() const override;

//     Q_INVOKABLE int addModule(const QString& typeName);
//     Q_INVOKABLE void removeModule(int uid);
// protected:
//     std::vector<std::unique_ptr<ModuleStoreItem>> m_items;
// };

#endif
