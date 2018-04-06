#include "ModulePlatformBackend.h"

#include <core/io_utils/IcsAdapter.h>
#include "VolumeTexture.h"
#include "VolumeData.h"
#include <core/high_platform/PythonComputeModule.h>

using namespace std;
using namespace core::io_utils;
using namespace core::multidim_image_platform;
using namespace core::high_platform;

BackendModule::BackendModule(uint32_t uid) : m_uid(uid)
{}

uint32_t BackendModule::uid() const
{
    return m_uid;
}

// --------------------------------------------------------

DataSourceModule::DataSourceModule(cp::ComputePlatform& parent, uint32_t uid)
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
    map["name"] = QVariant(QString::fromStdString("DataSource" + to_string(uid())));
    return map;
}

bool DataSourceModule::hasTexture(std::size_t outputPortId)
{
    return outputPortId == 0;
}

VolumeTexture* DataSourceModule::getModuleTexture(std::size_t outputPortId)
{
    VolumeTexture* tex = new VolumeTexture;
    tex->init(*m_data, outputPortId);
    return tex;
}

cp::ComputeModule& DataSourceModule::getModule()
{
    return *this;
}

// --------------------------------------------------------

GenericModule::GenericModule(cp::ComputePlatform& parent, const std::string& script, uint32_t uid)
    : hp::PythonComputeModule(parent, script),
    BackendModule(uid)
{}

QVariantMap GenericModule::getProperties()
{
    QVariantMap map;
    map["name"] = QVariant(QString::fromStdString("GenericModule" + to_string(uid())));
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

QList<uint32_t> ModulePlatformBackend::createSourceModulesFromIcsFile(const QUrl& filename)
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

    QList<uint32_t> uids;
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

uint32_t ModulePlatformBackend::createGenericModule(const QUrl& scriptPath)
{
    const string path = scriptPath.toLocalFile().toStdString();
    auto newModule = new GenericModule(m_platform, path,nextUid());

    m_modules.emplace(make_pair(newModule->uid(), newModule));
    Q_EMIT moduleCreated(newModule->uid());
    return newModule->uid();
}

bool ModulePlatformBackend::hasModule(uint32_t uid)
{
    return m_modules.end() != m_modules.find(uid);
}

void ModulePlatformBackend::destroyModule(uint32_t uid)
{
    if (!hasModule(uid)) {
        return;
    }

    Q_EMIT moduleWillBeDestroyed(uid);

    // TODO: remove the module -> implement ComputePlatform::removeModule + its test

    Q_EMIT moduleDestroyed(uid);
}

QVariantMap ModulePlatformBackend::getModuleProperties(uint32_t uid)
{
    if (hasModule(uid)) {
        return m_modules[uid]->getProperties();
    }
    return QVariantMap();
}

VolumeTexture* ModulePlatformBackend::getModuleTexture(uint32_t uid, std::size_t outputPortId)
{
    if (hasModule(uid) && m_modules[uid]->hasTexture(outputPortId)) {
        return m_modules[uid]->getModuleTexture(outputPortId);
    }
    return nullptr;
}

QMultiMap<uint32_t, size_t> ModulePlatformBackend::getCompatibleModules(uint32_t uid, std::size_t inputPortId)
{
    QMultiMap<uint32_t, size_t> ports;
    
    if (!hasModule(uid)) {
        return ports;
    }

    cp::ComputeModule& q = m_modules[uid]->getModule();

    if (inputPortId >= q.numInputs()) {
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
            if (connectPorts(m, i, q, inputPortId)) {
                disconnectPorts(m, i, q, inputPortId);
                ports.insert(pr.first, i);
            }
        }
    }

    return ports;
}

inline uint32_t ModulePlatformBackend::nextUid() const
{
    static uint32_t counter = 0;
    return counter++;
}
