#include "bf_game_hero_bullet_level.h"

#include "btn_sound_items.h"
#include "bf_game_bullet_util.h"

namespace bf::game
{

namespace
{
    constexpr const hero_bullet_event _level0_events[] = {
        hero_bullet_event(direction_vector(0,       -1,     2.5),   1,  0,  true),
    };

    constexpr const hero_bullet_event _level1_events[] = {
        hero_bullet_event(direction_vector(0,       -1,     2.5),   1,  1,  true),
        hero_bullet_event(direction_vector(-0.075,  -1,     2.5),   11, 0,  true),
        hero_bullet_event(direction_vector(0.075,   -1,     2.5),   31, 0,  true),
    };

    constexpr const hero_bullet_event _level2_events[] = {
        hero_bullet_event(direction_vector(0,       -1,     1.75),  1,  2,  true),
        hero_bullet_event(direction_vector(-0.085,  -1,     2.5),   11, 0,  false),
        hero_bullet_event(direction_vector(0.05,    -1,     2.5),   21, 1,  true),
        hero_bullet_event(direction_vector(0.085,   -1,     2.5),   31, 0,  false),

        hero_bullet_event(direction_vector(0,       -1,     1.75),  41, 2,  true),
        hero_bullet_event(direction_vector(-0.085,  -1,     2.5),   51, 0,  false),
        hero_bullet_event(direction_vector(-0.05,   -1,     2.5),   61, 1,  true),
        hero_bullet_event(direction_vector(0.085,   -1,     2.5),   71, 0,  false),
    };

    constexpr const hero_bullet_level _levels[] = {
        hero_bullet_level(_level0_events, btn::fixed_size(9, 9),    btn::sound_items::gun_5,  4,
                btn::color(31, 28, 5),  20,   1),
        hero_bullet_level(_level1_events, btn::fixed_size(16, 16),  btn::sound_items::gun_5,  300,
                btn::color(31, 28, 5),  40,   2),
        hero_bullet_level(_level2_events, btn::fixed_size(13, 16),  btn::sound_items::gun_1,  800,
                btn::color(13, 2, 2),   80,   4),
    };
}

btn::span<const hero_bullet_level> hero_bullet_level::all_levels()
{
    return _levels;
}

int hero_bullet_level::gem_experience(int level, btn::fixed y)
{
    int height = y.integer() + constants::play_height;
    int height_level = btn::clamp(height / 16, 0, 4);
    int multiplier;
    height_level = 4 - height_level;
    ++height_level;

    switch(level)
    {

    case 0:
        multiplier = 1;
        break;

    case 1:
        multiplier = 5;
        break;

    default:
        multiplier = 10;
        break;
    }

    return height_level * multiplier;
}

}
