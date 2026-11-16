#include <iostream>
#include "delay_line.hpp"

int main() {
    // Creăm un buffer care reține 4 eșantioane
    adi::dsp::DelayLine<4> delay_line;

    // Vin 6 eșantioane consecutive de la "ADC" (convertorul analog-digital)
    std::cout << "Inseram valorile: 10, 20, 30, 40, 50, 60\n\n";
    for (float sample : {10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f}) {
        delay_line.push(sample);
    }

    // Afișăm ce a rămas în memorie
    std::cout << "Continutul buffer-ului circular:\n";
    for (std::size_t k = 0; k < delay_line.capacity(); ++k) {
        std::cout << "x[n - " << k << "] = " << delay_line.get(k) << '\n';
    }

    return 0;
}