#ifndef BF_CONSTANTS_H
#define BF_CONSTANTS_H

#include "btn_common.h"

#ifndef BF_CFG_ENEMIES_GRID_LOG_ENABLED
    #define BF_CFG_ENEMIES_GRID_LOG_ENABLED false
#endif

namespace bf::constants
{
    constexpr const int max_hero_bullets = 32;
    constexpr const int max_hero_bombs = 2;
    constexpr const int max_enemies = 48;
    constexpr const int max_rect_size = 48;
    constexpr const int play_width = 72;
    constexpr const int play_height = (160 - 72) / 2;
    constexpr const int view_width = play_width * 2;
    constexpr const int view_height = (160 + max_rect_size) / 2;
    constexpr const int camera_width = 32;
    constexpr const int hero_bullet_levels = 9;
    constexpr const int hero_bullets_z_order = 1;
    constexpr const int enemies_z_order = 2;
    constexpr const int enemies_grid_size = 16;
    constexpr const int max_enemies_per_grid_cell = 4;
    constexpr const int reserved_sprite_affine_mats = 4;
}

#endif
