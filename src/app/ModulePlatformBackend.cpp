#include "ModulePlatformBackend.h"

#include <core/io_utils/IcsAdapter.h>
#include <fstream>
#include <sstream>
#include "VolumeTexture.h"
#include "VolumeData.h"
#include <core/high_platform/PythonComputeModule.h>
#include <QDirIterator>
#include <QFileInfo>

#include <iostream>

using namespace std;
using namespace core::io_utils;
using namespace core::compute_platform;
using namespace core::multidim_image_platform;
using namespace core::high_platform;

BackendModule::BackendModule(int uid)
    : m_uid(uid)
{}

int BackendModule::uid() const
{
    return m_uid;
}

std::string BackendModule::name() const
{
    return getComputeModule().name();
}

// --------------------------------------------------------

DataSourceModule::DataSourceModule(core::compute_platform::ComputePlatform& parent, int uid)
    : core::compute_platform::ComputeModule(parent, m_inputs, m_outputs, "DataSource" + to_string(uid)),
    BackendModule(uid),
    m_inputs(*this),
    m_outputs(*this)
{}

void DataSourceModule::execute()
{
    m_outputs.output<0>()->forwardFromSharedPtr(m_data);
}

void DataSourceModule::setData(std::shared_ptr<core::multidim_image_platform::MultiDimImage<float>> data)
{
    m_data = data;
}

core::compute_platform::ComputeModule& DataSourceModule::getComputeModule()
{
    return const_cast<core::compute_platform::ComputeModule&>(static_cast<const DataSourceModule&>(*this).getComputeModule());
}

const core::compute_platform::ComputeModule& DataSourceModule::getComputeModule() const
{
    return *this;
}

// --------------------------------------------------------

GenericModule::GenericModule(core::compute_platform::ComputePlatform& parent, const std::string& script, int uid)
    : core::high_platform::PythonComputeModule(parent, script, "Generic" + to_string(uid)),
    BackendModule(uid)
{}

core::compute_platform::ComputeModule& GenericModule::getComputeModule()
{
    return const_cast<core::compute_platform::ComputeModule&>(static_cast<const GenericModule&>(*this).getComputeModule());
}

const core::compute_platform::ComputeModule& GenericModule::getComputeModule() const
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
        // TODO: buildimageOutputHelperModules requires that m_modules has the module with the uid,
        // make it explicit or modify the function, because swapping the line above and below will
        // fail to do their job
        buildimageOutputHelperModules(newModule->uid());
    }

    return uids;
}

int PrivateModulePlatformBackend::createGenericModule(const QString& scriptPath)
{
    string path = scriptPath.toStdString();

    ifstream f(path);
    if (!f.is_open()) {
        throw std::runtime_error("missing module script: " + path);
    }
    stringstream buffer;
    buffer << f.rdbuf();

    // TODO: don't summon objects through naked pointers
    auto newModule = new GenericModule(m_platform, buffer.str(), nextUid());

    m_modules.emplace(make_pair(newModule->uid(), newModule));
    buildParamHelperModules(newModule->uid());
    buildimageOutputHelperModules(newModule->uid());
    return newModule->uid();
}

void PrivateModulePlatformBackend::buildParamHelperModules(int uid)
{
    QList<int> paramList = enumerateParamPorts(uid);
    auto& m = fetchBackendModule(uid);

    for (int portId : paramList) {
        auto port = fetchInputPort(uid, portId).lock();
        const auto& t = port->traits();

        // TODO: don't use naked pointers
        ParamHelperModule* helperModule = nullptr;

        // TODO: fix this mess, find a project-wide solution for static-dynamic type conversion
        if (t.hasTrait("int-like")) {
            if (t.hasTrait("uint8_t")) {
                helperModule = new TypedParamHelperModule<uint8_t>(m_platform, 0);
            } else if (t.hasTrait("uint16_t")) {
                helperModule = new TypedParamHelperModule<uint16_t>(m_platform, 0);
            } else if (t.hasTrait("uint32_t")) {
                helperModule = new TypedParamHelperModule<uint32_t>(m_platform, 0);
            } else if (t.hasTrait("uint64_t")) {
                helperModule = new TypedParamHelperModule<uint64_t>(m_platform, 0);
            } else if (t.hasTrait("int8_t")) {
                helperModule = new TypedParamHelperModule<int8_t>(m_platform, 0);
            } else if (t.hasTrait("int16_t")) {
                helperModule = new TypedParamHelperModule<int16_t>(m_platform, 0);
            } else if (t.hasTrait("int32_t")) {
                helperModule = new TypedParamHelperModule<int32_t>(m_platform, 0);
            } else if (t.hasTrait("int64_t")) {
                helperModule = new TypedParamHelperModule<int64_t>(m_platform, 0);
            } else {
                throw std::runtime_error("unknown input parameter int-like type, can not create input module for that");
            }
        } else if (t.hasTrait("float-like")) {
            if (t.hasTrait("float")) {
                helperModule = new TypedParamHelperModule<float>(m_platform, 0);
            } else if (t.hasTrait("double")) {
                helperModule = new TypedParamHelperModule<double>(m_platform, 0);
            } else {
                throw std::runtime_error("unknown input parameter float-like type, can not create input module for that");
            }
        } else if (t.hasTrait("bool-like")) {
            helperModule = new TypedParamHelperModule<bool>(m_platform, 0);
        } else {
            throw std::runtime_error("unknown input parameter type, can not create input module for that");
        }

        if (!connectPorts(*helperModule, 0, m.getComputeModule(), portId)) {
            throw std::runtime_error("internal error, can not connect param helper modules");
        }
        m_paramHelpers[make_pair(uid, portId)] = unique_ptr<ParamHelperModule>(helperModule);
    }
}

void PrivateModulePlatformBackend::buildimageOutputHelperModules(int uid)
{
    QList<int> outList = enumerateOutputPorts(uid);
    auto& m = fetchBackendModule(uid);

    for (int portId : outList) {
        auto port = fetchOutputPort(uid, portId).lock();
        const auto& t = port->traits();

        if (t.hasTrait("float-image")) {
            // TODO: don't use naked pointers
            ImageOutputHelperModule* helperModule = nullptr;
            helperModule = new ImageOutputHelperModule(m_platform);
            connectPorts(m.getComputeModule(), portId, *helperModule, 0);
            m_imageOutputHelpers[make_pair(uid, portId)] = unique_ptr<ImageOutputHelperModule>(helperModule);
        } else {
            // TODO: handle int-image type too
        }
    }
}

bool PrivateModulePlatformBackend::hasModule(int uid)
{
    return m_modules.end() != m_modules.find(uid);
}

void PrivateModulePlatformBackend::destroyModule(int uid)
{
    erase_if(m_paramHelpers,
        [uid](const decltype(m_paramHelpers)::value_type& item)
        {
            return uid == item.first.first;
        });

    erase_if(m_imageOutputHelpers,
        [uid](const decltype(m_imageOutputHelpers)::value_type& item)
        {
            return uid == item.first.first;
        });

    erase_if(m_modules,
        [uid](const decltype(m_modules)::value_type& item)
        {
            return uid == item.first;
        });
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
        [](ComputeModule& m, size_t i) { return ! m.inputPort(i).lock()->hasTag("parameter"); });
}

QList<int> PrivateModulePlatformBackend::enumerateParamPorts(int uid)
{
    return enumeratePorts(uid,
        [](ComputeModule& m) { return m.numInputs(); },
        [](ComputeModule& m, size_t i) { return m.inputPort(i).lock()->hasTag("parameter"); });
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

std::vector<std::pair<int, int>> PrivateModulePlatformBackend::fetchInputPortsCompatibleTo(std::shared_ptr<core::compute_platform::OutputPort> port)
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

std::vector<std::pair<int, int>> PrivateModulePlatformBackend::fetchOutputPortsCompatibleTo(std::shared_ptr<core::compute_platform::InputPort> port)
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
    if (p->traits().hasTrait("py-object")) {
        vmap["type"] = "py-object";
    }

    return vmap;
}

VolumeTexture* PrivateModulePlatformBackend::getOutputTexture(int uid, int portId)
{
    ImageOutputHelperModule& helper = fetchImageOutputHelperModule(uid, portId);

    // TODO: don't use naked ptrs
    
    auto imPtr = helper.getImage();
    if (imPtr) {
        VolumeTexture* tex = new VolumeTexture;
        tex->init(*imPtr);
        return tex;
    }
    return nullptr;
}

bool PrivateModulePlatformBackend::connectInputOutput(int outputModuleUid, int outputPortId,
    int inputModuleUid, int inputPortId)
{
    auto input = fetchInputPort(inputModuleUid, inputPortId);
    auto output = fetchOutputPort(outputModuleUid, outputPortId).lock();
    if (output) {
        return output->bind(input);
    } else {
        qDebug() << "no output port with id " << outputPortId << " for module with uid " << outputModuleUid << ", can not connect";
        return false;
    }
}

void PrivateModulePlatformBackend::disconnectInput(int inputModuleUid, int inputPortId)
{
    auto input = fetchInputPort(inputModuleUid, inputPortId);
    if (auto inPtr = input.lock()) {
        if (auto srcPtr = inPtr->getSource().lock()) {
            srcPtr->unbind(input);
        }
    }
}

bool PrivateModulePlatformBackend::setParamPortProperty(int uid, int portId, QVariant value)
{
    return fetchParamHelperModule(uid, portId).setData(value);
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

ParamHelperModule& PrivateModulePlatformBackend::fetchParamHelperModule(int uid, int portId)
{
    auto& ptr = m_paramHelpers[make_pair(uid, portId)];
    if (!ptr) {
        throw std::runtime_error("no helper param module for uid " + to_string(uid) + " portId " + to_string(portId));
    }
    return *ptr;
}

ImageOutputHelperModule& PrivateModulePlatformBackend::fetchImageOutputHelperModule(int uid, int portId)
{
    auto& ptr = m_imageOutputHelpers[make_pair(uid, portId)];
    if (!ptr) {
        throw std::runtime_error("no helper image output module for uid " + to_string(uid) + " portId " + to_string(portId));
    }
    return *ptr;
}

void PrivateModulePlatformBackend::evaluatePlatform()
{
    m_platform.printModuleConnections();
    m_platform.run();
}

QVariantList PrivateModulePlatformBackend::getModuleScriptsList()
{
    QVariantList vlist;
    auto isModuleFile = [](const QFileInfo& f)
    {
        return f.isFile()
            && f.fileName().toLower().startsWith("module")
            && f.suffix().toLower() == "py";
    };

    QDirIterator level1("modules");
    while (level1.hasNext()) {
        level1.next();
        if (level1.fileInfo().isDir() && !level1.fileInfo().isHidden()
			&& level1.fileName() != "." && level1.fileName() != "..") {

            QVariantMap vmap;
            vmap["displayName"] = level1.fileName();
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
                    fileVMap["displayName"] = name;
                    fileList.append(fileVMap);
                }
            }
            if (!fileList.empty()) {
                vmap["files"] = fileList;
                vlist.append(vmap);
            }
        }
    }

    return vlist;
}

ModulePlatformBackend::ModulePlatformBackend(QObject* parent)
    : QObject(parent)
{
    // TODO: move to cpp
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