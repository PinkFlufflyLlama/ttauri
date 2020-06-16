// Copyright 2019 Pokitec
// All rights reserved.

#include "TTauri/Widgets/ImageWidget.hpp"
#include "TTauri/GUI/utils.hpp"
#include "TTauri/Foundation/utils.hpp"
#include <cmath>
#include <typeinfo>

namespace tt {

ImageWidget::ImageWidget(Window &window, Widget *parent, PixelMap<R16G16B16A16SFloat> image) noexcept :
    Widget(window, parent, vec{Theme::height, Theme::height}),
    image(std::move(image))
{
}

void ImageWidget::layout(hires_utc_clock::time_point displayTimePoint) noexcept
{
    backingImage.draw(
        window,
        image.extent(),
        [this](auto &stagingImage) {
            copy(image, stagingImage);
        }
    );
}

void ImageWidget::draw(DrawContext const &drawContext, hires_utc_clock::time_point displayTimePoint) noexcept
{
    ttlet drawingBackingImage = backingImage.loadOrDraw(
        window,
        image.extent(),
        [this](auto &stagingImage) {
            return drawImage(stagingImage);
        },
        "ImageWidget", state()
    );

    if (drawingBackingImage) {
        forceRedraw = true;
    }

    if (backingImage.image) {
        ttlet currentScale = (extent() / vec{backingImage.image->extent}).xy11();

        auto context = drawContext;
        context.transform = context.transform * mat::S(currentScale);
        context.drawImage(*(backingImage.image));
    }

    Widget::draw(drawContext, displayTimePoint);
}

void ImageWidget::drawTrianglesOutward(Path &path, vec position, float radius) noexcept
{
    ttlet L = radius * 0.5;
    ttlet W = radius * 0.3;

    path.moveTo({position.x() - L, position.y() - L});
    path.lineTo({position.x() + W, position.y() - L});
    path.lineTo({position.x() - L, position.y() + W});
    path.closeContour();

    path.moveTo({position.x() + L, position.y() + L});
    path.lineTo({position.x() - W, position.y() + L});
    path.lineTo({position.x() + L, position.y() - W});
    path.closeContour();
}

void ImageWidget::drawTrianglesInward(Path &path, vec position, float radius) noexcept
{
    ttlet L = radius * 0.8;

    path.moveTo({position.x(), position.y()});
    path.lineTo({position.x() - L, position.y()});
    path.lineTo({position.x(), position.y() - L});
    path.closeContour();

    path.moveTo({position.x(), position.y()});
    path.lineTo({position.x() + L, position.y()});
    path.lineTo({position.x(), position.y() + L});
    path.closeContour();
}

void ImageWidget::drawCross(Path &path, vec position, float radius) noexcept
{
    ttlet W = sqrt(0.5);
    ttlet L = radius * 0.5;
    
    // Left bottom line.
    path.moveTo({position.x() - W, position.y()});
    path.lineTo({position.x() - L, position.y() - L + W});
    path.lineTo({position.x() - L + W, position.y() - L});
    path.lineTo({position.x(), position.y() - W});

    // Right bottom line.
    path.lineTo({position.x() + L - W, position.y() - L});
    path.lineTo({position.x() + L, position.y() - L + W});
    path.lineTo({position.x() + W, position.y()});

    // Right top line.
    path.lineTo({position.x() + L, position.y() + L - W});
    path.lineTo({position.x() + L - W, position.y() + L});
    path.lineTo({position.x(), position.y() + W});

    // Left top line.
    path.lineTo({position.x() - L + W, position.y() + L});
    path.lineTo({position.x() - L, position.y() + L - W});

    path.closeContour();
}

PixelMap<R16G16B16A16SFloat> ImageWidget::drawApplicationIconImage(PipelineImage::Image &image) noexcept
{
    auto linearMap = PixelMap<R16G16B16A16SFloat>{image.extent};
    fill(linearMap);

    ttlet iconPath = applicationIcon.centerScale(vec{image.extent}, 3.0);
    composit(linearMap, iconPath);

    if (!window.active) {
        desaturate(linearMap, 0.5f);
    }
    return linearMap;
}

PixelMap<R16G16B16A16SFloat> ImageWidget::drawTrafficLightsImage(PipelineImage::Image &image) noexcept
{
    ttlet redCenter = vec{
        MARGIN + RADIUS,
        extent().height() / 2.0
    };
    ttlet yellowCenter = vec{
        MARGIN + DIAMETER + SPACING + RADIUS,
        extent().height() / 2.0
    };
    ttlet greenCenter = vec{
        MARGIN + DIAMETER + SPACING + DIAMETER + SPACING + RADIUS,
        extent().height() / 2.0
    };

    auto drawing = Path();
    drawing.addCircle(redCenter, RADIUS);
    
    if (!window.active && !hover) {
        drawing.closeLayer(vec{0.246, 0.246, 0.246, 1.0});
    } else if (pressedRed) {
        drawing.closeLayer(vec{1.0, 0.242, 0.212, 1.0});
    } else {
        drawing.closeLayer(vec{1.0, 0.1, 0.082, 1.0});
    }

    drawing.addCircle(yellowCenter, RADIUS);
    if (!window.active && !hover) {
        drawing.closeLayer(vec{0.246, 0.246, 0.246, 1.0});
    } else if (pressedYellow) {
        drawing.closeLayer(vec{1.0, 0.847, 0.093, 1.0});
    } else {
        drawing.closeLayer(vec{0.784, 0.521, 0.021, 1.0});
    }

    drawing.addCircle(greenCenter, RADIUS);
    if (!window.active && !hover) {
        drawing.closeLayer(vec{0.246, 0.246, 0.246, 1.0});
    } else if (pressedGreen) {
        drawing.closeLayer(vec{0.223, 0.863, 0.1, 1.0});
    } else {
        drawing.closeLayer(vec{0.082, 0.533, 0.024, 1.0});
    }

    if (hover) {
        drawCross(drawing, redCenter, RADIUS);
        drawing.closeLayer(vec{0.319, 0.0, 0.0, 1.0});

        drawing.addRectangle({yellowCenter.x() - RADIUS * 0.5 - 0.5, yellowCenter.y() - 0.5, RADIUS * 1.0 + 1.0, 1.0});
        drawing.closeLayer(vec{0.212, 0.1, 0.0, 1.0});

        if (window.size == Window::Size::Maximized) {
            drawTrianglesInward(drawing, greenCenter, RADIUS);
        } else {
            drawTrianglesOutward(drawing, greenCenter, RADIUS);
        }
        drawing.closeLayer(vec{0.0, 0.133, 0.0, 1.0});
    }

    auto linearMap = PixelMap<R16G16B16A16SFloat>{image.extent};
    fill(linearMap);
    composit(linearMap, drawing);
    return linearMap;
}

PipelineImage::Backing::ImagePixelMap ImageWidget::drawImage(std::shared_ptr<PipelineImage::Image> image) noexcept
{
    if constexpr (operatingSystem == OperatingSystem::Windows) {
        return { std::move(image), drawApplicationIconImage(*image) };
    } else if constexpr (operatingSystem == OperatingSystem::MacOS) {
        return { std::move(image), drawTrafficLightsImage(*image) };
    } else {
        tt_no_default;
    }
}

std::tuple<aarect, aarect, aarect, aarect> ImageWidget::getButtonRectangles() const noexcept
{
    ttlet sysmenuButtonBox = aarect{
        vec::point(0.0, 0.0),
        {extent().height(), extent().height()}
    };

    ttlet redButtonBox = aarect{
        vec::point(MARGIN, MARGIN),
        {DIAMETER, DIAMETER}
    };

    ttlet yellowButtonBox = aarect{
        vec::point(MARGIN + DIAMETER + SPACING, MARGIN),
        {DIAMETER, DIAMETER}
    };

    ttlet greenButtonBox = aarect{
        vec::point(MARGIN + DIAMETER * 2.0 + SPACING * 2.0, MARGIN),
        {DIAMETER, DIAMETER}
    };

    return {redButtonBox, yellowButtonBox, greenButtonBox, sysmenuButtonBox};    
}

void ImageWidget::handleMouseEvent(MouseEvent const &event) noexcept
{
    Widget::handleMouseEvent(event);

    if constexpr (operatingSystem == OperatingSystem::Windows) {
        return;

    } else if constexpr (operatingSystem == OperatingSystem::MacOS) {
        ttlet [redButtonRect, yellowButtonRect, greenButtonRect, sysmenuButtonBox] = getButtonRectangles();

        if (event.type == MouseEvent::Type::ButtonUp && event.cause.leftButton) {
            if (pressedRed) {
                window.closeWindow();
            } else if (pressedYellow) {
                window.minimizeWindow();
            } else if (pressedGreen) {
                switch (window.size) {
                case Window::Size::Normal:
                    window.maximizeWindow();
                    break;
                case Window::Size::Maximized:
                    window.normalizeWindow();
                    break;
                default:
                    tt_no_default;
                }
            }
        }

        // Only change the pressed state after checking for Button Up, the
        // button up will check which button was pressed from button down.
        auto stateHasChanged = false;
        stateHasChanged |= assign_and_compare(pressedRed, event.down.leftButton && redButtonRect.contains(event.position));
        stateHasChanged |= assign_and_compare(pressedYellow, event.down.leftButton && yellowButtonRect.contains(event.position));
        stateHasChanged |= assign_and_compare(pressedGreen, event.down.leftButton && greenButtonRect.contains(event.position));
        if (stateHasChanged) {
            forceRedraw = true;
        }

    } else {
        tt_no_default;
    }
}

HitBox ImageWidget::hitBoxTest(vec position) const noexcept
{
    ttlet [redButtonRect, yellowButtonRect, greenButtonRect, sysmenuButtonBox] = getButtonRectangles();

    auto r = HitBox{};

    if constexpr (operatingSystem == OperatingSystem::Windows) {
        if (sysmenuButtonBox.contains(position)) {
            r = HitBox{this, elevation, HitBox::Type::ApplicationIcon};
        }

    } else if constexpr (operatingSystem == OperatingSystem::MacOS) {
        if (redButtonRect.contains(position) ||
            yellowButtonRect.contains(position) ||
            greenButtonRect.contains(position)
        ) {
            r = HitBox{this, elevation, HitBox::Type::Button};
        }

    } else {
        tt_no_default;
    }

    return r;
}

}