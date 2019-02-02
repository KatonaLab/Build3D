#ifndef _app_BackendInput_h_
#define _app_BackendInput_h_

#include "BackendStoreItem.h"
#include <core/compute_platform/ComputeModule.h>

class BackendStore;

class BackendInput : public BackendStoreItem {
    typedef core::compute_platform::InputPort InputPort;
public:
    BackendInput(std::weak_ptr<InputPort> source, int portId,
        int parentUid, const BackendStore& store);
    int uid() const override;
    int parentUid() const override;
    QString category() const override;
    QString name() const override;
    QString type() const override;
    int status() const override;
    QVariant value() const override;
    QVariant hints() const override;
    void invalidate() override {}

    void setName(const QString& name) override;
    void setStatus(int status) override;
    bool setValue(QVariant value) override;

    std::weak_ptr<InputPort> source();
protected:
    std::weak_ptr<InputPort> m_source;
    int m_portId = -1;
    int m_parentUid = -1;
    QVariantMap m_hints;
    QString m_type;
    const BackendStore& m_store;
};

#endif
