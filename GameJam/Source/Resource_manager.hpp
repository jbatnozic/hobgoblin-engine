#pragma once

#include "Engine.hpp"

#include "Resource_manager_interface.hpp"

class ResourceManager
    : public  ResourceManagerInterface
    , public  spe::NonstateObject {
public:
    explicit ResourceManager(QAO_RuntimeRef aRuntimeRef, int aExecutionPriority);

    ~ResourceManager() override;

    void setToHostMode() override;
    void setToClientMode() override;
    Mode getMode() const override;

    const hg::gr::SpriteLoader& getSpriteLoader() const override;

private:
    Mode _mode = Mode::UNINITIALIZED;

    hg::gr::SpriteLoader _spriteLoader;
};
