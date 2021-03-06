/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#include "bf_game_background.h"

#include "btn_colors.h"
#include "btn_blending.h"
#include "btn_bg_palettes.h"
#include "btn_rect_window.h"
#include "btn_regular_bg_builder.h"
#include "bf_game_stage.h"
#include "bf_wave_generator.h"

namespace bf::game
{

namespace
{
    constexpr const btn::fixed blending_transparency = 0.4;

    btn::regular_bg_ptr _create_bottom_bg(const stage& stage, const btn::camera_ptr& camera)
    {
        btn::regular_bg_builder builder(stage.background_bottom_bg_item);
        builder.set_mosaic_enabled(true);
        builder.set_camera(camera);
        return builder.release_build();
    }

    btn::regular_bg_ptr _create_top_bg(const stage& stage, const btn::camera_ptr& camera)
    {
        btn::regular_bg_builder builder(stage.background_top_bg_item);
        builder.set_priority(2);
        builder.set_blending_enabled(true);
        builder.set_mosaic_enabled(true);
        builder.set_camera(camera);
        return builder.release_build();
    }
}

background::background(const stage& stage, const btn::camera_ptr& camera) :
    _bottom_move_action(_create_bottom_bg(stage, camera), stage.background_bottom_bg_delta_position),
    _top_move_action(_create_top_bg(stage, camera), stage.background_top_bg_delta_position),
    _hblank_effect(btn::regular_bg_position_hblank_effect_ptr::create_horizontal(
                       _bottom_move_action.bg(), _hblank_effect_deltas))
{
    btn::blending::set_transparency_alpha(blending_transparency);
    btn::window::internal().set_show_bg(_top_move_action.bg(), false);
    _hblank_effect.set_visible(false);
}

void background::set_visible(bool visible)
{
    btn::regular_bg_ptr bottom_bg = _bottom_move_action.bg();
    bottom_bg.set_visible(visible);

    btn::regular_bg_ptr top_bg = _top_move_action.bg();
    top_bg.set_visible(visible);
}

void background::show_bomb_open(int frames)
{
    btn::bg_palette_ptr bottom_palette = _bottom_move_action.bg().palette();
    bottom_palette.set_grayscale_intensity(0);
    _bottom_palette_grayscale_action.emplace(btn::move(bottom_palette), frames / 2, 1);

    btn::bgs_mosaic::set_stretch(0.1);
    _mosaic_action = btn::bgs_mosaic_stretch_loop_action(4, 0.2);
}

void background::show_bomb_close(int frames)
{
    btn::bgs_mosaic::set_stretch(0);
    _mosaic_action.reset();

    _hblank_effect.set_visible(true);
    _fade_frames = frames;
}

void background::hide_bomb_close(int frames)
{
    btn::bg_palette_ptr bottom_palette = _bottom_move_action.bg().palette();
    bottom_palette.set_grayscale_intensity(1);
    _bottom_palette_grayscale_action.emplace(btn::move(bottom_palette), frames, 0);
}

void background::show_explosion_open(int frames)
{
    const btn::regular_bg_ptr& bottom_bg = _bottom_move_action.bg();
    btn::bg_palette_ptr bottom_palette = bottom_bg.palette();
    bottom_palette.set_fade(btn::colors::orange, 0);
    _bottom_palette_fade_action.emplace(btn::move(bottom_palette), frames, 1);

    const btn::regular_bg_ptr& top_bg = _top_move_action.bg();
    btn::bg_palette_ptr top_palette = top_bg.palette();
    top_palette.set_fade(btn::colors::orange, 0);
    _top_palette_fade_action.emplace(btn::move(top_palette), frames, 1);

    btn::bgs_mosaic::set_stretch(0.1);
    _mosaic_action = btn::bgs_mosaic_stretch_loop_action(4, 0.2);

    btn::rect_window external_window = btn::rect_window::external();
    external_window.set_show_bg(bottom_bg, false);
    external_window.set_show_bg(top_bg, false);

    btn::bg_palettes::set_transparent_color(btn::colors::orange);
}

void background::show_explosion_close(int frames)
{
    const btn::regular_bg_ptr& bottom_bg = _bottom_move_action.bg();
    btn::bg_palette_ptr bottom_palette = bottom_bg.palette();
    _bottom_palette_fade_action.emplace(btn::move(bottom_palette), frames, 0);

    const btn::regular_bg_ptr& top_bg = _top_move_action.bg();
    btn::bg_palette_ptr top_palette = top_bg.palette();
    _top_palette_fade_action.emplace(btn::move(top_palette), frames, 0);

    btn::bgs_mosaic::set_stretch(0);
    _mosaic_action.reset();

    btn::rect_window external_window = btn::rect_window::external();
    external_window.set_show_bg(bottom_bg, true);
    external_window.set_show_bg(top_bg, true);

    _hblank_effect.set_visible(true);
    _fade_frames = frames;
}

void background::show_top(int frames)
{
    btn::blending::set_transparency_alpha(0);
    _blending_action.emplace(frames, blending_transparency);
}

void background::show_hero_dying()
{
    _bottom_palette_inverted_action.emplace(_bottom_move_action.bg().palette(), 5);
    _top_palette_inverted_action.emplace(_top_move_action.bg().palette(), 5);
    btn::green_swap::set_enabled(true);
    _green_swap_action.emplace(5);
}

void background::show_hero_alive()
{
    btn::bg_palette_ptr bottom_palette = _bottom_move_action.bg().palette();
    btn::bg_palette_ptr top_palette = _top_move_action.bg().palette();
    bottom_palette.set_inverted(false);
    top_palette.set_inverted(false);
    btn::green_swap::set_enabled(false);
    _bottom_palette_inverted_action.reset();
    _top_palette_inverted_action.reset();
    _green_swap_action.reset();
}

void background::show_hero_dead()
{
    btn::bg_palette_ptr bottom_palette = _bottom_move_action.bg().palette();
    btn::bg_palette_ptr top_palette = _top_move_action.bg().palette();
    bottom_palette.set_inverted(true);
    top_palette.set_inverted(true);
    btn::green_swap::set_enabled(false);
    _bottom_palette_inverted_action.reset();
    _top_palette_inverted_action.reset();
    _green_swap_action.reset();
}

void background::reset()
{
    btn::blending::set_transparency_alpha(1);
}

void background::update()
{
    _bottom_move_action.update();
    _top_move_action.update();

    if(_mosaic_action)
    {
        _mosaic_action->update();
    }

    if(_blending_action)
    {
        _blending_action->update();

        if(_blending_action->done())
        {
            _blending_action.reset();
        }
    }

    if(_bottom_palette_fade_action)
    {
        _bottom_palette_fade_action->update();
        _top_palette_fade_action->update();

        if(_bottom_palette_fade_action->done())
        {
            _bottom_palette_fade_action.reset();
            _top_palette_fade_action.reset();
        }
    }

    if(_bottom_palette_grayscale_action)
    {
        _bottom_palette_grayscale_action->update();

        if(_bottom_palette_grayscale_action->done())
        {
            _bottom_palette_grayscale_action.reset();
        }
    }

    if(_bottom_palette_inverted_action)
    {
        _bottom_palette_inverted_action->update();
        _top_palette_inverted_action->update();
        _green_swap_action->update();
    }

    if(_fade_frames)
    {
        --_fade_frames;

        if(_fade_frames)
        {
            wave_generator wave_generator;
            int hblank_effect_speed_multiplier;
            int hblank_effect_amplitude;

            switch(_fade_frames)
            {

            case 1:
                hblank_effect_speed_multiplier = 1;
                hblank_effect_amplitude = 1;
                break;

            case 2:
                hblank_effect_speed_multiplier = 2;
                hblank_effect_amplitude = 1;
                break;

            case 3:
                hblank_effect_speed_multiplier = 4;
                hblank_effect_amplitude = 2;
                break;

            case 4:
                hblank_effect_speed_multiplier = 8;
                hblank_effect_amplitude = 2;
                break;

            case 5:
                hblank_effect_speed_multiplier = 16;
                hblank_effect_amplitude = 3;
                break;

            default:
                hblank_effect_speed_multiplier = 32;
                hblank_effect_amplitude = 3;
                break;
            }

            wave_generator.set_speed(_fade_frames * hblank_effect_speed_multiplier);
            wave_generator.set_amplitude(hblank_effect_amplitude);
            wave_generator.generate(_hblank_effect_deltas);
            _hblank_effect.reload_deltas_ref();
        }
        else
        {
            _hblank_effect.set_visible(false);
        }
    }
}

}
