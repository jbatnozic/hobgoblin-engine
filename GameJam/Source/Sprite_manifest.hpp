#pragma once

#include "Engine.hpp"

#include <Hobgoblin/Graphics.hpp>

#include <filesystem>

enum SpriteIds {
    // Hands
    SPR_HAND_OPEN,
    SPR_HAND_CLOSED,
    // Legs
    SPR_LEGS,
    // Body
    SPR_BODY_SIZE_1,
    SPR_BODY_SIZE_2,
    SPR_BODY_SIZE_3,
    SPR_BODY_SIZE_4,
    SPR_BODY_SIZE_5,
    // Biceps
    SPR_BICEPS_SIZE_1,
    SPR_BICEPS_SIZE_2,
    SPR_BICEPS_SIZE_3,
    SPR_BICEPS_SIZE_4,
    SPR_BICEPS_SIZE_5,
    // Forearm
    SPR_FOREARM_SIZE_1,
    SPR_FOREARM_SIZE_2,
    SPR_FOREARM_SIZE_3,
    SPR_FOREARM_SIZE_4,
    SPR_FOREARM_SIZE_5,
};

inline void LoadSprites(hg::gr::SpriteLoader& aSpriteLoader) {
    const std::filesystem::path basePath = "Assets/Sprites";
    float occupancy = 0.f;

    ///////////////////////////////////////////////////////////////////////////
    // PLAYER CHARACTER                                                      //
    ///////////////////////////////////////////////////////////////////////////

    aSpriteLoader
        .startTexture(2048, 2048)
        // Hand
        ->addSprite(SPR_HAND_OPEN, basePath / "hor.png")
        ->addSprite(SPR_HAND_CLOSED, basePath / "hcr.png")
        // Legs
        ->addSprite(SPR_LEGS, basePath / "legs.png")
        // Body
        ->addSprite(SPR_BODY_SIZE_1, basePath / "b1.png")
        ->addSprite(SPR_BODY_SIZE_2, basePath / "b2.png")
        ->addSprite(SPR_BODY_SIZE_3, basePath / "b3.png")
        ->addSprite(SPR_BODY_SIZE_4, basePath / "b4.png")
        ->addSprite(SPR_BODY_SIZE_5, basePath / "b5.png")
        // Biceps
        ->addSprite(SPR_BICEPS_SIZE_1, basePath / "b1r1.png")
        ->addSprite(SPR_BICEPS_SIZE_2, basePath / "b2r1.png")
        ->addSprite(SPR_BICEPS_SIZE_3, basePath / "b3r1.png")
        ->addSprite(SPR_BICEPS_SIZE_4, basePath / "b4r1.png")
        ->addSprite(SPR_BICEPS_SIZE_5, basePath / "b5r1.png")
        // Forearm
        ->addSprite(SPR_FOREARM_SIZE_1, basePath / "b1r2.png")
        ->addSprite(SPR_FOREARM_SIZE_2, basePath / "b2r2.png")
        ->addSprite(SPR_FOREARM_SIZE_3, basePath / "b3r2.png")
        ->addSprite(SPR_FOREARM_SIZE_4, basePath / "b4r2.png")
        ->addSprite(SPR_FOREARM_SIZE_5, basePath / "b5r2.png")
        // Finalize
        ->finalize(hg::gr::TexturePackingHeuristic::BestAreaFit, &occupancy);
    HG_LOG_INFO(LOG_ID,
                "Player character sprites loaded successfully (testure occupancy {}%).",
                occupancy * 100.f);

    ///////////////////////////////////////////////////////////////////////////
    // TERRAIN                                                               //
    ///////////////////////////////////////////////////////////////////////////

    // TODO
}
