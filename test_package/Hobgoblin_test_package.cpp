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

#include <SPeMPE/SPeMPE.hpp>

using namespace jbatnozic::hobgoblin;
namespace spe = jbatnozic::spempe;

int main() {
    // ColDetect
    {
        cd::QuadTreeCollisionDomain domain{128.0, 128.0, 5, 5, 0};
    }
    // Common
    {
        try {
            throw TracedRuntimeError{"message"};
        }
        catch (...) {
        }
    }
    // Config
    {
        HGConfig hgconf{"[[Section1]]"};
    }
    // Format
    {
        fmt::format("This is {}!", "Hobgoblin");
    }
    // Graphics
    {
        // TODO
    }
    // Math
    {
        math::Sqr(11.f);
    }
    // Preprocessor
    {
        const int five = HG_PP_COUNT_ARGS(a, 2, c, 3, e);
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
}
