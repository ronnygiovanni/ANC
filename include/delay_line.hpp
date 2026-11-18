#pragma once

#include <array>
#include <cstddef>

namespace adi::dsp {

template <std::size_t Capacity>
class DelayLine {
    static_assert(Capacity > 0, "Capacitatea trebuie sa fie mai mare de 0");

public:
    constexpr DelayLine() noexcept {
        reset();
    }

  
    constexpr void reset() noexcept {
        buffer_.fill(0.0f);
        write_idx_ = 0;
    }


    constexpr void push(float sample) noexcept {
        buffer_[write_idx_] = sample;
    
        write_idx_ = (write_idx_ + 1) % Capacity;
    }

    
    // delay = 0 îți dă ultimul eșantion introdus x[n]
    // delay = 1 îți dă x[n-1], etc.
    [[nodiscard]] constexpr float get(std::size_t delay) const noexcept {

        const std::size_t index = (write_idx_ + Capacity - 1 - delay) % Capacity;
        return buffer_[index];
    }

    [[nodiscard]] constexpr std::size_t capacity() const noexcept {
        return Capacity;
    }

private:
    std::array<float, Capacity> buffer_{};
    std::size_t write_idx_{0};
};

} 