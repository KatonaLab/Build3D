#ifndef _app_BackendOutput_h_
#define _app_BackendOutput_h_

#include "BackendStoreItem.h"
#include <core/compute_platform/ComputeModule.h>
#include "OutputInterfaceModules.hpp"
#include <QColor>

class BackendOutput : public BackendStoreItem {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    typedef core::compute_platform::OutputPort OutputPort;
public:
    BackendOutput(std::weak_ptr<OutputPort> source, ComputePlatform& platform,
        int portId, int parentUid);
    int uid() const override;
    int parentUid() const override;
    QString category() const override;
    QString name() const override;
    QString type() const override;
    int status() const override;
    QVariant value() const override;
    QVariant hints() const override;

    void setName(const QString& name) override;
    void setStatus(int status) override;
    bool setValue(QVariant value) override;

    std::weak_ptr<OutputPort> source();
protected:
    std::weak_ptr<OutputPort> m_source;
    int m_portId = -1;
    int m_parentUid = -1;
    QVariantMap m_hints;
    QString m_type;
    std::shared_ptr<ImageOutputInterfaceModule> m_interfaceModule;
    QColor m_color;
    bool m_visible;
};

namespace details {

    template <typename T>
    std::shared_ptr<ImageOutputInterfaceModule> buildImageOutput(
        core::compute_platform::ComputePlatform& platform)
    {
        return std::make_shared<TypedImageOutputInterfaceModule<T>>(platform);
    }

    typedef std::function<
        std::shared_ptr<ImageOutputInterfaceModule>
        (core::compute_platform::ComputePlatform&)> BuildOutputFunction;
}

#endif
