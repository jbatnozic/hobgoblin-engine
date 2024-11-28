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
    // Mountain
    SPR_MOUNTAIN,
    SPR_ROCK_EDGE,
    // Miscellaneous
    SPR_POWER,
    SPR_BACKGROUND,
    SPR_SCALE
};

inline void LoadSprites(hg::gr::SpriteLoader& aSpriteLoader) {
    const std::filesystem::path basePath     = "Assets/Sprites";
    const std::filesystem::path mountainPath = "Assets/Sprites/Mountain";

    float occupancy = 0.f;

    aSpriteLoader
        .startTexture(2048, 2048)
        // Hand
        ->addSubsprite(SPR_HAND_OPEN, basePath / "hor.png")
        ->addSubsprite(SPR_HAND_CLOSED, basePath / "hcr.png")
        // Legs
        ->addSubsprite(SPR_LEGS, basePath / "legs.png")
        // Body
        ->addSubsprite(SPR_BODY_SIZE_1, basePath / "b1.png")
        ->addSubsprite(SPR_BODY_SIZE_2, basePath / "b2.png")
        ->addSubsprite(SPR_BODY_SIZE_3, basePath / "b3.png")
        ->addSubsprite(SPR_BODY_SIZE_4, basePath / "b4.png")
        ->addSubsprite(SPR_BODY_SIZE_5, basePath / "b5.png")
        // Biceps
        ->addSubsprite(SPR_BICEPS_SIZE_1, basePath / "b1r1.png")
        ->addSubsprite(SPR_BICEPS_SIZE_2, basePath / "b2r1.png")
        ->addSubsprite(SPR_BICEPS_SIZE_3, basePath / "b3r1.png")
        ->addSubsprite(SPR_BICEPS_SIZE_4, basePath / "b4r1.png")
        ->addSubsprite(SPR_BICEPS_SIZE_5, basePath / "b5r1.png")
        // Forearm
        ->addSubsprite(SPR_FOREARM_SIZE_1, basePath / "b1r2.png")
        ->addSubsprite(SPR_FOREARM_SIZE_2, basePath / "b2r2.png")
        ->addSubsprite(SPR_FOREARM_SIZE_3, basePath / "b3r2.png")
        ->addSubsprite(SPR_FOREARM_SIZE_4, basePath / "b4r2.png")
        ->addSubsprite(SPR_FOREARM_SIZE_5, basePath / "b5r2.png")
        // Mountain
        ->addSubsprite(SPR_MOUNTAIN, mountainPath / "m1.png")
        ->addSubsprite(SPR_MOUNTAIN, mountainPath / "m2.png")
        ->addSubsprite(SPR_MOUNTAIN, mountainPath / "m3.png")
        ->addSubsprite(SPR_MOUNTAIN, mountainPath / "mtr1.png")
        ->addSubsprite(SPR_MOUNTAIN, mountainPath / "mtr2.png")
        ->addSubsprite(SPR_MOUNTAIN, mountainPath / "mtr3.png")
        ->addSubsprite(SPR_MOUNTAIN, mountainPath / "mtl1.png")
        ->addSubsprite(SPR_MOUNTAIN, mountainPath / "mtl2.png")
        ->addSubsprite(SPR_MOUNTAIN, mountainPath / "mtl3.png")
        ->addSubsprite(SPR_ROCK_EDGE, mountainPath / "h-right.png")
        ->addSubsprite(SPR_ROCK_EDGE, mountainPath / "h-up.png")
        ->addSubsprite(SPR_ROCK_EDGE, mountainPath / "h-left.png")
        ->addSubsprite(SPR_ROCK_EDGE, mountainPath / "h-down.png")
        // Miscellaneous
        ->addSubsprite(SPR_POWER, basePath / "power.png")
        // Finalize
        ->finalize(hg::gr::TexturePackingHeuristic::BestAreaFit, &occupancy);

    HG_LOG_INFO(LOG_ID, "Game sprites loaded successfully (testure occupancy {}%).", occupancy * 100.f);

    aSpriteLoader.startTexture(2048, 2048)
        ->addSubsprite(SPR_SCALE, mountainPath / "Scales.png")
        ->finalize(hg::gr::TexturePackingHeuristic::BestAreaFit, &occupancy);

    HG_LOG_INFO(LOG_ID, "Scale sprite loaded successfully (testure occupancy {}%).", occupancy * 100.f);

    aSpriteLoader
        .startTexture(4096, 4094)
        // Miscellaneous
        ->addSubsprite(SPR_BACKGROUND, basePath / "background.png")
        // Finalize
        ->finalize(hg::gr::TexturePackingHeuristic::BestAreaFit, &occupancy);

    HG_LOG_INFO(LOG_ID,
                "Background sprites loaded successfully (testure occupancy {}%).",
                occupancy * 100.f);
}
