#include "ModulePlatformBackend.h"

#include <core/io_utils/IcsAdapter.h>
#include "VolumeTexture.h"
#include "VolumeData.h"
#include <core/high_platform/PythonComputeModule.h>

using namespace std;
using namespace core::io_utils;
using namespace core::multidim_image_platform;
using namespace core::high_platform;

ModulePlatformBackend::ModulePlatformBackend(QObject* parent)
: QObject(parent)
{}

QList<uint32_t> ModulePlatformBackend::createSourceNodesFromIcsFile(const QUrl& filename)
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
            volumes = m_image.splitDim(3);
            break;
        default:
            // TODO: proper error message to GUI
            throw std::runtime_error("ICS files with dimensions XYZ or XYZC are supported only");
    };

    QList<uint32_t> uids;
    for (auto& vol : volumes) {
        uint32_t id = nextUid();
        uids.push_back(id);

        auto newModule = new DataSourceModule(m_platform, id);
        // TODO: try to use move semantics to make a shared_ptr out of vol
        auto p = make_shared<MultiDimImage<float>>();
        swap(*p, vol);
        newModule->setData(p);

        m_modules.emplace_back(newModule);
        Q_EMIT nodeCreated(newModule->uid());
    }

    return uids;
}

uint32_t ModulePlatformBackend::createGenericNode(const QUrl& scriptPath)
{
    m_modules.emplace_back(new GenericModule(m_platform,
        scriptPath.toLocalFile().toStdString(), nextUid()));
    Q_EMIT nodeCreated(m_modules.back()->uid());
    return m_modules.back()->uid();
}

bool ModulePlatformBackend::hasNode(uint32_t uid)
{
    return m_modules.end() != find_if(m_modules.begin(), m_modules.end(),
        [uid](const unique_ptr<BackendModule>& x) { return uid == x->uid(); });
}

void ModulePlatformBackend::destroyNode(uint32_t uid)
{

}

QVariant ModulePlatformBackend::getNodeState(uint32_t uid)
{

}

VolumeTexture& ModulePlatformBackend::getNodeTexture(uint32_t uid, std::size_t outputPortId)
{

}

QList<uint32_t> ModulePlatformBackend::getPortCompatibleNodes(uint32_t uid, std::size_t inputPortId)
{

}

// VolumeDataCollection::VolumeDataCollection(QObject *parent)
//     : QObject(parent)
// {}

// bool VolumeDataCollection::loadICS(const string& filename)
// {
//     IcsAdapter ics;
//     ics.open(filename);
//     m_image = ics.readScaledConvert<float>(true);

//     size_t cn = 0;
//     switch (m_image.dims()) {
//         case 3: cn = 1; break;
//         case 4: cn = m_image.dim(3); break;
//         case 5: {
//             m_image.removeDims({3});
//             cn = m_image.dim(3);
//         } break;
//         default:
//             throw std::runtime_error("ICS files with dimensions XYZ or XYZC are supported only");
//     };

//     m_progress = 0.0f;
//     emit progressChanged();

//     for (size_t i = 0; i < cn; ++i) {
//         VolumeTexturePtr vtex = VolumeTexturePtr::create();
//         vtex->init(m_image, i);
//         m_dataList.append(vtex);

//         m_progress = (i + 1.0) / cn;
//         emit progressChanged();
//     }

//     return true;
// }

// void VolumeDataCollection::setSource(const QUrl& source)
// {
//     // TODO: loading to another thread (?)
//     m_source = source;
//     emit sourceChanged();
//     setStatus(Status::Loading);
//     bool success = loadICS(source.toLocalFile().toStdString());
//     setStatus(success ? Status::Ready : Status::Error);
// }

// void VolumeDataCollection::setStatus(const Status &status)
// {
//     m_status = status;
//     emit statusChanged();
// }

// QQmlListProperty<VolumeTexture> VolumeDataCollection::volumes()
// {
//     m_qmlList.clear();
//     for (auto &elem : m_dataList) {
//         m_qmlList.append(elem.data());
//     }
//     return QQmlListProperty<VolumeTexture>(this, m_qmlList);
// }