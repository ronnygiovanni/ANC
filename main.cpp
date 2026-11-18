#include <iostream>
#include <cmath>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include "wav_io.hpp"
#include "nlms_filter.hpp"

namespace fs = std::filesystem;

int main() {
    try {
        //Definim folderele
        fs::path dir_voices = "1_Voices";
        fs::path dir_distortions = "2_Distortions";
        fs::path dir_results = "3_Results";


        std::cout << "[1] Structura de foldere confirmata.\n";

        //Cautam orice fisier .wav in folderul 1_Voices
        fs::path voice_path;
        bool found_file = false;

        // Iteram prin toate fisierele din folder
        for (const auto& entry : fs::directory_iterator(dir_voices)) {
            if (entry.is_regular_file() && entry.path().extension() == ".wav") {
                voice_path = entry.path();
                found_file = true;
                break;
            }
        }

        if (!found_file) {
            std::cerr << "-> PAUZA: Folderul este gol sau nu contine niciun fisier .wav.\n"
                      << "Te rog sa pui ORICE fisier .wav in:\n" 
                      << fs::absolute(dir_voices) << '\n'
                      << "Apoi ruleaza programul din nou.\n";
            return 1; 
        }

        // Incarcam fisierul gasit (indiferent de nume)
        std::cout << "[2] Am detectat si incarcam: " << voice_path.filename().string() << "...\n";
        auto voice_audio = adi::dsp::WavIo::read(voice_path.string());
        
        if (voice_audio.channels != 1) {
            throw std::runtime_error("Fisierul trebuie sa fie Mono (1 canal), nu Stereo!");
        }

        const std::size_t num_samples = voice_audio.samples.size();
        const unsigned int sample_rate = voice_audio.sample_rate;

        // Pregatim structurile
        adi::dsp::AudioData ref_audio{{}, sample_rate, 1}; 
        adi::dsp::AudioData pri_audio{{}, sample_rate, 1}; 
        adi::dsp::AudioData out_audio{{}, sample_rate, 1}; 
        
        ref_audio.samples.reserve(num_samples);
        pri_audio.samples.reserve(num_samples);
        out_audio.samples.reserve(num_samples);

        // Generam distorsiunea de motor
        std::cout << "[3] Aplicam zgomotul industrial peste fisier...\n";
        for (std::size_t i = 0; i < num_samples; ++i) {
            const float t = static_cast<float>(i) / sample_rate;
            const float noise = 0.5f * std::sin(2.0f * 3.14159f * 50.0f * t) + 
                                0.3f * std::sin(2.0f * 3.14159f * 120.0f * t);
            
            ref_audio.samples.push_back(noise);
            pri_audio.samples.push_back(voice_audio.samples[i] + noise);
        }

        // Salvam fisierele de distorsiune, preluand numele fisierului original ca baza
        std::string base_name = voice_path.stem().string(); // extrage doar numele, fara ".wav"
        
        fs::path pure_noise_path = dir_distortions / (base_name + "_zgomot_pur.wav");
        fs::path contaminated_path = dir_distortions / (base_name + "_contaminat.wav");
        
        adi::dsp::WavIo::write(pure_noise_path.string(), ref_audio);
        adi::dsp::WavIo::write(contaminated_path.string(), pri_audio);

        // 5. Rulam filtrul NLMS
        std::cout << "[4] Rulam filtrul NLMS (128 taps) pentru procesare adaptiva...\n";
        adi::dsp::NlmsFilter<128> anc_filter(0.05f); 

        for (std::size_t i = 0; i < num_samples; ++i) {
            float clean_sample = anc_filter.process(ref_audio.samples[i], pri_audio.samples[i]);
            out_audio.samples.push_back(clean_sample);
        }

        // 6. Salvam rezultatul final curatat
        fs::path result_path = dir_results / (base_name + "_curatat.wav");
        adi::dsp::WavIo::write(result_path.string(), out_audio);
        
        std::cout << "[5] Succes! Fisierul rezultat se afla in:\n" << fs::absolute(dir_results) << '\n';

    } catch (const std::exception& e) {
        std::cerr << "Eroare: " << e.what() << '\n';
        return 1;
    }

    return 0;
}