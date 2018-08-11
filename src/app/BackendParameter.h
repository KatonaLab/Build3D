#ifndef _app_BackendParameter_h_
#define _app_BackendParameter_h_

#include "BackendStoreItem.h"
#include <core/compute_platform/ComputeModule.h>
#include "ParameterInterfaceModules.hpp"

class BackendParameter : public BackendStoreItem {
    typedef core::compute_platform::ComputeModule ComputeModule;
    typedef core::compute_platform::InputPort InputPort;
public:
    BackendParameter(ComputeModule& sourceModule, int portId);
    int uid() const override;
    QString category() const override;
    QString name() const override;
    QString type() const override;
    int status() const override;
    QVariant value() const override;
protected:
    int m_portId = -1;
    std::weak_ptr<InputPort> m_source;
    std::shared_ptr<ParameterInterfaceModule> m_interfaceModule;
};

#endif
