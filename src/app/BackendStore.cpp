#include "BackendStore.h"

#include <tuple>
#include <vector>
#include <core/high_platform/PythonComputeModule.h>
#include "BackendModule.h"
#include "BackendInput.h"
#include "BackendParameter.h"
#include "BackendOutput.h"
#include "IcsDataSourceModule.h"
#include "GlobalSettings.h"
#include <fstream>
#include <algorithm>
#include <QStandardPaths>

using namespace std;
using namespace core::compute_platform;
using namespace core::high_platform;

void BackendStoreSerializer::write(QString filename, BackendStore& store)
{
    QFile saveFile(filename);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << "couldn't open save file" << filename;
        return;
    }

    QJsonArray jsonArray;
    for (size_t i = 0; i < store.m_items.size(); ++i) {
        auto& item = store.m_items[i];
        QJsonObject obj = {
            {"parentUid", item->parentUid()},
            {"uid", item->uid()},
            {"category", item->category()}};
        if (item->category() == QString("module")) {
            obj["order"] = (int)i;
            obj["name"] = item->name();
            obj["module"] = item->hints().toJsonValue();
        } else if (item->category() == QString("input")) {
            obj["connection"] = item->value().toJsonValue();
        } else {
            QVariantMap vmap = item->value().toMap();
            if (vmap.empty()) {
                obj["value"] = QJsonValue::fromVariant(item->value());
            } else {
                obj["value"] = QJsonObject::fromVariantMap(vmap);
            }
        }
        jsonArray.append(obj);
    }
    QJsonObject rootObject = {{"uidCounter", store.m_uidCounter},
        {"items", jsonArray}};
    QJsonDocument saveDoc(rootObject);
    saveFile.write(saveDoc.toJson());
}

void BackendStoreSerializer::read(QString filename, BackendStore& store)
{
    QFile loadFile(filename);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning() << "couldn't open file" << filename;
    } else {
        QByteArray data = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(data));
        QJsonObject json = loadDoc.object();
        QJsonArray array = json["items"].toArray();

        // TODO: restore module order by the "order" parameter in the json file
        for (int i = 0; i < array.size(); ++i) {
            QJsonObject obj = array[i].toObject();
            QString cat = obj["category"].toString();
            if (cat == QString("module")) {
                int uid = obj["uid"].toInt();
                store.m_uidCounter = uid;
                store.addModule(obj["module"].toString());
                auto it = store.getItem(-1, uid, cat);
                if (it) {
                    it->setName(obj["name"].toString());
                } else {
                    qWarning() << "could not load module" << obj;
                }
            }
        }

        for (int i = 0; i < array.size(); ++i) {
            QJsonObject obj = array[i].toObject();
            QString cat = obj["category"].toString();
            if (cat == QString("module")) {
                // nothing to do
            } else if (cat == QString("input")) {
                bool ok = store.connect(
                    obj["connection"].toObject()["parentUid"].toInt(),
                    obj["connection"].toObject()["uid"].toInt(),
                    obj["parentUid"].toInt(),
                    obj["uid"].toInt());
                if (!ok) {
                    qWarning() << "could not connect" << obj;
                }
            } else {
                int parentUid = obj["parentUid"].toInt();
                int uid = obj["uid"].toInt();
                auto it = store.getItem(parentUid, uid, cat);
                if (it) {
                    QVariant var = obj["value"].toVariant();
                    if (var.isValid() && !var.isNull()) {
                        it->setValue(var);
                    }
                } else {
                    qWarning() << "could not set value for" << obj;
                }
            }
        }

        store.m_uidCounter = json["uidCounter"].toInt();
    }
}

QString pathToModuleName(const QString& path)
{
    QString x = path;
    x = x.mid(QString("module").size()).replace('_', ' ').simplified();
    x.chop(3);
    return x;
}

void BackendStore::dirty()
{
    if (!m_unsaved) {
        m_unsaved = true;
        Q_EMIT unsavedChanged();
    }
}

QString BackendStore::generateModuleName(const QString &type)
{
    if (!m_moduleTypeCounter.count(type)) {
        m_moduleTypeCounter[type] = 0;
    }
    return type + QString(" ") + QString::number(++(m_moduleTypeCounter[type]));
}

BackendStore::BackendStore(QObject* parent)
    : QAbstractListModel(parent)
{
    try {
        OutStreamRouters routers;
        routers.stdOut.setCallback([](const std::string& str)
        {
            qInfo("%s", str.c_str());
        });

        routers.stdErr.setCallback([](const std::string& str)
        {
            qCritical("%s", str.c_str());
        });

        PythonEnvironment::outStreamRouters = routers;
        PythonEnvironment::instance();

        refreshAvailableModules();
        m_editorMode = GlobalSettings::editorMode;

    } catch (exception& e) {
        qCritical() << e.what();
    }
}

pair<int, int> BackendStore::findPort(weak_ptr<PortBase> port) const
{
    // TODO: this is a helper function, basically for BackendInput::value()
    // since there is no other way to get the parentUid and uid by a weak_ptr<PortBase>
    // but to search it from the main m_items list.
    // A mechanism that could store these infos in a weak_ptr<PortBase> would be nice.

    if (!port.lock()) {
        return make_pair(-1, -1);
    }

    pair<int, int> r;
    auto it = find_if(m_items.begin(), m_items.end(),
        [port](const unique_ptr<BackendStoreItem>& item)
        {
            if (item->category() == "input") {
                BackendInput* bi = dynamic_cast<BackendInput*>(item.get());
                if (bi) {
                    return port.lock().get() == bi->source().lock().get();
                }
            }

            if (item->category() == "output") {
                BackendOutput* bo = dynamic_cast<BackendOutput*>(item.get());
                if (bo) {
                    return port.lock().get() == bo->source().lock().get();
                }
            }
            return false;
        });
    
    if (it == m_items.end()) {
        return make_pair(-1, -1);
    }
    return make_pair((*it)->parentUid(), (*it)->uid());
}

BackendStoreItem* BackendStore::getItem(int parentUid, int uid, QString category)
{
    auto it = find_if(m_items.begin(), m_items.end(),
        [parentUid, uid, category](const unique_ptr<BackendStoreItem>& item)
        {
            return (item->category() == category) &&
                ((item->parentUid() < 0 && item->uid() == uid) ||
                (item->parentUid() == parentUid && item->uid() == uid));
        });

    if (it != m_items.end()) {
        // TODO: dont use naked pointer
        return it->get();
    } else {
        return nullptr;
    }
}

void BackendStore::addModule(const QString& scriptPath)
{
    if (!editorMode()) {
        qWarning() << "not in editor mode, can not add module";
        return;
    }

    static const QString nativePath("native://");
    try {
        shared_ptr<ComputeModule> module;
        dirty();

        if (scriptPath.startsWith(nativePath)) {
            QString name = scriptPath.mid(nativePath.size());
            if (name == QString("ics reader")) {
                module = make_shared<IcsDataSourceModule>(m_platform);
            }
        } else {
            std::string script = GlobalSettings::modulePath.filePath(scriptPath).toStdString();
            qInfo() << "loading module" << QString::fromStdString(script);
            ifstream f(script);
            if (!f.is_open()) {
                throw std::runtime_error("missing module script: " + script);
            }
            stringstream buffer;
            buffer << f.rdbuf();

            QString type = pathToModuleName(QFileInfo(scriptPath).fileName());
            QString name = generateModuleName(type);
            module = make_shared<PythonComputeModule>(m_platform, buffer.str(),
                name.toStdString(), type.toStdString());
        }

        if (!module) {
            throw std::runtime_error("unknown module to load: " + scriptPath.toStdString());
        }

        int numRows = 1 + (int)module->numInputs() + (int)module->numOutputs();
        int uid = m_uidCounter++;

        beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + numRows);
        addBackendStoreItem(make_unique<BackendModule>(module, uid, scriptPath));

        for (size_t i = 0; i < module->numInputs(); ++i) {
            auto port = module->inputPort(i);
            if (port.lock()->properties().hasKey("parameter")) {
                addBackendStoreItem(make_unique<BackendParameter>(port, m_platform, i, uid));
            } else {
                addBackendStoreItem(make_unique<BackendInput>(port, i, uid, *this));
            }
        }

        for (size_t i = 0; i < module->numOutputs(); ++i) {
            auto port = module->outputPort(i);
            addBackendStoreItem(make_unique<BackendOutput>(port, m_platform, i, uid));
        }

        endInsertRows();

    } catch (exception& e) {
        qCritical() << e.what();
    }
}

void BackendStore::addBackendStoreItem(std::unique_ptr<BackendStoreItem>&& item)
{
    const BackendStoreItem* p = item.get();
    auto notifyGenerator = [this, p] (ModuleRoles role)
    {
        return [this, p, role] () {
            this->itemChanged(p, role);
        };
    };
    QObject::connect(item.get(), &BackendStoreItem::nameChanged, this,
        notifyGenerator(ModuleRoles::NameRole));
    QObject::connect(item.get(), &BackendStoreItem::statusChanged, this,
        notifyGenerator(ModuleRoles::StatusRole));
    QObject::connect(item.get(), &BackendStoreItem::valueChanged, this,
        notifyGenerator(ModuleRoles::ValueRole));
    QObject::connect(item.get(), &BackendStoreItem::hintsChanged, this,
        notifyGenerator(ModuleRoles::HintsRole));

    m_items.push_back(move(item));
}

void BackendStore::itemChanged(const BackendStoreItem* item, ModuleRoles role)
{
    auto it = find_if(m_items.begin(), m_items.end(),
        [item](const unique_ptr<BackendStoreItem>& x) {
            return x.get() == item;
        });
    if (it != m_items.end()) {
        int row = distance(m_items.begin(), it);
        QModelIndex ix = index(row, 0, QModelIndex());
        dirty();
        Q_EMIT dataChanged(ix, ix, {role});
    }
}

void BackendStore::removeModule(int uid)
{
    if (!editorMode()) {
        qWarning() << "not in editor mode, can not remove module";
        return;
    }

    try {
        dirty();
        auto removable = [uid](const unique_ptr<BackendStoreItem>& item) {
            return (item->category() == "module" && item->uid() == uid)
                || (item->category() != "module" && item->parentUid() == uid);
        };

        int n = 0;
        for (int i = 0; i < (int)m_items.size(); ++i) {
            if (removable(m_items[i])) {
                beginRemoveRows(QModelIndex(), i, i);
                ++n;
            }
        }

        auto newEndIt = remove_if(m_items.begin(), m_items.end(), removable);
        m_items.erase(newEndIt, m_items.end());

        for (int i = 0; i < n; ++i) {
            endRemoveRows();
        }

        // notify all the potential connections
        for (int i = 0; i < (int)m_items.size(); ++i) {
            auto& item = m_items[i];
            if (item->category() == "input") {
                QModelIndex ix = index(i, 0, QModelIndex());
                Q_EMIT dataChanged(ix, ix, {BackendStore::ValueRole});
            }
        }

    } catch (exception& e) {
        qCritical() << e.what();
    }
}

QHash<int, QByteArray> BackendStore::roleNames() const
{
    static QHash<int, QByteArray> roles = {
        {UidRole, "uid"},
        {ParentUidRole, "parentUid"},
        {CategoryRole, "category"},
        {NameRole, "name"},
        {TypeRole, "type"},
        {StatusRole, "status"},
        {HintsRole, "hints"},
        {ValueRole, "value"}
    };
    return roles;
}

QVariant BackendStore::data(const QModelIndex& index, int role) const
{
    try {
        if (!index.isValid()) {
            return QVariant();
        }

        if (index.row() < 0 || index.row() >= (int)m_items.size()) {
            return QVariant();
        }

        auto& item = m_items[index.row()];

        switch (role) {
            case UidRole: return item->uid();
            case ParentUidRole: return item->parentUid();
            case CategoryRole: return item->category();
            case NameRole: return item->name();
            case TypeRole: return item->type();
            case StatusRole: return item->status();
            case HintsRole: return item->hints();
            case ValueRole: return item->value();
            default: return QVariant();
        }
    } catch (exception& e) {
        qCritical() << e.what();
    }
    return QVariant();
}

bool BackendStore::setData(const QModelIndex &index, const QVariant &value, int role)
{
    try {
        if (!index.isValid()) {
            return false;
        }

        if (index.row() < 0 || index.row() >= (int)m_items.size()) {
            return false;
        }

        dirty();
        auto& item = m_items[index.row()];
        switch (role) {
            case NameRole: {
                if (value.canConvert<QString>()) {
                    item->setName(value.toString());
                    return true;
                } else {
                    qWarning() << "invalid value for setting the name of '" + item->name() + "'";
                    return false;
                }
            }
            case StatusRole: {
                if (value.canConvert<int>()) {
                    item->setStatus(value.toInt());
                    return true;
                } else {
                    qWarning() << "invalid value for setting the status of '" + item->name() + "'";
                    return false;
                }
            }
            case ValueRole: {
                bool b = item->setValue(value);
                return b;
            }
            default: return false;
        }
    } catch (exception& e) {
        qCritical() << e.what();
    }
    return false;
}

int BackendStore::rowCount(const QModelIndex&) const
{
    return m_items.size();
}

QVariant BackendStore::get(int row) const
{
    try {
        if (row >= 0 && row < (int)m_items.size()) {
            return QVariant::fromValue(m_items[row].get());
        } else {
            return QVariant();
        }
    } catch (exception& e) {
        qCritical() << e.what();
    }
    return QVariant();
}

int BackendStore::count() const
{
    return rowCount(QModelIndex());
}

bool BackendStore::connect(int outModuleUid, int outPortUid, int inModuleUid, int inPortUid)
{
    if (!editorMode()) {
        qWarning() << "not in editor mode, can connect modules";
        return false;
    }

    try {
        auto b = m_items.begin();
        auto e = m_items.end();

        auto outIt = find_if(b, e,
            [outModuleUid, outPortUid] (const unique_ptr<BackendStoreItem>& item) {
                return item->uid() == outPortUid
                    && item->parentUid() == outModuleUid
                    && item->category() == "output";
            });

        auto inIt = find_if(b, e,
            [inModuleUid, inPortUid] (const unique_ptr<BackendStoreItem>& item) {
                return item->uid() == inPortUid
                    && item->parentUid() == inModuleUid
                    && item->category() == "input";
            });

        bool valid = (outIt != e) && (inIt != e) &&
            (*outIt)->category() == "output" && (*inIt)->category() == "input";

        if (!valid) {
            // TODO: proper message, catch throw
            throw std::runtime_error("no port to connect");
        }

        BackendOutput* out = dynamic_cast<BackendOutput*>((*outIt).get());
        BackendInput* in = dynamic_cast<BackendInput*>((*inIt).get());
        bool canConvert = out && in;

        if (!canConvert) {
            // TODO: proper message, catch throw
            throw std::runtime_error("invalid ports to connect");
        }

        bool success = out->source().lock()->bind(in->source());
        if (success) {
            dirty();
            Q_EMIT in->valueChanged();
        }
        return success;
    } catch (exception& e) {
        qCritical() << e.what();
    }
    return false;
}

void BackendStore::evaluate(int uid)
{
    try {
        if (uid == -1) {
            m_platform.printModuleConnections();
            m_platform.run();
        } else {
            // TODO:
        }
    } catch (exception& e) {
        qCritical() << e.what();
    }
}

QVariantList BackendStore::availableModules() const
{
    return m_availableModules;
}

void BackendStore::refreshAvailableModules()
{
    try {
        QVariantList vlist;
        auto isModuleFile = [](const QFileInfo& f)
        {
            return f.isFile()
                && f.fileName().toLower().startsWith("module")
                && f.suffix().toLower() == "py";
        };

        // TODO: move magic string constants to highlighted section/file
        QDirIterator level1(GlobalSettings::modulePath.absolutePath());
        while (level1.hasNext()) {
            level1.next();
            if (level1.fileInfo().isDir() && !level1.fileInfo().isHidden()
            && level1.fileName() != "." && level1.fileName() != "..") {

                QVariantMap vmap;
                vmap["name"] = level1.fileName();
                QVariantList fileList;
                QDirIterator level2(level1.filePath(), QDirIterator::Subdirectories);
                while (level2.hasNext()) {
                    level2.next();
                    if (isModuleFile(level2.fileInfo() )) {
                        QVariantMap fileVMap;
                        fileVMap["path"] = GlobalSettings::modulePath
                            .relativeFilePath(level2.fileInfo().absoluteFilePath());
                        QString name = pathToModuleName(level2.fileName());
                        fileVMap["name"] = name;
                        fileList.append(fileVMap);
                    }
                }
                if (!fileList.empty()) {
                    vmap["files"] = fileList;
                    vlist.append(vmap);
                }
            }
        }

        m_availableModules = vlist;

        addAvailableNativeModules();

        Q_EMIT(availableModulesChanged());
    } catch (exception& e) {
        qCritical() << e.what();
    }
}

void BackendStore::addAvailableNativeModules()
{
    QVariantMap groupMap;
    QVariantMap fileMap;
    QVariantList fileList;
    fileMap["name"] = QString("ics reader");
    fileMap["path"] = QString("native://ics reader");
    fileList.append(fileMap);
    groupMap["name"] = QString("general");
    groupMap["files"] = fileList;

    m_availableModules.append(groupMap);
}

void BackendStore::newWorkflow()
{
    beginResetModel();
    try {
        m_items.clear();
        ComputePlatform cleanPlatform;
        swap(m_platform, cleanPlatform);
        m_unsaved = false;
    } catch (exception& e) {
        qCritical() << e.what();
    }
    endResetModel();
}

void BackendStore::readWorkflow(const QUrl& url)
{
    try {
        newWorkflow();
        BackendStoreSerializer ser;
        ser.read(url.toLocalFile(), *this);
        m_unsaved = false;
    } catch (exception& e) {
        qCritical() << e.what();
    }
}

void BackendStore::writeWorkflow(const QUrl& url)
{
    try {
        BackendStoreSerializer ser;
        ser.write(url.toLocalFile(), *this);
        m_unsaved = false;
    } catch (exception& e) {
        qCritical() << e.what();
    }
}

// TODO: move to separate file
BackendStoreFilter::BackendStoreFilter(QObject* parent)
    :  QSortFilterProxyModel(parent)
{}

bool BackendStoreFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    try {
        QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
        if (!index.isValid()) {
            return false;
        }

        vector<tuple<const QList<int>*, BackendStore::ModuleRoles, bool>> intTri = {
            make_tuple(&m_includeUid, BackendStore::UidRole, true),
            make_tuple(&m_excludeUid, BackendStore::UidRole, false),
            make_tuple(&m_includeParentUid, BackendStore::ParentUidRole, true),
            make_tuple(&m_excludeParentUid, BackendStore::ParentUidRole, false),
            make_tuple(&m_includeStatus, BackendStore::StatusRole, true),
            make_tuple(&m_excludeStatus, BackendStore::StatusRole, false)
        };

        for (auto& tri: intTri) {
            if (std::get<0>(tri)->empty()) {
                continue;
            }
            int value = sourceModel()->data(index, std::get<1>(tri)).toInt();
            // "contains?" XOR "should be contained?"
            if ((bool)std::get<0>(tri)->contains(value) != (bool)std::get<2>(tri)) {
                return false;
            }
        }

        vector<tuple<const QList<QString>*, BackendStore::ModuleRoles, bool>> stringTri = {
            make_tuple(&m_includeCategory, BackendStore::CategoryRole, true),
            make_tuple(&m_excludeCategory, BackendStore::CategoryRole, false),
            make_tuple(&m_includeType, BackendStore::TypeRole, true),
            make_tuple(&m_excludeType, BackendStore::TypeRole, false)
        };

        for (auto& tri: stringTri) {
            if (std::get<0>(tri)->empty()) {
                continue;
            }
            QString value = sourceModel()->data(index, std::get<1>(tri)).toString();
            // "contains?" XOR "should be contained?"
            if ((bool)std::get<0>(tri)->contains(value) != (bool)std::get<2>(tri)) {
                return false;
            }
        }

        return true;
    } catch (exception& e) {
        qCritical() << e.what();
    }
    return false;
}

BackendStore* BackendStoreFilter::sourceStore() const
{
    return m_store;
}

void BackendStoreFilter::setSourceStore(BackendStore* store)
{
    try {
        if (m_store != store) {
            m_store = store;
            setSourceModel(m_store);
        }
    } catch (exception& e) {
        qCritical() << e.what();
    }
}

QList<int> BackendStoreFilter::includeUid() const
{
    return m_includeUid;
}

QList<int> BackendStoreFilter::excludeUid() const
{
    return m_excludeUid;
}

QList<int> BackendStoreFilter::includeParentUid() const
{
    return m_includeParentUid;
}

QList<int> BackendStoreFilter::excludeParentUid() const
{
    return m_excludeParentUid;
}

QList<int> BackendStoreFilter::includeStatus() const
{
    return m_includeStatus;
}

QList<int> BackendStoreFilter::excludeStatus() const
{
    return m_excludeStatus;
}

QList<QString> BackendStoreFilter::includeCategory() const
{
    return m_includeCategory;
}

QList<QString> BackendStoreFilter::excludeCategory() const
{
    return m_excludeCategory;
}

QList<QString> BackendStoreFilter::includeType() const
{
    return m_includeType;
}

QList<QString> BackendStoreFilter::excludeType() const
{
    return m_excludeType;
}

void BackendStoreFilter::setIncludeUid(QList<int> list)
{
    m_includeUid = list;
    invalidateFilter();
    Q_EMIT firstChanged();
}

void BackendStoreFilter::setExcludeUid(QList<int> list)
{
    m_excludeUid = list;
    invalidateFilter();
    Q_EMIT firstChanged();
}

void BackendStoreFilter::setIncludeParentUid(QList<int> list)
{
    m_includeParentUid = list;
    invalidateFilter();
    Q_EMIT firstChanged();
}

void BackendStoreFilter::setExcludeParentUid(QList<int> list)
{
    m_excludeParentUid = list;
    invalidateFilter();
    Q_EMIT firstChanged();
}

void BackendStoreFilter::setIncludeStatus(QList<int> list)
{
    m_includeStatus = list;
    invalidateFilter();
    Q_EMIT firstChanged();
}

void BackendStoreFilter::setExcludeStatus(QList<int> list)
{
    m_excludeStatus = list;
    invalidateFilter();
    Q_EMIT firstChanged();
}

void BackendStoreFilter::setIncludeCategory(QList<QString> list)
{
    m_includeCategory = list;
    invalidateFilter();
    Q_EMIT firstChanged();
}

void BackendStoreFilter::setExcludeCategory(QList<QString> list)
{
    m_excludeCategory = list;
    invalidateFilter();
    Q_EMIT firstChanged();
}

void BackendStoreFilter::setIncludeType(QList<QString> list)
{
    m_includeType = list;
    invalidateFilter();
    Q_EMIT firstChanged();
}

void BackendStoreFilter::setExcludeType(QList<QString> list)
{
    m_excludeType = list;
    invalidateFilter();
    Q_EMIT firstChanged();
}

QVariant BackendStoreFilter::get(int row) const
{
    try {
        if (m_store == nullptr) {
            return QVariant();
        }

        QModelIndex proxyIndex = index(row, 0, QModelIndex());
        if (!proxyIndex.isValid()) {
            return QVariant();
        }

        QModelIndex sourceIndex = mapToSource(proxyIndex);
        return m_store->get(sourceIndex.row());
    } catch (exception& e) {
        qCritical() << e.what();
    }
    return QVariant();
}

int BackendStoreFilter::count() const
{
    return rowCount(QModelIndex());
}

QVariant BackendStoreFilter::first() const
{
    return get(0);
}
