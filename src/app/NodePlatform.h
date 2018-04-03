#ifndef _app_NodePlatform_h_
#define _app_NodePlatform_h_

#include <QtCore>
#include <QQmlComponent>
#include <core/high_platform/PythonComputeModule.h>
#include "VolumeData.h"

class NodePlatform {
public:
    explicit NodePlatform(QObject *parent = Q_NULLPTR);
    Q_INVOKABLE uint32_t createSourceNode(const VolumeData& data);
    Q_INVOKABLE uint32_t createGenericNode(const QString& scriptPath);
    Q_INVOKABLE void hasNode(uint32_t uid);
    Q_INVOKABLE void removeNode(uint32_t uid);
    virtual ~NodePlatform() = default;
protected:
   ComputePlatform m_computePlatform;
};

#endif
