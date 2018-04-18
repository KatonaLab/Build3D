#include "ModulePlatformBackend.h"

#include <core/io_utils/IcsAdapter.h>
#include <fstream>
#include <sstream>
#include "VolumeTexture.h"
#include "VolumeData.h"
#include <core/high_platform/PythonComputeModule.h>

using namespace std;
using namespace core::io_utils;
using namespace core::compute_platform;
using namespace core::multidim_image_platform;
using namespace core::high_platform;

BackendModule::BackendModule(int uid, const std::string& name)
    : m_uid(uid), m_name(name)
{}

int BackendModule::uid() const
{
    return m_uid;
}

std::string BackendModule::name() const
{
    return m_name;
}

// --------------------------------------------------------

DataSourceModule::DataSourceModule(cp::ComputePlatform& parent, int uid)
    : cp::ComputeModule(parent, m_inputs, m_outputs),
    BackendModule(uid, "DataSource" + to_string(uid)),
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

cp::ComputeModule& DataSourceModule::getComputeModule()
{
    return *this;
}

// --------------------------------------------------------

GenericModule::GenericModule(cp::ComputePlatform& parent, const std::string& script, int uid)
    : hp::PythonComputeModule(parent, script),
    BackendModule(uid, "Generic" + to_string(uid))
{
    // TODO: remove or move to qDebug output
    std::cout << script << std::endl;
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

cp::ComputeModule& GenericModule::getComputeModule()
{
    return *this;
}

// --------------------------------------------------------

QList<int> PrivateModulePlatformBackend::createSourceModulesFromIcsFile(const QUrl& filename)
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
    }

    return uids;
}

int PrivateModulePlatformBackend::createGenericModule(const QString& scriptPath)
{
    string path = scriptPath.toStdString();

    ifstream f(path);
    stringstream buffer;
    buffer << f.rdbuf();

    // TODO: remove or put to qDebug
    std::cout << buffer.str() << std::endl;

    auto newModule = new GenericModule(m_platform, buffer.str(), nextUid());

    m_modules.emplace(make_pair(newModule->uid(), newModule));
    return newModule->uid();
}

bool PrivateModulePlatformBackend::hasModule(int uid)
{
    return m_modules.end() != m_modules.find(uid);
}

void PrivateModulePlatformBackend::destroyModule(int uid)
{
    if (!hasModule(uid)) {
        return;
    }

    // TODO: remove the module -> implement ComputePlatform::removeModule + its test
}

VolumeTexture* PrivateModulePlatformBackend::getModuleTexture(int uid, int outputPortId)
{
    if (hasModule(uid) && m_modules[uid]->hasTexture((std::size_t)outputPortId)) {
        return m_modules[uid]->getModuleTexture((std::size_t)outputPortId);
    }
    return nullptr;
}

QVariantList PrivateModulePlatformBackend::getInputOptions(int uid, int inputPortId)
{
    auto portType = getInputPort(uid, inputPortId).lock()->typeHash();
    QVariantList vlist;

    for (auto& pr : m_modules) {
        auto& m = pr.second;
        for (std::size_t i = 0; i < m->getComputeModule().numOutputs(); ++i) {
            auto p = m->getComputeModule().outputPort(i).lock();
            if (p->typeHash() == portType) {
                QVariantMap vmap;
                vmap["uid"] = pr.first;
                vmap["portId"] = (int)i;
                vmap["displayName"] = QString::fromStdString(m->name() + "/" + p->name());
                vlist.append(QVariant(vmap));
            }
        }
    }

    return vlist;
}

bool PrivateModulePlatformBackend::connectInputOutput(int outputModuleUid, int outputPortId,
    int inputModuleUid, int inputPortId)
{
    auto input = getInputPort(inputModuleUid, inputPortId);
    auto output = getOutputPort(outputModuleUid, outputPortId).lock();
    return output->bind(input);
}

void PrivateModulePlatformBackend::disconnectInput(int inputModuleUid, int inputPortId)
{
    auto input = getInputPort(inputModuleUid, inputPortId);
    input.lock()->getSource().lock()->unbind(input);
}

QVariantList PrivateModulePlatformBackend::getInputs(int uid)
{
    BackendModule& m = getBackendModule(uid);
    QVariantList vlist;

    for (size_t i = 0; i < m.getComputeModule().numInputs(); ++i) {
        auto p = m.getComputeModule().inputPort(i).lock();
        string tag = p->tag();
        if (tag.empty() || tag.find("regular") != string::npos) {
            QVariantMap vmap;
            vmap["displayName"] = QString::fromStdString(p->name());
            vmap["portIndex"] = (int)i;
            vlist.append(vmap);
        }
    }

    return vlist;
}

QVariantList PrivateModulePlatformBackend::getParameters(int uid)
{
    BackendModule& m = getBackendModule(uid);
    QVariantList vlist;

    for (size_t i = 0; i < m.getComputeModule().numInputs(); ++i) {
        auto p = m.getComputeModule().inputPort(i).lock();
        string tag = p->tag();
        if (tag.find("parameter") != string::npos) {
            QVariantMap vmap;
            vmap["displayName"] = QString::fromStdString(p->name());
            vmap["portIndex"] = (int)i;
            vmap["hint"] = QString("TODO: hint");
            vlist.append(vmap);
        }
    }
    
    return vlist;
}

void PrivateModulePlatformBackend::setParameter(int uid, int paramId, QVariant value)
{
    // TODO:
}

QVariantList PrivateModulePlatformBackend::getOutputs(int uid)
{
    BackendModule& m = getBackendModule(uid);
    QVariantList vlist;

    for (size_t i = 0; i < m.getComputeModule().numOutputs(); ++i) {
        auto p = m.getComputeModule().outputPort(i);
        QVariantMap vmap;
        vmap["displayName"] = QString::fromStdString(p.lock()->name());
        vmap["portIndex"] = (int)i;
        vlist.append(vmap);
    }

    return vlist;
}

int PrivateModulePlatformBackend::nextUid() const
{
    static int counter = 1;
    return counter++;
}

BackendModule& PrivateModulePlatformBackend::getBackendModule(int uid)
{
    if(!hasModule(uid)) {
        throw std::runtime_error("backend: no module with uid " + to_string(uid));
    }

    return *m_modules[uid];
}

std::weak_ptr<InputPort> PrivateModulePlatformBackend::getInputPort(int uid, int portId)
{
    auto& m = getBackendModule(uid);
    if (m.getComputeModule().numInputs() <= portId) {
        throw std::runtime_error("backend: no input port with id " + to_string(portId) + " at module uid " + to_string(uid));
    }
    return m.getComputeModule().inputPort(portId);
}

std::weak_ptr<OutputPort> PrivateModulePlatformBackend::getOutputPort(int uid, int portId)
{
    auto& m = getBackendModule(uid);
    if (m.getComputeModule().numOutputs() <= portId) {
        throw std::runtime_error("backend: no output port with id " + to_string(portId) + " at module uid " + to_string(uid));
    }
    return m.getComputeModule().outputPort(portId);
}
