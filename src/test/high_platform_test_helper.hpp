#ifndef _test_high_platform_test_helper_hpp_
#define _test_high_platform_test_helper_hpp_

#include <iostream>
#include <memory>
#include <string>

#include <core/compute_platform/ports.h>
#include <core/compute_platform/port_utils.hpp>
#include <core/compute_platform/ComputeModule.h>
#include <core/compute_platform/ComputePlatform.h>
#include <core/multidim_image_platform/MultiDimImage.hpp>

namespace high_platform_test {

namespace cp = core::compute_platform;
namespace md = core::multidim_image_platform;

typedef md::MultiDimImage<float> Image;

template <typename T>
class NumberSource : public cp::ComputeModule {
public:
    NumberSource(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_seed(T()),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_outputs.template output<0>()->value() = m_seed;
    }
    void setNumber(T x)
    {
        m_seed = x;
    }
protected:
    T m_seed;
    cp::InputPortCollection m_inputs;
    cp::TypedOutputPortCollection<T> m_outputs;
};

template <typename T>
class NumberSink : public cp::ComputeModule {
public:
    NumberSink(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_result = m_inputs.template input<0>()->value();
    }
    T getNumber()
    {
        return m_result;
    }
protected:
    T m_result;
    cp::TypedInputPortCollection<T> m_inputs;
    cp::OutputPortCollection m_outputs;
};

class ImageSource : public cp::ComputeModule {
public:
    ImageSource(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_seed(std::make_shared<Image>()),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_outputs.output<0>()->forwardFromSharedPtr(m_seed);
    }
    void setImage(Image im)
    {
        *m_seed = im;
    }
protected:
    std::shared_ptr<Image> m_seed;
    cp::InputPortCollection m_inputs;
    cp::TypedOutputPortCollection<Image> m_outputs;
};

class ImageBypass : public cp::ComputeModule {
public:
    ImageBypass(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_outputs.output<0>()->forwardFromInput(m_inputs.input<0>());
    }
protected:
    cp::TypedInputPortCollection<Image> m_inputs;
    cp::TypedOutputPortCollection<Image> m_outputs;
};

class TwoImageBypass : public cp::ComputeModule {
public:
    TwoImageBypass(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_outputs.output<0>()->forwardFromInput(m_inputs.input<0>());
    }
protected:
    cp::TypedInputPortCollection<Image, Image> m_inputs;
    cp::TypedOutputPortCollection<Image> m_outputs;
};

class ImageSink : public cp::ComputeModule {
public:
    ImageSink(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_result = m_inputs.input<0>()->inputPtr().lock();
    }
    Image& getImage()
    {
        return *m_result;
    }
protected:
    std::shared_ptr<Image> m_result;
    cp::TypedInputPortCollection<Image> m_inputs;
    cp::OutputPortCollection m_outputs;
};

class TwoImageSink : public cp::ComputeModule {
public:
    TwoImageSink(cp::ComputePlatform& parent)
        : cp::ComputeModule(parent, m_inputs, m_outputs),
        m_inputs(*this),
        m_outputs(*this)
    {}
    void execute() override
    {
        m_result = m_inputs.input<0>()->inputPtr().lock();
    }
    Image& getImage()
    {
        return *m_result;
    }
protected:
    std::shared_ptr<Image> m_result;
    cp::TypedInputPortCollection<Image, Image> m_inputs;
    cp::OutputPortCollection m_outputs;
};

}

#endif