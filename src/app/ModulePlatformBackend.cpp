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

cp::ComputeModule& DataSourceModule::getComputeModule()
{
    return *this;
}

// --------------------------------------------------------

GenericModule::GenericModule(cp::ComputePlatform& parent, const std::string& script, int uid)
    : hp::PythonComputeModule(parent, script),
    BackendModule(uid, "Generic" + to_string(uid))
{}

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

QList<int> PrivateModulePlatformBackend::enumeratePorts(
    int uid,
    std::function<std::size_t(ComputeModule&)> numInputsFunc,
    std::function<bool(ComputeModule&, std::size_t)> predFunc)
{
    ComputeModule& m = fetchBackendModule(uid).getComputeModule();
    QList<int> list;
    size_t k = numInputsFunc(m);
    for (size_t i = 0; i < k; ++i) {
        if (predFunc(m, i)) {
            list.append(i);
        }
    }
    return list;
}

QList<int> PrivateModulePlatformBackend::enumerateInputPorts(int uid)
{
    return enumeratePorts(uid,
        [](ComputeModule& m) { return m.numInputs(); },
        [](ComputeModule& m, size_t i) { return ! m.inputPort(i).lock()->traits().hasTrait("parameter"); });
}

QList<int> PrivateModulePlatformBackend::enumerateParamPorts(int uid)
{
    return enumeratePorts(uid,
        [](ComputeModule& m) { return m.numInputs(); },
        [](ComputeModule& m, size_t i) { return m.inputPort(i).lock()->traits().hasTrait("parameter"); });
}

QList<int> PrivateModulePlatformBackend::enumerateOutputPorts(int uid)
{
    return enumeratePorts(uid,
        [](ComputeModule& m) { return m.numOutputs(); },
        [](ComputeModule& m, size_t i) { return true; });
}

QVariantMap PrivateModulePlatformBackend::getModuleProperties(int uid)
{
    auto& m = fetchBackendModule(uid);

    QVariantMap vmap;
    vmap["uid"] = uid;
    vmap["displayName"] = QString::fromStdString(m.name());
    return vmap;
}

std::vector<std::pair<int, int>> PrivateModulePlatformBackend::fetchInputPortsCompatibleTo(std::shared_ptr<cp::OutputPort> port)
{
    auto& outTraits = port->traits();
    std::vector<std::pair<int, int>> list;

    for (auto& pr : m_modules) {
        QList<int> result = enumeratePorts(
            pr.first,
            [](ComputeModule& m) { return m.numInputs(); },
            [&outTraits](ComputeModule& m, size_t i)
            {
                return m.inputPort(i).lock()->traits().equals(outTraits); 
            }
        );
        for (int x : result) {
            list.push_back(make_pair(pr.first, x));
        }
    }
    return list;
}

std::vector<std::pair<int, int>> PrivateModulePlatformBackend::fetchOutputPortsCompatibleTo(std::shared_ptr<cp::InputPort> port)
{
    auto& inTraits = port->traits();
    std::vector<std::pair<int, int>> list;
    
    for (auto& pr : m_modules) {
        QList<int> result = enumeratePorts(
            pr.first,
            [](ComputeModule& m) { return m.numOutputs(); },
            [&inTraits](ComputeModule& m, size_t i) { return m.outputPort(i).lock()->traits().equals(inTraits); }
        );
        for (int x : result) {
            list.push_back(make_pair(pr.first, x));
        }
    }
    return list;
}

QVariantMap PrivateModulePlatformBackend::getInputPortProperties(int uid, int portId)
{
    auto& m = fetchBackendModule(uid);
    auto p = fetchInputPort(uid, portId).lock();

    QVariantMap vmap;
    vmap["uid"] = uid;
    vmap["portId"] = portId;
    vmap["displayName"] = QString::fromStdString(p->name());
    vmap["isParameter"] = p->traits().hasTrait("parameter");
    if (p->traits().hasTrait("int-like")) {
        vmap["type"] = "int";
    }
    if (p->traits().hasTrait("float-like")) {
        vmap["type"] = "float";
    }
    if (p->traits().hasTrait("bool-like")) {
        vmap["type"] = "bool";
    }

    auto portList = fetchOutputPortsCompatibleTo(p);
    QVariantList vlist;
    for (auto pr : portList) {
        QVariantMap listItemMap;
        listItemMap["targetUid"] = pr.first;
        listItemMap["targetPortId"] = pr.second;
        auto& targetModule = fetchBackendModule(pr.first);
        auto targetPort = fetchOutputPort(pr.first, pr.second).lock();
        listItemMap["targetModuleDisplayName"] = QString::fromStdString(targetModule.name());
        listItemMap["targetPortDisplayName"] = QString::fromStdString(targetPort->name());
        vlist.append(listItemMap);
    }
    vmap["options"] = vlist;
    return vmap;
}

QVariantMap PrivateModulePlatformBackend::getOutputPortProperties(int uid, int portId)
{
    auto& m = fetchBackendModule(uid);
    auto p = fetchOutputPort(uid, portId).lock();

    QVariantMap vmap;
    vmap["uid"] = uid;
    vmap["portId"] = portId;
    vmap["displayName"] = QString::fromStdString(p->name());
    
    if (p->traits().hasTrait("int-like")) {
        vmap["type"] = "int";
    }
    if (p->traits().hasTrait("float-like")) {
        vmap["type"] = "float";
    }
    if (p->traits().hasTrait("bool-like")) {
        vmap["type"] = "bool";
    }
    if (p->traits().hasTrait("float-image")) {
        vmap["type"] = "float-image";
    }
    if (p->traits().hasTrait("int-image")) {
        vmap["type"] = "int-image";
    }

    return vmap;
}

VolumeTexture* PrivateModulePlatformBackend::getOutputTexture(int uid, int portId)
{
    // TOOD: plug a typed Sink module to this output and read the sink modules multidim
    // image instead of the nast dyncast

    auto& m = fetchBackendModule(uid);
    auto p = fetchOutputPort(uid, portId).lock();
    if (p->traits().hasTrait("float-image")) {
        // TODO: kind of nasty, try to find a better way
        TypedOutputPort<MultiDimImage<float>>* tp = 
            dynamic_cast<TypedOutputPort<MultiDimImage<float>>*>(p.get());

        if (!tp) {
            throw std::runtime_error("internal output format error");
        }
        // TODO: double check these dangerous naked pointers
        VolumeTexture* tex = new VolumeTexture;
        tex->init(tp->value());
        return tex;
    }
    return nullptr;
}

bool PrivateModulePlatformBackend::connectInputOutput(int outputModuleUid, int outputPortId,
    int inputModuleUid, int inputPortId)
{
    auto input = fetchInputPort(inputModuleUid, inputPortId);
    auto output = fetchOutputPort(outputModuleUid, outputPortId).lock();
    return output->bind(input);
}

void PrivateModulePlatformBackend::disconnectInput(int inputModuleUid, int inputPortId)
{
    auto input = fetchInputPort(inputModuleUid, inputPortId);
    input.lock()->getSource().lock()->unbind(input);
}

bool PrivateModulePlatformBackend::setParamPortProperty(int uid, int portId, QVariant value)
{
    // auto input = fetchInputPort(uid, portId);
    // auto& t = input.lock()->traits();
    // if (t.hasTrait("int-like")) {

    // }
    return false;
}

int PrivateModulePlatformBackend::nextUid() const
{
    static int counter = 1;
    return counter++;
}

BackendModule& PrivateModulePlatformBackend::fetchBackendModule(int uid)
{
    if(!hasModule(uid)) {
        throw std::runtime_error("backend: no module with uid " + to_string(uid));
    }

    return *m_modules[uid];
}

std::weak_ptr<InputPort> PrivateModulePlatformBackend::fetchInputPort(int uid, int portId)
{
    auto& m = fetchBackendModule(uid);
    if (m.getComputeModule().numInputs() <= (size_t)portId) {
        throw std::runtime_error("backend: no input port with id " + to_string(portId) + " at module uid " + to_string(uid));
    }
    return m.getComputeModule().inputPort(portId);
}

std::weak_ptr<OutputPort> PrivateModulePlatformBackend::fetchOutputPort(int uid, int portId)
{
    auto& m = fetchBackendModule(uid);
    if (m.getComputeModule().numOutputs() <= (size_t)portId) {
        throw std::runtime_error("backend: no output port with id " + to_string(portId) + " at module uid " + to_string(uid));
    }
    return m.getComputeModule().outputPort(portId);
}

void PrivateModulePlatformBackend::evaluatePlatform()
{
    m_platform.run();
}