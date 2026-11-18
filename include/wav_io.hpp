#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

// Această macrocomandă îi spune bibliotecii dr_wav să scrie implementarea efectivă
// doar în acest fișier, pentru a evita erori de "multiple definition" la compilare.
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

namespace adi::dsp {

// Structură care reține datele extrase dintr-un fișier WAV
struct AudioData {
    std::vector<float> samples;
    unsigned int sample_rate;
    unsigned int channels;
};

class WavIo {
public:
    // Citește un fișier .wav și returnează datele (convertite la float)
    [[nodiscard]] static AudioData read(const std::string& filename) {
        unsigned int channels;
        unsigned int sample_rate;
        drwav_uint64 total_pcm_frame_count;

        // drwav_open_file_and_read_pcm_frames_f32 face toată magia:
        // Citește fișierul și convertește automat orice biți (16, 24, 32) în float-uri între -1.0 și +1.0
        float* p_sample_data = drwav_open_file_and_read_pcm_frames_f32(
            filename.c_str(), &channels, &sample_rate, &total_pcm_frame_count, nullptr);

        if (p_sample_data == nullptr) {
            throw std::runtime_error("Eroare la citirea fisierului WAV: " + filename);
        }

        // Copiem datele de pe pointer-ul brut (C-style) într-un std::vector curat (C++ modern)
        std::size_t total_samples = total_pcm_frame_count * channels;
        std::vector<float> audio_samples(p_sample_data, p_sample_data + total_samples);

        // Eliberăm memoria alocată de dr_wav
        drwav_free(p_sample_data, nullptr);

        return {audio_samples, sample_rate, channels};
    }

    // Scrie un std::vector de float-uri înapoi într-un fișier .wav pe hard disk
   // Scrie un std::vector de float-uri înapoi într-un fișier .wav pe hard disk
    static void write(const std::string& filename, const AudioData& data) {
        drwav_data_format format;
        format.container = drwav_container_riff;     // .wav normal
        format.format = DR_WAVE_FORMAT_IEEE_FLOAT;   // Salvăm direct ca float
        format.channels = data.channels;
        format.sampleRate = data.sample_rate;
        format.bitsPerSample = 32;                   // 32-bit float

        // Aici am schimbat drwav_open_file_write cu drwav_init_file_write
        drwav wav;
        if (!drwav_init_file_write(&wav, filename.c_str(), &format, nullptr)) {
            throw std::runtime_error("Eroare la deschiderea fisierului WAV pentru scriere: " + filename);
        }

        drwav_uint64 frames_to_write = data.samples.size() / data.channels;
        drwav_write_pcm_frames(&wav, frames_to_write, data.samples.data());

        // Aici am schimbat drwav_close cu drwav_uninit
        drwav_uninit(&wav);
    }
    
};

} // namespace adi::dsp