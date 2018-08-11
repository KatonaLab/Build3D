#ifndef _app_BackendOutput_h_
#define _app_BackendOutput_h_

#include "BackendStoreItem.h"
#include <core/compute_platform/ComputeModule.h>
#include "OutputInterfaceModules.hpp"

class BackendOutput : public BackendStoreItem {
    typedef core::compute_platform::ComputeModule ComputeModule;
    typedef core::compute_platform::OutputPort OutputPort;
public:
    BackendOutput(ComputeModule& sourceModule, int portId);
    int uid() const override;
    QString category() const override;
    QString name() const override;
    QString type() const override;
    int status() const override;
    QVariant value() const override;
protected:
    int m_portId = -1;
    std::weak_ptr<OutputPort> m_source;
    std::shared_ptr<ImageOutputInterfaceModule> m_interfaceModule;
};

#endif
