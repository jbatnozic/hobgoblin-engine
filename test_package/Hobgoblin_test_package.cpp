#include <Hobgoblin/ChipmunkPhysics.hpp>
#include <Hobgoblin/ColDetect.hpp>
#include <Hobgoblin/Common.hpp>
#include <Hobgoblin/Config.hpp>
#include <Hobgoblin/Format.hpp>
#include <Hobgoblin/Graphics.hpp>
#include <Hobgoblin/Math.hpp>
#include <Hobgoblin/Preprocessor.hpp>
#include <Hobgoblin/QAO.hpp>
#include <Hobgoblin/RigelNet.hpp>
#include <Hobgoblin/RmlUi.hpp>
#include <Hobgoblin/Utility/Dynamic_bitset.hpp> // Utility doesn't have an umbrella header
#include <Hobgoblin/Unicode.hpp>

#include <SPeMPE/SPeMPE.hpp>

#include <cstdlib>
#include <iostream>

using namespace jbatnozic::hobgoblin;
namespace spe = jbatnozic::spempe;

int main() {
    // ColDetect
    {
        cd::QuadTreeCollisionDomain domain{128.0, 128.0, 5, 5, 0};
    }
    // Common
    {
        PZInteger pz = 1337;
        (void)pz;
    }
    // Format
    {
        (void)fmt::format("This is {}!", "Hobgoblin");
    }
    // Graphics
    {
        // TODO
    }
    // HGConfig
    {
        HGConfig hgconf{"[[Section1]]"};
    }
    // HGExcept
    {
        try {
            HG_THROW_TRACED(TracedLogicError, 0, "message");
        }
        catch (...) {
        }
    }
    // Math
    {
        math::Sqr(11.f);
    }
    // Preprocessor
    {
        const int five = HG_PP_COUNT_ARGS(a, 2, c, 3, e);
    }
    // Unicode
    {
        const auto us = HG_UNISTR("hobgoblin");
        const auto path = UniStrConv(TO_STD_PATH, us);
    }
    // QAO
    {
        QAO_Runtime rt;
        rt.startStep();
    }
    // RigelNet
    {
        RN_IndexHandlers();
    }
    // RmlUi
    {
        auto guard = rml::HobgoblinBackend::initialize();
        guard.reset();
    }
    // Utility
    {
        util::DynamicBitset bitset;
        bitset.setBit(100);
    }
    // SPeMPE
    {
        spe::GameContext::RuntimeConfig rtcfg;
        spe::GameContext ctx{rtcfg};
        ctx.runFor(1);
    }

    std::cout << "The Goblin hobs happily!\n";
    return EXIT_SUCCESS;
}
