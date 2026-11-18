#pragma once

#include <array>
#include <cstddef>
#include "delay_line.hpp"

namespace adi::dsp {

template <std::size_t NumTaps>
class NlmsFilter {
    static_assert(NumTaps > 0, "Numarul de coeficienti trebuie sa fie mai mare decat 0");

public:
    // Constructorul setează rata de învățare (pasul de adaptare)
    constexpr explicit NlmsFilter(float learning_rate = 0.1f) noexcept 
        : learning_rate_{learning_rate} {
        weights_.fill(0.0f);
    }

    // Funcția principală: procesează un singur eșantion la un moment dat
    // reference_noise = x[n] (zgomotul captat de microfonul exterior)
    // primary_signal  = d[n] (vocea ta + zgomot, de la microfonul de lângă gură)
    // Returnează: semnalul curățat (care matematic este "eroarea" e[n])
    [[nodiscard]] constexpr float process(float reference_noise, float primary_signal) noexcept {
        
        // 1. Introducem noul eșantion de zgomot în linia de întârziere
        x_buffer_.push(reference_noise);

        // AICI VEI SCRIE TU MATEMATICA (vezi instrucțiunile de mai jos)
        float estimated_noise = 0.0f; // y[n]
        float error = 0.0f;           // e[n]
        float energy = 0.0f;          // E

        for (std::size_t k = 0; k < NumTaps; ++k) {
            float x_k = x_buffer_.get(k); // Salvăm eșantionul ca să nu apelăm get(k) de două ori
            estimated_noise += weights_[k] * x_k;
            energy += x_k * x_k;
        }

        // 2. Calculul Erorii (Signal Extraction)
        // Scădem zgomotul estimat din semnalul principal. Ce rămâne e semnalul dorit.
        error = primary_signal - estimated_noise;

        // Optimizare hardware: Împărțirea este foarte lentă.
        // Calculăm fracția o singură dată aici, apoi facem doar înmulțiri în buclă.
        float step_size = learning_rate_ / (energy + epsilon_);

        // 4. Adaptarea Greutăților (Învățarea)
        for (std::size_t k = 0; k < NumTaps; ++k) {
            weights_[k] += step_size * error * x_buffer_.get(k);
        }

        return error; // Eroarea este exact vocea ta curată!
    }

private:
    DelayLine<NumTaps> x_buffer_{};          // Memoria semnalului de referință
    std::array<float, NumTaps> weights_{};   // Coeficienții filtrului (w)
    float learning_rate_{0.1f};              // Viteza de adaptare (mu)
    
    // O constantă mică adunată la energie pentru a preveni împărțirea la ZERO
    static constexpr float epsilon_ = 1e-8f; 
};

} // namespace adi::dsp