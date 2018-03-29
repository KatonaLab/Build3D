#include "VolumeDataCollection.h"

#include <core/io_utils/IcsAdapter.h>
#include "VolumeTexture.h"
#include "VolumeData.h"

using namespace std;
using namespace core::io_utils;

VolumeDataCollection::VolumeDataCollection(QObject *parent)
    : QObject(parent)
{}

bool VolumeDataCollection::loadICS(const string& filename)
{
    IcsAdapter ics;
    ics.open(filename);
    m_image = ics.readScaledConvert<float>(true);

    size_t cn = 0;
    switch (m_image.dims()) {
        case 3: cn = 1; break;
        case 4: cn = m_image.dim(3); break;
        case 5: {
            m_image.removeDims({3});
            cn = m_image.dim(3);
        } break;
        default:
            throw std::runtime_error("ICS files with dimensions XYZ or XYZC are supported only");
    };

    m_progress = 0.0f;
    emit progressChanged();

    for (size_t i = 0; i < cn; ++i) {
        VolumeTexturePtr vtex = VolumeTexturePtr::create();
        vtex->init(m_image, i);
        m_dataList.append(vtex);

        m_progress = (i + 1.0) / cn;
        emit progressChanged();
    }

    return true;
}

void VolumeDataCollection::setSource(const QUrl& source)
{
    // TODO: loading to another thread (?)
    m_source = source;
    emit sourceChanged();
    setStatus(Status::Loading);
    bool success = loadICS(source.toLocalFile().toStdString());
    setStatus(success ? Status::Ready : Status::Error);
}

void VolumeDataCollection::setStatus(const Status &status)
{
    m_status = status;
    emit statusChanged();
}

QQmlListProperty<VolumeTexture> VolumeDataCollection::volumes()
{
    m_qmlList.clear();
    for (auto &elem : m_dataList) {
        m_qmlList.append(elem.data());
    }
    return QQmlListProperty<VolumeTexture>(this, m_qmlList);
}