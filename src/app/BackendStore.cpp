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

using namespace std;
using namespace core::high_platform;

BackendStore::BackendStore(QObject* parent)
    : QAbstractListModel(parent)
{
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
}

void BackendStore::addModule(const QString& scriptPath)
{
    std::string script = "modules/examples/module_hello_parameters_module.py";
    
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
    m_items.push_back(make_unique<BackendModule>(pyModule, uid));

    for (size_t i = 0; i < pyModule->numInputs(); ++i) {
        auto port = pyModule->inputPort(i);
        if (port.lock()->properties().hasKey("parameter")) {
            m_items.push_back(make_unique<BackendParameter>(port, i, uid));
        } else {
            m_items.push_back(make_unique<BackendInput>(port, i, uid));
        }
    }

    for (size_t i = 0; i < pyModule->numOutputs(); ++i) {
        auto port = pyModule->outputPort(i);
        m_items.push_back(make_unique<BackendOutput>(port, i, uid));
    }

    endInsertRows();
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
        {ValueRole, "value"}
    };
    return roles;
}

QVariant BackendStore::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
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
        case ValueRole: return item->value();
        default: return QVariant();
    }
}

int BackendStore::rowCount(const QModelIndex& parent) const
{
    return m_items.size();
}

QVariant BackendStore::get(int row)
{
    if (row >= 0 && row < (int)m_items.size()) {
        return QVariant::fromValue(m_items[row].get());
    } else {
        return QVariant();
    }
}

int BackendStore::count() const
{
    return rowCount(QModelIndex());
}

BackendStoreFilter::BackendStoreFilter(QObject* parent)
    :  QSortFilterProxyModel(parent)
{}

bool BackendStoreFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    if (!index.isValid()) {
        return false;
    }

    vector<tuple<const QList<int>*, BackendStore::ModuleRoles, bool>> intTri = {
        make_tuple(&m_includeUid, BackendStore::UidRole, true),
        make_tuple(&m_excludeUid, BackendStore::UidRole, false),
        make_tuple(&m_includeParentUid, BackendStore::ParentUidRole, true),
        make_tuple(&m_excludeParentUid, BackendStore::ParentUidRole, false)
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
}

BackendStore* BackendStoreFilter::sourceStore() const
{
    return m_store;
}

void BackendStoreFilter::setSourceStore(BackendStore* store)
{
    if (m_store) {
        m_store->disconnect(this);
    }

    if (m_store != store) {
        m_store = store;
        setSourceModel(m_store);

        // TODO: optimize signal emission and invalidate filter calls

        QObject::connect(m_store, &BackendStore::dataChanged, [=](const QModelIndex& topLeft,
            const QModelIndex& bottomRight, const QVector<int>& roles)
        {
            this->invalidateFilter();
            Q_EMIT firstChanged();
        });

        QObject::connect(m_store, &BackendStore::modelReset, [=]()
        {
            this->invalidateFilter();
            Q_EMIT firstChanged();
        });

        QObject::connect(m_store, &BackendStore::rowsInserted, [=](const QModelIndex& parent,
            int first, int last)
        {
            this->invalidateFilter();
            Q_EMIT firstChanged();
        });

        QObject::connect(m_store, &BackendStore::rowsMoved, [=](const QModelIndex& parent, int start,
            int end, const QModelIndex& destination, int row)
        {
            this->invalidateFilter();
            Q_EMIT firstChanged();
        });

        QObject::connect(m_store, &BackendStore::rowsRemoved, [=](const QModelIndex& parent,
            int first, int last)
        {
            this->invalidateFilter();
            Q_EMIT firstChanged();
        });
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
}

void BackendStoreFilter::setExcludeUid(QList<int> list)
{
    m_excludeUid = list;
}

void BackendStoreFilter::setIncludeParentUid(QList<int> list)
{
    m_includeParentUid = list;
}

void BackendStoreFilter::setExcludeParentUid(QList<int> list)
{
    m_excludeParentUid = list;
}

void BackendStoreFilter::setIncludeCategory(QList<QString> list)
{
    m_includeCategory = list;
}

void BackendStoreFilter::setExcludeCategory(QList<QString> list)
{
    m_excludeCategory = list;
}

void BackendStoreFilter::setIncludeType(QList<QString> list)
{
    m_includeType = list;
}

void BackendStoreFilter::setExcludeType(QList<QString> list)
{
    m_excludeType = list;
}

QVariant BackendStoreFilter::get(int row) const
{
    if (m_store == nullptr) {
        return QVariant();
    }

    QModelIndex proxyIndex = index(row, 0, QModelIndex());
    if (!proxyIndex.isValid()) {
        return QVariant();
    }

    QModelIndex sourceIndex = mapToSource(proxyIndex);
    return m_store->get(sourceIndex.row());
}

int BackendStoreFilter::count() const
{
    return rowCount(QModelIndex());
}

QVariant BackendStoreFilter::first() const
{
    return get(0);
}