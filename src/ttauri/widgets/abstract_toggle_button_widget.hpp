// Copyright Take Vos 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include "abstract_button_widget.hpp"

namespace tt {

/** An abstract toggle button widget.
 * This widgets toggles a value from the true_value to false_value and back.
 */
template<typename T>
class abstract_toggle_button_widget : public abstract_button_widget<T> {
public:
    using super = abstract_button_widget<T>;
    using value_type = T;

    value_type const false_value;

    template<typename Value = observable<value_type>>
    abstract_toggle_button_widget(
        gui_window &window,
        std::shared_ptr<abstract_container_widget> parent,
        value_type true_value,
        value_type false_value,
        Value &&value = {}) noexcept :
        super(window, parent, true_value, std::forward<Value>(value)),
        false_value(std::move(false_value))
    {
        _value_callback = this->value.subscribe([this](auto...) {
            ttlet lock = std::scoped_lock(gui_system_mutex);
            this->window.request_redraw(aarect{this->local_to_window() * this->clipping_rectangle()});
        });
        _callback = this->subscribe([this]() {
            this->toggle();
        });
    }

    void toggle() noexcept
    {
        ttlet lock = std::scoped_lock(gui_system_mutex);

        if (compare_then_assign(this->value, this->value == this->false_value ? this->true_value : this->false_value)) {
            this->window.request_redraw(aarect{this->_local_to_window * this->_clipping_rectangle});
        }
    }

private:
    typename decltype(super::value)::callback_ptr_type _value_callback;
    typename super::callback_ptr_type _callback;
};

} // namespace tt