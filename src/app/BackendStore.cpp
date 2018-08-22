#include "BackendStore.h"

#include <tuple>
#include <vector>
#include <QDebug>
#include <core/high_platform/PythonComputeModule.h>
#include "BackendModule.h"
#include "BackendInput.h"
#include "BackendParameter.h"
#include "BackendOutput.h"
#include <fstream>
#include <algorithm>

using namespace std;
using namespace core::high_platform;

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

void BackendStore::addModule(const QString& scriptPath)
{
    try {
        std::string script = scriptPath.toStdString();
        ifstream f(script);
        if (!f.is_open()) {
            throw std::runtime_error("missing module script: " + script);
        }
        stringstream buffer;
        buffer << f.rdbuf();

        auto pyModule = make_shared<PythonComputeModule>(m_platform, buffer.str(), "Generic");
        int numRows = 1 + (int)pyModule->numInputs() + (int)pyModule->numOutputs();
        int uid = m_uidCounter++;

        beginInsertRows(QModelIndex(), m_items.size(), m_items.size() + numRows);
        addBackendStoreItem(make_unique<BackendModule>(pyModule, uid));

        for (size_t i = 0; i < pyModule->numInputs(); ++i) {
            auto port = pyModule->inputPort(i);
            if (port.lock()->properties().hasKey("parameter")) {
                addBackendStoreItem(make_unique<BackendParameter>(port, m_platform, i, uid));
            } else {
                addBackendStoreItem(make_unique<BackendInput>(port, i, uid, *this));
            }
        }

        for (size_t i = 0; i < pyModule->numOutputs(); ++i) {
            auto port = pyModule->outputPort(i);
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
        Q_EMIT dataChanged(ix, ix, {role});
    }
}

void BackendStore::removeModule(int uid)
{
    try {
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

QVariant BackendStore::get(int row)
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
        QDirIterator level1("modules");
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
                        fileVMap["path"] = level2.filePath();
                        QString name = level2.fileName()
                            .mid(QString("module").size())
                            .replace('_', ' ')
                            .simplified();
                        name.chop(3);
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
        Q_EMIT(availableModulesChanged());
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
