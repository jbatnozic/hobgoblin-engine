#pragma once

constexpr float gravity         = 0.5;
constexpr float character_speed = 10;
constexpr float fall_timer      = 40; // in frames
constexpr float fling_timer     = 30; // in frames
constexpr float fling_speed     = 30;

constexpr int   single_terrain_size = 512;
constexpr int   terrain_size        = 30;
constexpr float y_floor             = single_terrain_size * terrain_size;

constexpr float hole_chance      = 0.2f;
constexpr float terrain_1_chance = 0.3f;
constexpr float terrain_2_chance = 0.1f;
constexpr int   mountain_height  = terrain_size;
constexpr float slope_chance     = 0.5f;
inline int             left_offset      = 0;// max left mountain offset 
inline int             right_offset     = 0; // mar right mountain offset
