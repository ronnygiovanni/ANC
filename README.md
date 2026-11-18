# Active Noise Cancellation (ANC) DSP Pipeline

Overview
This project focuses on building a "first-principles" Adaptive Digital Signal Processing (DSP) pipeline for Active Noise Cancellation. The core objective is to implement the Normalized Least Mean Squares (NLMS) algorithm from scratch in modern C++20, moving beyond black-box libraries to demonstrate strict adherence to real-time embedded systems constraints and online learning mathematics.

## Architecture
1. Algorithm Selection: Online Learning
Unlike traditional offline Machine Learning models that freeze after training, this system utilizes Online Learning (Gradient Descent). The environment changes every millisecond, requiring the model to continuously adapt its filter weights in real-time to simulate a physical dual-microphone ANC headset.

2. Memory Strategy: Zero Dynamic Allocation
To guarantee deterministic execution and avoid cache misses during the 44.1kHz audio processing loop, the real-time critical path is strictly isolated from heap allocations:
- AST-style Static Sizing: The system uses C++ templates (std::array) to define filter capacities at compile-time.
- O(1) Cyclic Buffering: The DelayLine component utilizes modular arithmetic to act as a ring buffer, preventing the CPU overhead of shifting memory arrays during runtime.

3. DSP Core: NLMS Filter
The NlmsFilter acts as the mathematical engine. At each sample cycle, it performs four sequential steps:
- Computes the FIR filter output (estimating the noise trajectory).
- Extracts the clean signal (calculating the error).
- Normalizes signal energy (incorporating epsilon safety nets to prevent hardware-level division-by-zero faults).
- Updates the mathematical weights via Gradient Descent based on a configurable learning rate.

4. I/O & File Management
The pipeline bridges PC file systems with the real-time DSP core using industry-standard utilities:
- Audio Processing: Integrates the dr_wav header-only library for precise bit-to-float conversions.
- Data Management: Utilizes C++17 std::filesystem to dynamically generate input/output directories and prevent hardcoded string anti-patterns.

Execution Modes
The algorithm requires two raw audio inputs to perform Blind Source Separation and cancel noise effectively. The pipeline enforces strict data hygiene by halting execution if the required dataset is missing.

- Reference Signal: Simulates the external headset microphone capturing pure environmental noise (e.g., motor hum).
- Primary Signal: Simulates the internal microphone capturing the user's voice contaminated by the leaked noise.
- Output: The pipeline subtracts the estimated noise from the primary signal, outputting the isolated clean voice.

Command Examples

## Tip
For testing, you must place your 44.1kHz Mono WAV files into the automatically generated 1_Intrari_Microfoane directory before running the pipeline. The executable will dynamically detect the filenames.

1. Run the pipeline (Windows):
.\build\Release\anc_test.exe

2. Run the pipeline (Linux/macOS):
./build/anc_test

## Project Roadmap
Phase 1: Hyperparameter Tuning (Python Integration)
- Connect the C++ executable to a Python script utilizing Optuna (TPESampler).
- Automate the discovery of the optimal NumTaps (filter capacity) and learning_rate by mathematically minimizing the residual noise energy in the output files.

Phase 2: Mathematical Benchmarking
- Implement alternative DSP algorithms, such as the standard LMS (faster but susceptible to instability) and RLS (Recursive Least Squares, faster convergence but computationally expensive matrix inversions).
- Compare execution time and convergence rates against the baseline NLMS filter.

Phase 3: Hardware Deployment
- Strip the WavIo offline utility and map the NlmsFilter directly to the ADC/DAC interrupts of an ARM Cortex-M microcontroller (e.g., STM32) for live analog audio processing.
