#ifndef _app_BackendInput_h_
#define _app_BackendInput_h_

#include "BackendStoreItem.h"
#include <core/compute_platform/ComputeModule.h>

class BackendInput : public BackendStoreItem {
    typedef core::compute_platform::InputPort InputPort;
public:
    BackendInput(std::weak_ptr<InputPort> source, int portId);
    int uid() const override;
    QString category() const override;
    QString name() const override;
    QString type() const override;
    int status() const override;
    QVariant value() const override;
protected:
    std::weak_ptr<InputPort> m_source;
    int m_portId = -1;
};

#endif
