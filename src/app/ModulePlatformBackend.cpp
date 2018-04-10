#include "ModulePlatformBackend.h"

#include <core/io_utils/IcsAdapter.h>
#include <fstream>
#include <sstream>
#include "VolumeTexture.h"
#include "VolumeData.h"
#include <core/high_platform/PythonComputeModule.h>

using namespace std;
using namespace core::io_utils;
using namespace core::multidim_image_platform;
using namespace core::high_platform;

BackendModule::BackendModule(int uid) : m_uid(uid)
{}

int BackendModule::uid() const
{
    return m_uid;
}

// --------------------------------------------------------

DataSourceModule::DataSourceModule(cp::ComputePlatform& parent, int uid)
    : cp::ComputeModule(parent, m_inputs, m_outputs),
    BackendModule(uid),
    m_inputs(*this),
    m_outputs(*this)
{}

void DataSourceModule::execute()
{
    m_outputs.output<0>()->forwardFromSharedPtr(m_data);
}

void DataSourceModule::setData(std::shared_ptr<md::MultiDimImage<float>> data)
{
    m_data = data;
}

QVariantMap DataSourceModule::getProperties()
{
    QVariantMap map;
    map["displayName"] = QVariant(QString::fromStdString("DataSource" + to_string(uid())));
    map["inputs"] = QVariantList();
    map["parameters"] = QVariantList();

    QVariantList outputList;
    {
        QVariantMap out;
        out["displayName"] = QVariant("output");
        out["type"] = QVariant("volume");
        outputList.append(out);
    }
    map["outputs"] = outputList;

    return map;
}

bool DataSourceModule::hasTexture(std::size_t outputPortId)
{
    return outputPortId == 0;
}

VolumeTexture* DataSourceModule::getModuleTexture(std::size_t outputPortId)
{
    VolumeTexture* tex = new VolumeTexture;
    tex->init(*m_data);
    return tex;
}

cp::ComputeModule& DataSourceModule::getModule()
{
    return *this;
}

// --------------------------------------------------------

GenericModule::GenericModule(cp::ComputePlatform& parent, const std::string& script, int uid)
    : hp::PythonComputeModule(parent, script),
    BackendModule(uid)
{
    std::cout << script << std::endl;
}

QVariantMap GenericModule::getProperties()
{
    QVariantMap map;
    map["displayName"] = QVariant(QString::fromStdString("GenericModule" + to_string(uid())));
    map["inputs"] = QVariantList();
    map["parameters"] = QVariantList();
    map["outputs"] = QVariantList();

    {
        std::cout << numInputs() << " " << numOutputs() << std::endl;
        QVariantList vlist;
        for (size_t i = 0; i < numInputs(); ++i) {
            QVariantMap vmap;
            auto portName = inputPort(i).lock()->name();
            PyTypes portType = inputPortPyType(portName);
            vmap["displayName"] = QVariant(QString::fromStdString(portName));
            if (portType == PyTypes::TYPE_MultiDimImageFloat) {
                vmap["type"] = QVariant("volume");
            } else {
                vmap["type"] = QVariant("?");
            }
            vlist.append(vmap);
        }
        map["inputs"] = vlist;
    }

    {
        QVariantList vlist;
        for (size_t i = 0; i < numOutputs(); ++i) {
            QVariantMap vmap;
            auto portName = outputPort(i).lock()->name();
            PyTypes portType = outputPortPyType(portName);
            vmap["displayName"] = QVariant(QString::fromStdString(portName));
            if (portType == PyTypes::TYPE_MultiDimImageFloat) {
                vmap["type"] = QVariant("volume");
            } else {
                vmap["type"] = QVariant("?");
            }
            vlist.append(vmap);
        }
        map["outputs"] = QVariant(vlist);
    }

    map["parameters"] = QVariantList();

    return map;
}

bool GenericModule::hasTexture(std::size_t outputPortId)
{
    // TODO
    return false;
}

VolumeTexture* GenericModule::getModuleTexture(std::size_t outputPortId)
{
    // TODO
    return nullptr;
}

cp::ComputeModule& GenericModule::getModule()
{
    return *this;
}

// --------------------------------------------------------

ModulePlatformBackend::ModulePlatformBackend(QObject* parent)
: QObject(parent)
{}

QList<int> ModulePlatformBackend::createSourceModulesFromIcsFile(const QUrl& filename)
{
    IcsAdapter ics;
    ics.open(filename.toLocalFile().toStdString());
    auto image = ics.readScaledConvert<float>(true);

    vector<MultiDimImage<float>> volumes;

    switch (image.dims()) {
        case 3: // xyz
            volumes.push_back(image);
            break;
        case 5: // xyztc
            // removing dimension 't'
            image.removeDims({3});
            // no break - let it flow
        case 4: // xyzc or xyzt
            volumes = image.splitDim(3);
            break;
        // TODO: support XY images too
        default:
            // TODO: proper error message to GUI
            throw std::runtime_error("ICS files with dimensions XYZ or XYZC are supported only");
    };

    QList<int> uids;
    for (auto& vol : volumes) {
        uids.push_back(nextUid());
        auto newModule = new DataSourceModule(m_platform, uids.back());
        // TODO: try to use move semantics to make a shared_ptr out of vol
        auto p = make_shared<MultiDimImage<float>>();
        swap(*p, vol);
        newModule->setData(p);

        m_modules.emplace(make_pair(newModule->uid(), newModule));
        Q_EMIT moduleCreated(newModule->uid());
    }

    return uids;
}

int ModulePlatformBackend::createGenericModule(const QString& scriptPath)
{
    string path = scriptPath.toStdString();

    ifstream f(path);
    stringstream buffer;
    buffer << f.rdbuf();

    std::cout << buffer.str() << std::endl;

    auto newModule = new GenericModule(m_platform, buffer.str(), nextUid());

    m_modules.emplace(make_pair(newModule->uid(), newModule));
    Q_EMIT moduleCreated(newModule->uid());
    return newModule->uid();
}

bool ModulePlatformBackend::hasModule(int uid)
{
    return m_modules.end() != m_modules.find(uid);
}

void ModulePlatformBackend::destroyModule(int uid)
{
    if (!hasModule(uid)) {
        return;
    }

    Q_EMIT moduleWillBeDestroyed(uid);

    // TODO: remove the module -> implement ComputePlatform::removeModule + its test

    Q_EMIT moduleDestroyed(uid);
}

QVariantMap ModulePlatformBackend::getModuleProperties(int uid)
{
    if (hasModule(uid)) {
        return m_modules[uid]->getProperties();
    }
    return QVariantMap();
}

VolumeTexture* ModulePlatformBackend::getModuleTexture(int uid, int outputPortId)
{
    if (hasModule(uid) && m_modules[uid]->hasTexture((std::size_t)outputPortId)) {
        return m_modules[uid]->getModuleTexture((std::size_t)outputPortId);
    }
    return nullptr;
}

QMultiMap<int, size_t> ModulePlatformBackend::getCompatibleModules(int uid, int inputPortId)
{
    QMultiMap<int, size_t> ports;
    
    if (!hasModule(uid)) {
        return ports;
    }

    cp::ComputeModule& q = m_modules[uid]->getModule();

    if ((std::size_t)inputPortId >= q.numInputs()) {
        return ports;
    }

    for (auto& pr : m_modules) {
        if (uid == pr.first) {
            continue;
        }

        cp::ComputeModule& m = pr.second->getModule();
        for (size_t i = 0; i < m.numOutputs(); ++i) {
            // TODO: it is not the most elegant way for the connection test
            // try to find a better solution
            if (connectPorts(m, i, q, (std::size_t)inputPortId)) {
                disconnectPorts(m, i, q, (std::size_t)inputPortId);
                ports.insert(pr.first, i);
            }
        }
    }

    return ports;
}

int ModulePlatformBackend::nextUid() const
{
    static int counter = 0;
    return counter++;
}
