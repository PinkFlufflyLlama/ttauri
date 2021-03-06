// Copyright Take Vos 2020-2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "window_traffic_lights_widget.hpp"
#include "../GUI/utils.hpp"
#include "../text/ttauri_icon.hpp"
#include "../utils.hpp"
#include <cmath>
#include <typeinfo>

namespace tt {

window_traffic_lights_widget::window_traffic_lights_widget(
    gui_window &window,
    std::shared_ptr<abstract_container_widget> parent) noexcept :
    super(window, parent)
{
    // Toolbar buttons hug the toolbar and neighbor widgets.
    _margin = 0.0f;
}

[[nodiscard]] bool
window_traffic_lights_widget::update_constraints(hires_utc_clock::time_point display_time_point, bool need_reconstrain) noexcept
{
    tt_axiom(gui_system_mutex.recurse_lock_count());

    if (super::update_constraints(display_time_point, need_reconstrain)) {
        if constexpr (theme::global->operatingSystem == OperatingSystem::Windows) {
            ttlet width = theme::global->toolbarDecorationButtonWidth * 3.0f;
            ttlet height = theme::global->toolbarHeight;
            _preferred_size = {f32x4{width, height}, f32x4{width, std::numeric_limits<float>::infinity()}};

        } else if constexpr (theme::global->operatingSystem == OperatingSystem::MacOS) {
            ttlet width = DIAMETER * 3.0f + 2.0f * MARGIN + 2.0f * SPACING;
            ttlet height = DIAMETER + 2.0f * MARGIN;
            _preferred_size = {f32x4{width, height}, f32x4{width, std::numeric_limits<float>::infinity()}};

        } else {
            tt_no_default();
        }
        return true;
    } else {
        return false;
    }
}

[[nodiscard]] void
window_traffic_lights_widget::update_layout(hires_utc_clock::time_point display_time_point, bool need_layout) noexcept
{
    tt_axiom(gui_system_mutex.recurse_lock_count());

    need_layout |= std::exchange(_request_relayout, false);
    if (need_layout) {
        auto extent = rectangle().extent();
        if (extent.height() > theme::global->toolbarHeight * 1.2f) {
            extent = f32x4{extent.width(), theme::global->toolbarHeight};
        }
        auto y = rectangle().height() - extent.height();

        if constexpr (theme::global->operatingSystem == OperatingSystem::Windows) {
            closeRectangle =
                aarect{f32x4::point(extent.width() * 2.0f / 3.0f, y), f32x4{extent.width() * 1.0f / 3.0f, extent.height()}};

            maximizeRectangle =
                aarect{f32x4::point(extent.width() * 1.0f / 3.0f, y), f32x4{extent.width() * 1.0f / 3.0f, extent.height()}};

            minimizeRectangle = aarect{f32x4::point(0.0f, y), f32x4{extent.width() * 1.0f / 3.0f, extent.height()}};

        } else if constexpr (theme::global->operatingSystem == OperatingSystem::MacOS) {
            closeRectangle = aarect{f32x4::point(MARGIN, extent.height() / 2.0f - RADIUS), {DIAMETER, DIAMETER}};

            minimizeRectangle =
                aarect{f32x4::point(MARGIN + DIAMETER + SPACING, extent.height() / 2.0f - RADIUS), {DIAMETER, DIAMETER}};

            maximizeRectangle = aarect{
                f32x4::point(MARGIN + DIAMETER + SPACING + DIAMETER + SPACING, extent.height() / 2.0f - RADIUS),
                {DIAMETER, DIAMETER}};
        } else {
            tt_no_default();
        }

        closeWindowGlyph = to_font_glyph_ids(ttauri_icon::CloseWindow);
        minimizeWindowGlyph = to_font_glyph_ids(ttauri_icon::MinimizeWindow);

        if constexpr (theme::global->operatingSystem == OperatingSystem::Windows) {
            maximizeWindowGlyph = to_font_glyph_ids(ttauri_icon::MaximizeWindowMS);
            restoreWindowGlyph = to_font_glyph_ids(ttauri_icon::RestoreWindowMS);

        } else if constexpr (theme::global->operatingSystem == OperatingSystem::MacOS) {
            maximizeWindowGlyph = to_font_glyph_ids(ttauri_icon::MaximizeWindowMacOS);
            restoreWindowGlyph = to_font_glyph_ids(ttauri_icon::RestoreWindowMacOS);
        } else {
            tt_no_default();
        }

        ttlet closeWindowGlyphBB = pipeline_SDF::device_shared::getBoundingBox(closeWindowGlyph);
        ttlet minimizeWindowGlyphBB = pipeline_SDF::device_shared::getBoundingBox(minimizeWindowGlyph);
        ttlet maximizeWindowGlyphBB = pipeline_SDF::device_shared::getBoundingBox(maximizeWindowGlyph);
        ttlet restoreWindowGlyphBB = pipeline_SDF::device_shared::getBoundingBox(restoreWindowGlyph);

        ttlet glyph_size = theme::global->operatingSystem == OperatingSystem::MacOS ? 5.0f : theme::global->small_icon_size;

        closeWindowGlyphRectangle = align(closeRectangle, scale(closeWindowGlyphBB, glyph_size), alignment::middle_center);
        minimizeWindowGlyphRectangle =
            align(minimizeRectangle, scale(minimizeWindowGlyphBB, glyph_size), alignment::middle_center);
        maximizeWindowGlyphRectangle =
            align(maximizeRectangle, scale(maximizeWindowGlyphBB, glyph_size), alignment::middle_center);
        restoreWindowGlyphRectangle = align(maximizeRectangle, scale(restoreWindowGlyphBB, glyph_size), alignment::middle_center);
    }
    super::update_layout(display_time_point, need_layout);
}

void window_traffic_lights_widget::drawMacOS(draw_context const &drawContext, hires_utc_clock::time_point displayTimePoint) noexcept
{
    tt_axiom(gui_system_mutex.recurse_lock_count());

    auto context = drawContext;
    context.corner_shapes = f32x4{RADIUS, RADIUS, RADIUS, RADIUS};

    if (!window.active && !_hover) {
        context.fill_color = color(0.246f, 0.246f, 0.246f);
    } else if (pressedClose) {
        context.fill_color = color(1.0f, 0.242f, 0.212f);
    } else {
        context.fill_color = color(1.0f, 0.1f, 0.082f);
    }
    context.line_color = context.fill_color;
    context.draw_box_with_border_inside(closeRectangle);

    if (!window.active && !_hover) {
        context.fill_color = color(0.246f, 0.246f, 0.246f);
    } else if (pressedMinimize) {
        context.fill_color = color(1.0f, 0.847f, 0.093f);
    } else {
        context.fill_color = color(0.784f, 0.521f, 0.021f);
    }
    context.line_color = context.fill_color;
    context.draw_box_with_border_inside(minimizeRectangle);

    if (!window.active && !_hover) {
        context.fill_color = color(0.246f, 0.246f, 0.246f);
    } else if (pressedMaximize) {
        context.fill_color = color(0.223f, 0.863f, 0.1f);
    } else {
        context.fill_color = color(0.082f, 0.533f, 0.024f);
    }
    context.line_color = context.fill_color;
    context.draw_box_with_border_inside(maximizeRectangle);

    if (_hover) {
        context.transform = translate3{0.0f, 0.0f, 0.1f} * context.transform;
        context.line_color = color(0.319f, 0.0f, 0.0f);
        context.draw_glyph(closeWindowGlyph, closeWindowGlyphRectangle);

        context.line_color = color(0.212f, 0.1f, 0.0f);
        context.draw_glyph(minimizeWindowGlyph, minimizeWindowGlyphRectangle);

        context.line_color = color(0.0f, 0.133f, 0.0f);
        if (window.size_state == gui_window_size::maximized) {
            context.draw_glyph(restoreWindowGlyph, restoreWindowGlyphRectangle);
        } else {
            context.draw_glyph(maximizeWindowGlyph, maximizeWindowGlyphRectangle);
        }
    }
}

void window_traffic_lights_widget::drawWindows(
    draw_context const &drawContext,
    hires_utc_clock::time_point displayTimePoint) noexcept
{
    tt_axiom(gui_system_mutex.recurse_lock_count());

    auto context = drawContext;

    if (pressedClose) {
        context.fill_color = color(1.0f, 0.0f, 0.0f);
    } else if (hoverClose) {
        context.fill_color = color(0.5f, 0.0f, 0.0f);
    } else {
        context.fill_color = theme::global->fillColor(_semantic_layer);
    }
    context.draw_filled_quad(closeRectangle);

    if (pressedMinimize) {
        context.fill_color = theme::global->fillColor(_semantic_layer + 2);
    } else if (hoverMinimize) {
        context.fill_color = theme::global->fillColor(_semantic_layer + 1);
    } else {
        context.fill_color = theme::global->fillColor(_semantic_layer);
    }
    context.draw_filled_quad(minimizeRectangle);

    if (pressedMaximize) {
        context.fill_color = theme::global->fillColor(_semantic_layer + 2);
    } else if (hoverMaximize) {
        context.fill_color = theme::global->fillColor(_semantic_layer + 1);
    } else {
        context.fill_color = theme::global->fillColor(_semantic_layer);
    }
    context.draw_filled_quad(maximizeRectangle);

    if (window.active) {
        context.line_color = theme::global->foregroundColor;
    } else {
        context.line_color = theme::global->borderColor(_semantic_layer);
    }
    context.transform = translate3{0.0f, 0.0f, 0.1f} * context.transform;
    context.draw_glyph(closeWindowGlyph, closeWindowGlyphRectangle);
    context.draw_glyph(minimizeWindowGlyph, minimizeWindowGlyphRectangle);
    if (window.size_state == gui_window_size::maximized) {
        context.draw_glyph(restoreWindowGlyph, restoreWindowGlyphRectangle);
    } else {
        context.draw_glyph(maximizeWindowGlyph, maximizeWindowGlyphRectangle);
    }
}

void window_traffic_lights_widget::draw(draw_context context, hires_utc_clock::time_point display_time_point) noexcept
{
    tt_axiom(gui_system_mutex.recurse_lock_count());

    if (overlaps(context, this->window_clipping_rectangle())) {
        if constexpr (theme::global->operatingSystem == OperatingSystem::MacOS) {
            drawMacOS(context, display_time_point);

        } else if constexpr (theme::global->operatingSystem == OperatingSystem::Windows) {
            drawWindows(context, display_time_point);

        } else {
            tt_no_default();
        }
    }

    super::draw(std::move(context), display_time_point);
}

bool window_traffic_lights_widget::handle_event(mouse_event const &event) noexcept
{
    ttlet lock = std::scoped_lock(gui_system_mutex);
    auto handled = super::handle_event(event);

    // Check the hover states of each button.
    auto stateHasChanged = false;
    ttlet position = _from_window_transform * event.position;
    stateHasChanged |= compare_then_assign(hoverClose, closeRectangle.contains(position));
    stateHasChanged |= compare_then_assign(hoverMinimize, minimizeRectangle.contains(position));
    stateHasChanged |= compare_then_assign(hoverMaximize, maximizeRectangle.contains(position));
    if (stateHasChanged) {
        window.request_redraw(window_clipping_rectangle());
    }

    if (event.cause.leftButton) {
        handled = true;

        switch (event.type) {
            using enum mouse_event::Type;
        case ButtonUp:
            if (pressedClose && hoverClose) {
                window.close_window();
            }

            if (pressedMinimize && hoverMinimize) {
                window.minimize_window();
            }

            if (pressedMaximize && hoverMaximize) {
                switch (window.size_state) {
                case gui_window_size::normal: window.maximize_window(); break;
                case gui_window_size::maximized: window.normalize_window(); break;
                default: tt_no_default();
                }
            }

            window.request_redraw(window_clipping_rectangle());
            pressedClose = false;
            pressedMinimize = false;
            pressedMaximize = false;
            break;

        case ButtonDown:
            window.request_redraw(window_clipping_rectangle());
            pressedClose = hoverClose;
            pressedMinimize = hoverMinimize;
            pressedMaximize = hoverMaximize;
            break;
        }
    }

    return handled;
}

hit_box window_traffic_lights_widget::hitbox_test(f32x4 window_position) const noexcept
{
    ttlet lock = std::scoped_lock(gui_system_mutex);
    ttlet position = _from_window_transform * window_position;

    if (_window_clipping_rectangle.contains(window_position)) {
        if (closeRectangle.contains(position) || minimizeRectangle.contains(position) || maximizeRectangle.contains(position)) {
            return hit_box{weak_from_this(), _draw_layer, hit_box::Type::Button};
        } else {
            return hit_box{};
        }
    } else {
        return {};
    }
}

} // namespace tt
