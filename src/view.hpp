// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2020 Artem Senichev <artemsen@gmail.com>

#pragma once

#include <cstddef>

/** @brief View mode. */
class View {
public:
    virtual ~View() = default;

    enum class MouseButton { left, middle, right };

    /**
     * @brief Draw scene.
     *
     * @return true if it is needed to redraw the window (animation in progress)
     */
    virtual bool draw() = 0;

    /**
     * @brief Click / touch handler.
     *
     * @param[in] x horizontal screen coordinate
     * @param[in] y vertical screen coordinate
     * @param[in] btn button type (left for touch)
     *
     * @return true if view mode must be swapped
     */
    virtual bool on_click(size_t x, size_t y, MouseButton btn) = 0;
};

/** @brief Level view (game mode). */
class ViewLevel : public View {
public:
    ViewLevel();
    bool draw() override;
    bool on_click(size_t x, size_t y, MouseButton btn) override;
};

/** @brief Settings view (setup mode). */
class ViewSettings : public View {
public:
    bool draw() override;
    bool on_click(size_t x, size_t y, MouseButton btn) override;
};
