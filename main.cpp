#include <iostream>
#include "delay_line.hpp"

int main() {
   
    adi::dsp::DelayLine<4> delay_line;

  
    std::cout << "Inseram valorile: 10, 20, 30, 40, 50, 60\n\n";
    for (float sample : {10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f}) {
        delay_line.push(sample);
    }

   
    std::cout << "Continutul buffer-ului circular:\n";
    for (std::size_t k = 0; k < delay_line.capacity(); ++k) {
        std::cout << "x[n - " << k << "] = " << delay_line.get(k) << '\n';
    }

    return 0;
}