#pragma once

#include <array>
#include <cstddef>

namespace adi::dsp {

// Folosim un template pentru capacitate ca să forțăm compilatorul 
// să rezerve memoria fixă dinainte, evitând alocările pe Heap.
template <std::size_t Capacity>
class DelayLine {
    static_assert(Capacity > 0, "Capacitatea trebuie sa fie mai mare de 0");

public:
    constexpr DelayLine() noexcept {
        reset();
    }

    // Curăță buffer-ul (util când repornești filtrul)
    constexpr void reset() noexcept {
        buffer_.fill(0.0f);
        write_idx_ = 0;
    }

    // Inserează un eșantion nou (ex: citit de la microfon/WAV)
    constexpr void push(float sample) noexcept {
        buffer_[write_idx_] = sample;
        // Aritmetică modulară: indexul se învârte în cerc de la 0 la Capacity - 1
        write_idx_ = (write_idx_ + 1) % Capacity;
    }

    // Citește eșantioanele din trecut.
    // delay = 0 îți dă ultimul eșantion introdus x[n]
    // delay = 1 îți dă x[n-1], etc.
    [[nodiscard]] constexpr float get(std::size_t delay) const noexcept {
        // Truc de interviu: adăugăm `Capacity` înainte să scădem `delay` 
        // pentru a garanta că nu avem un rezultat negativ (size_t este unsigned și ar face overflow)
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

} // namespace adi::dsp