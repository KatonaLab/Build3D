#ifndef _app_BackendOutput_h_
#define _app_BackendOutput_h_

#include "BackendStoreItem.h"
#include <core/compute_platform/ComputeModule.h>
#include "OutputInterfaceModules.hpp"

class BackendOutput : public BackendStoreItem {
    typedef core::compute_platform::ComputeModule ComputeModule;
    typedef core::compute_platform::OutputPort OutputPort;
public:
    BackendOutput(std::weak_ptr<OutputPort> source, int portId, int parentUid);
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
    std::weak_ptr<OutputPort> m_source;
    int m_portId = -1;
    int m_parentUid = -1;
    std::shared_ptr<ImageOutputInterfaceModule> m_interfaceModule;
};

#endif
