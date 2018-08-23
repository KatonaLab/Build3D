#ifndef _app_BackendModule_h_
#define _app_BackendModule_h_

#include "BackendStoreItem.h"
#include <core/compute_platform/ComputeModule.h>

class BackendModule : public BackendStoreItem {
    typedef core::compute_platform::ComputeModule ComputeModule;
public:
    BackendModule(std::shared_ptr<ComputeModule> sourceModule, int uid);
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

    std::shared_ptr<ComputeModule> source();
protected:
    std::shared_ptr<ComputeModule> m_source;
    int m_uid = -1;
    int m_status = 0;
};

#endif
