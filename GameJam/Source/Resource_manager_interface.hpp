#pragma once

#include "Engine.hpp"

#include <Hobgoblin/Graphics.hpp>

#include <string>

class ResourceManagerInterface : public spe::ContextComponent {
public:
    ~ResourceManagerInterface() override = default;

    const hg::gr::SpriteLoader& getSpriteLoader();

private:
    SPEMPE_CTXCOMP_TAG("ResourceManagerInterface");
};

using MResource = ResourceManagerInterface;
