/*
 * Copyright (c) 2020 Gustavo Valiente gustavo.valiente@protonmail.com
 * zlib License, see LICENSE file.
 */

#ifndef BTN_CORE_H
#define BTN_CORE_H

/**
 * @file
 * btn::core header file.
 *
 * @ingroup core
 */

#include "btn_span_fwd.h"
#include "btn_fixed_fwd.h"

namespace btn
{
    class string_view;
}

namespace btn::keypad
{
    enum class key_type;
}

/**
 * @brief Core related functions.
 *
 * @ingroup core
 */
namespace btn::core
{
    /**
     * @brief This function must be called before using butano, and it must be called only once.
     */
    void init();

    /**
     * @brief This function must be called before using butano, and it must be called only once.
     * @param keypad_commands Keypad commands recorded with the keypad logger.
     *
     * Instead of reading the keypad of the GBA, these keypad commands are replayed.
     */
    void init(const string_view& keypad_commands);

    /**
     * @brief Updates the screen and all of butano's subsystems.
     */
    void update();

    /**
     * @brief Sleeps the GBA until the given keypad key is pressed.
     */
    void sleep(keypad::key_type wake_up_key);

    /**
     * @brief Sleeps the GBA until the given keypad keys are pressed.
     */
    void sleep(const span<const keypad::key_type>& wake_up_keys);

    /**
     * @brief Resets the GBA, going back to the start of main().
     *
     * Keep in mind that it doesn't rewind the stack, so alive objects are not destroyed.
     */
    [[noreturn]] void reset();

    /**
     * @brief Returns the current CPU usage.
     *
     * A CPU usage greater than 1 means that at least one screen refresh has been missed.
     */
    [[nodiscard]] fixed cpu_usage();

    /**
     * @brief Returns the current V-Blank usage.
     *
     * A V-Blank usage greater than 1 means that the screen is being redrawn
     * before all of GBA display components being updated.
     */
    [[nodiscard]] fixed vblank_usage();
}

#endif
