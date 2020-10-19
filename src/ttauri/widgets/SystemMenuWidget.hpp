// Copyright 2019 Pokitec
// All rights reserved.

#pragma once

#include "Widget.hpp"
#include "../GUI/PipelineImage_Image.hpp"
#include "../Path.hpp"
#include "../cells/Image.hpp"
#include "../cells/ImageCell.hpp"
#include <memory>
#include <string>
#include <array>


namespace tt {

class SystemMenuWidget final : public Widget {
public:
    SystemMenuWidget(Window &window, Widget *parent, Image const &icon) noexcept;
    ~SystemMenuWidget() {}

    [[nodiscard]] bool update_constraints() noexcept override;
    [[nodiscard]] bool update_layout(hires_utc_clock::time_point display_time_point, bool need_layout) noexcept override;

    void draw(DrawContext context, hires_utc_clock::time_point display_time_point) noexcept override;

    [[nodiscard]] HitBox hitbox_test(vec window_position) const noexcept override;

private:
    std::unique_ptr<ImageCell> iconCell;

    aarect system_menu_rectangle;
};

}
