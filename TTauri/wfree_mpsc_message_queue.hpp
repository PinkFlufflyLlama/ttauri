// Copyright 2019 Pokitec
// All rights reserved.

#pragma once

#include "required.hpp"
#include "atomic.hpp"
#include <array>
#include <atomic>
#include <memory>
#include <thread>
#include <optional>
#include <type_traits>

namespace TTauri {

template<typename T, int64_t N>
class wfree_mpsc_message_queue {
    enum class message_state { Empty, Copying, Ready };

    struct message_type {
        T value;
        std::atomic<message_state> state = message_state::Empty;
    };

    using value_type = T;
    static constexpr int64_t capacity = N;
    
    std::array<message_type,capacity> messages;
    std::atomic<int64_t> head = 0;
    std::atomic<int64_t> tail = 0;

public:
    /*! Return the number of items in the message queue.
    * For the consumer this may show less items in the queue then there realy are.
    */
    int64_t size() const {
        // head and tail are extremelly large integers, they will never wrap arround.
        return head.load(std::memory_order_relaxed) - tail.load(std::memory_order_relaxed);
    }

    /*!
    * Wait free if queue is not full.
    * Blocks until queue is not full.
    */
    template<typename O>
    void insert(O &&value) noexcept {
        let index = head.fetch_add(1, std::memory_order_acquire);
        auto &message = messages[index % capacity];

        // We acquired the index before we knew if the queue was full.
        // It is assumed that the capacity of the queue is less than the number of threads.
        transition(message.state, message_state::Empty, message_state::Copying, std::memory_order_acquire);
        message.value = std::forward<O>(value);
        message.state.store(message_state::Ready, std::memory_order_release);
    }

    T &peek() noexcept {
        let index = tail.load(std::memory_order_acquire);
        auto &message = messages[index % capacity];

        // We acquired the index before we knew if the message was ready.
        wait_for_transition(message.state, message_state::Ready, std::memory_order_acquire);
        return message.value;
    }

    void pop() noexcept {
        let index = tail.fetch_add(1, std::memory_order_acquire);
        auto &message = messages[index % capacity];

        // We acquired the index before we knew if the message was ready.
        transition(message.state, message_state::Ready, message_state::Empty, std::memory_order_release);
        // The message itself does not need to be destructed.
        // This will happen automatically when wrapping around the ring buffer overwrites the message.
    }

    T pop_value() noexcept {
        static_assert(std::is_move_constructible_v<T>, "messages inside the message queue must be move-constructable for pop_value().");

        let index = tail.fetch_add(1, std::memory_order_acquire);
        auto &message = messages[index % capacity];

        // We acquired the index before we knew if the message was ready.
        wait_for_transition(message.state, message_state::Ready, std::memory_order_acquire);
        let value = std::move(message.value);
        message.state.store(message_state::Empty, std::memory_order_release);
        return std::move(value);
    }
};

}
