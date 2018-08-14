#ifndef _app_BackendParameter_h_
#define _app_BackendParameter_h_

#include "BackendStoreItem.h"
#include <core/compute_platform/ComputeModule.h>
#include "ParameterInterfaceModules.hpp"

class BackendParameter : public BackendStoreItem {
    typedef core::compute_platform::ComputeModule ComputeModule;
    typedef core::compute_platform::InputPort InputPort;
public:
    BackendParameter(ComputeModule& sourceModule, int portId, int parentUid);
    int uid() const override;
    int parentUid() const override;
    QString category() const override;
    QString name() const override;
    QString type() const override;
    int status() const override;
    QVariant value() const override;
protected:
    std::weak_ptr<InputPort> m_source;
    int m_portId = -1;
    int m_parentUid = -1;
    std::shared_ptr<ParameterInterfaceModule> m_interfaceModule;
};

#endif
