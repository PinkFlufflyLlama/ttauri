// Copyright 2019 Pokitec
// All rights reserved.

#pragma once

#include "TTauri/GUI/globals.hpp"
#include "TTauri/GUI/WindowDelegate.hpp"
#include "TTauri/GUI/Widget_forward.hpp"
#include "TTauri/GUI/Device_forward.hpp"
#include "TTauri/GUI/Cursor.hpp"
#include "TTauri/GUI/HitBox.hpp"
#include "TTauri/GUI/MouseEvent.hpp"
#include "TTauri/GUI/KeyboardEvent.hpp"
#include "TTauri/GUI/SubpixelOrientation.hpp"
#include "TTauri/Text/gstring.hpp"
#include "TTauri/Foundation/logger.hpp"
#include "TTauri/Foundation/vec.hpp"
#include "TTauri/Foundation/aarect.hpp"
#include "TTauri/Foundation/ivec.hpp"
#include "TTauri/Foundation/iaarect.hpp"
#include "TTauri/Foundation/Trigger.hpp"
#include "TTauri/Foundation/cpu_utc_clock.hpp"
#include <rhea/simplex_solver.hpp>
#include <unordered_set>
#include <memory>
#include <mutex>

namespace TTauri::GUI {

/*! A Window.
 * This Window is backed by a native operating system window with a Vulkan surface.
 * The Window should not have any decorations, which are to be drawn by the GUI, because
 * modern design requires drawing of user interface elements in the border.
 */
class Window_base {
public:
    enum class State {
        Initializing, //!< The window has not been initialized yet.
        NoWindow, //!< The window was destroyed, the device will drop the window on the next render cycle.
        NoDevice, //!< No device is associated with the Window and can therefor not be rendered on.
        NoSurface, //!< Need to request a new surface before building a swapchain
        NoSwapchain, //! Need to request a swapchain before rendering.
        ReadyToRender, //!< The swapchain is ready drawing is allowed.
        SwapchainLost, //!< The window was resized, the swapchain needs to be rebuild and can not be rendered on.
        SurfaceLost, //!< The Vulkan surface on the window was destroyed.
        DeviceLost, //!< The device was lost, but the window could move to a new device, or the device can be recreated.
        WindowLost, //!< The window was destroyed, need to cleanup.
    };

    enum class Size {
        Normal,
        Minimized,
        Maximized
    };

    State state = State::NoDevice;

    /** The current cursor.
     * Used for optimizing when the operating system cursor is updated.
     * Set to Cursor::None at the start (for the wait icon) and when the
     * operating system is going to display another icon to make sure
     * when it comes back in the application the cursor will be updated
     * correctly.
     */
    Cursor currentCursor = Cursor::None;

    /** When set to true the widgets will be layed out.
     */
    std::atomic<bool> forceLayout = true;

    /** When set to true the widgets will be redrawn.
    */
    std::atomic<bool> forceRedraw = true;

    /*! The window is currently being resized by the user.
     * We can disable expensive redraws during rendering until this
     * is false again.
     */
    std::atomic<bool> resizing = false;

    /*! The window is currently active.
     * Widgets may want to reduce redraws, or change colors.
     */
    std::atomic<bool> active = false;

    /*! Current size state of the window.
     */
    Size size = Size::Normal;

    //! The minimum window extent as calculated by laying out all the widgets.
    ivec minimumWindowExtent;

    //! The maximum window extent as calculated by laying out all the widgets.
    ivec maximumWindowExtent;

    //! The current window extent as set by the GPU library.
    ivec currentWindowExtent;

    std::shared_ptr<WindowDelegate> delegate;

    std::string title;

    Device *device = nullptr;

    /*! Orientation of the RGB subpixels.
     */
    SubpixelOrientation subpixelOrientation = SubpixelOrientation::BlueRight;

    /*! Dots-per-inch of the screen where the window is located.
     * If the window is located on multiple screens then one of the screens is used as
     * the source for the DPI value.
     */
    float dpi = 72.0;

    /** By how much the font needs to be scaled compared to current windowScale.
     * Widgets should pass this value to the text-shaper.
     */
    [[nodiscard]] float fontScale() const noexcept {
        return dpi / (windowScale() * 72.0f);
    }

    //! The widget covering the complete window.
    std::unique_ptr<Widgets::Widget,WidgetDeleter> widget;

    /** Target of the mouse
     * Since any mouse event will change the target this is used
     * to check if the target has changed, to send exit events to the previous mouse target.
     */
    Widgets::Widget *mouseTargetWidget = nullptr;

    /** Target of the keyboard
     * widget where keyboard events are sent to.
     */
    Widgets::Widget *keyboardTargetWidget = nullptr;

    /** The first widget in the window that needs to be selected.
     * This widget is selected when the window is opened and when
     * pressing tab when no other widget is selected.
     */
    Widgets::Widget *firstKeyboardWidget = nullptr;

    /** The first widget in the window that needs to be selected.
     * This widget is selected when pressing shift-tab when no other widget is selected.
     */
    Widgets::Widget *lastKeyboardWidget = nullptr;

    Window_base(const std::shared_ptr<WindowDelegate> delegate, const std::string title);
    virtual ~Window_base();

    Window_base(const Window_base &) = delete;
    Window_base &operator=(const Window_base &) = delete;
    Window_base(Window_base &&) = delete;
    Window_base &operator=(Window_base &&) = delete;

    virtual void initialize();

    /*! Set GPU device to manage this window.
     * Change of the device may be done at runtime.
     */
    void setDevice(Device *device);

    /*! Remove the GPU device from the window, making it an orphan.
     */
    void unsetDevice() { setDevice({}); }

    void layout(hires_utc_clock::time_point displayTimePoint);

    /** Layout the widgets in the window.
     * @return bit 0: Request redraw, bit 1: Request layout.
     */
    int layoutChildren(hires_utc_clock::time_point displayTimePoint, bool force);

    /*! Update window.
     * This will update animations and redraw all widgets managed by this window.
     */
    virtual void render(hires_utc_clock::time_point displayTimePoint) = 0;

    bool isClosed();

    template<typename T, typename... Args>
    T &addWidget(Args &&... args) {
        ttauri_assume(widget);
        return widget->addWidget<T>(std::forward<Args>(args)...);
    }

    rhea::constraint addConstraint(rhea::constraint const& constraint) noexcept;

    rhea::constraint addConstraint(
        rhea::linear_equation const& equation,
        rhea::strength const &strength = rhea::strength::required(),
        double weight = 1.0
    ) noexcept;

    rhea::constraint addConstraint(
        rhea::linear_inequality const& equation,
        rhea::strength const &strength = rhea::strength::required(),
        double weight = 1.0
    ) noexcept;

    void removeConstraint(rhea::constraint const& constraint) noexcept;

    rhea::constraint replaceConstraint(
        rhea::constraint const &oldConstraint,
        rhea::constraint const &newConstraint
    ) noexcept;

    rhea::constraint replaceConstraint(
        rhea::constraint const &oldConstraint,
        rhea::linear_equation const &newEquation,
        rhea::strength const &strength = rhea::strength::required(),
        double weight = 1.0
    ) noexcept;

    rhea::constraint replaceConstraint(
        rhea::constraint const &oldConstraint,
        rhea::linear_inequality const &newEquation,
        rhea::strength const &strength = rhea::strength::required(),
        double weight = 1.0
    ) noexcept;

    virtual void setCursor(Cursor cursor) = 0;

    virtual void closeWindow() = 0;

    virtual void minimizeWindow() = 0;
    virtual void maximizeWindow() = 0;
    virtual void normalizeWindow() = 0;
    virtual void setWindowSize(ivec extent) = 0;

    [[nodiscard]] virtual std::string getTextFromClipboard() const noexcept = 0;
    virtual void setTextOnClipboard(std::string str) noexcept = 0;

    void updateToNextKeyboardTarget(Widgets::Widget *currentTargetWidget) noexcept;

    void updateToPrevKeyboardTarget(Widgets::Widget *currentTargetWidget) noexcept;

protected:

    /*! The current rectangle which has been set by the operating system.
     * This value may lag behind the actual window extent as seen by the GPU
     * library. This value should only be read by the GPU library during
     * resize to determine the extent of the surface when the GPU library can
     * not figure this out by itself.
     */
    iaarect OSWindowRectangle;

    /** By how much graphic elements should be scaled to match a point.
    * The widget should not care much about this value, since the
    * transformation matrix will match the window scaling.
    */
    [[nodiscard]] float windowScale() const noexcept;

    /*! Called when the GPU library has changed the window size.
     */
    virtual void windowChangedSize(ivec extent);

    /*! call openingWindow() on the delegate. 
     */
    virtual void openingWindow();

    /*! call closingWindow() on the delegate.
     */
    virtual void closingWindow();

    /*! Teardown Window based on State::*_LOST
     */
    virtual void teardown() = 0;

    /*! Build Windows based on State::NO_*
     */
    virtual void build() = 0;

    void updateMouseTarget(Widgets::Widget const *newTargetWidget) noexcept;

    void updateKeyboardTarget(Widgets::Widget const *newTargetWidget) noexcept;

    /*! Mouse moved.
     * Called by the operating system to show the position of the mouse.
     * This is called very often so it must be made efficient.
     * Most often this function is used to determine the mouse cursor.
     */
    void handleMouseEvent(MouseEvent event) noexcept;

    /*! Handle keyboard event.
    * Called by the operating system to show the character that was entered
    * or special key that was used.
    */
    void handleKeyboardEvent(KeyboardEvent const &event) noexcept;

    void handleKeyboardEvent(KeyboardState _state, KeyboardModifiers modifiers, KeyboardVirtualKey key) noexcept;

    void handleKeyboardEvent(Text::Grapheme grapheme, bool full=true) noexcept;

    void handleKeyboardEvent(char32_t c, bool full=true) noexcept;

    /*! Test where the certain features of a window are located.
     */
    HitBox hitBoxTest(vec position) const noexcept;

private:
    //! This solver determines size and position of all widgets in this window.
    rhea::simplex_solver widgetSolver;

    /** Constraints have been updated.
    */
    bool constraintsUpdated = false;

    //! Stay constraint for the currentWindowExtent width.
    rhea::constraint currentWindowExtentWidthConstraint;

    //! Stay constraint for the currentWindowExtent height.
    rhea::constraint currentWindowExtentHeightConstraint;

    void setWidgetToCurrentExtent();

    void calculateMinimumAndMaximumWindowExtent();


};

}
