#ifndef _app_BackendParameter_h_
#define _app_BackendParameter_h_

#include "BackendStoreItem.h"
#include <core/compute_platform/ComputeModule.h>
#include "ParameterInterfaceModules.hpp"
#include <functional>
#include <memory>
#include <QVariantMap>
#include <QVariantList>
#include <QPair>

class BackendParameter : public BackendStoreItem {
    typedef core::compute_platform::ComputePlatform ComputePlatform;
    typedef core::compute_platform::InputPort InputPort;
public:
    BackendParameter(std::weak_ptr<InputPort> source,
        ComputePlatform& platform, int portId, int parentUid);
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
protected:
    std::weak_ptr<InputPort> m_source;
    int m_portId = -1;
    int m_parentUid = -1;
    QString m_type;
    std::shared_ptr<ParameterInterfaceModule> m_interfaceModule;
    QVariantMap m_hints;
};

namespace details {

    template <typename T>
    std::shared_ptr<ParameterInterfaceModule> buildParam(
        core::compute_platform::ComputePlatform& platform)
    {
        return std::make_shared<TypedParameterInterfaceModule<T>>(platform);
    }

    typedef std::function<
        std::shared_ptr<ParameterInterfaceModule>
        (core::compute_platform::ComputePlatform&)> BuildParamFunction;

    QPair<QVariantList, QVariantList> toEnumList(const core::compute_platform::PropertyMap& properties);
}

#endif
