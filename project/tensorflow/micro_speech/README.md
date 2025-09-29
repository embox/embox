# Micro Speech – Keyword Spotting ("yes"/"no")

This example demonstrates an on-device keyword spotting model running on TensorFlow Lite Micro (TFLM) within Embox RTOS. It performs simple speech command recognition using an int8 quantized model and bundled test audio samples.

- Recognizes: "yes" vs "no" (plus silence/noise cases)
- Runs on: QEMU ARM, STM32 Discovery boards, and other supported targets
- Uses: TFLM int8 ops, compact arena, bundled test audio snippets

### Model architecture : 

![output_qemu](assets/micro_speech_quantized.png)

---

## Steps to Run (QEMU ARM)

1. Load the TFLite configuration for QEMU ARM:
```bash
make confload-project/tensorflow/tflm_arm_qemu | cat
```

2. Build the project:
```bash
make -j$(nproc) | cat
```

3. Run in QEMU after the build completes:
```bash
./scripts/qemu/auto_qemu
```

4. In the QEMU terminal, launch the application:
```bash
micro_speech
```

You should see log output with inference results over the included test clips (yes/no, silence/noise). On hardware targets with a microphone/audio pipeline, you can adapt input capture to use live audio.

### Example Output (QEMU)

![output_qemu](assets/output_qemu.jpg)



---

## What This Example Does

- Loads a pre-quantized TensorFlow Lite Micro model for keyword spotting
- Feeds test audio windows to the model
- Prints scores for the two classes ("yes" and "no") and diagnostics using `MicroPrintf`

---

## Model Details

- Format: Int8 quantized TFLite Micro
- Classes: yes, no (with additional silence/noise samples for testing)
- Files:
  - `models/micro_speech_quantized_model_data.cc/.h`: model weights as C arrays
  - `models/audio_preprocessor_int8_model_data.cc/.h`: optional preprocessor
  - `testdata/*.cc/.h`: embedded test audio samples (yes/no/silence/noise, 30ms and 1000ms)
- Typical ops used:
  - DepthwiseConv2D (int8)
  - Conv2D (int8)
  - AveragePool2D (int8)
  - Reshape
  - Softmax (int8)
- Input preprocessing:
  - The example is structured to support basic audio preprocessing. For live audio, add MFCC/feature extraction or reuse the provided audio preprocessor model.

---

## Memory and Performance Notes

- Tensor arena size is target-dependent; defaults are tuned to fit QEMU and STM32 Discovery boards.
- Int8 quantization minimizes RAM/flash usage and enables optimized kernels.
- If you see arena allocation failures, increase the arena size in the source or reduce model complexity.

---

## Running on Other Targets

Templates that include `micro_speech` by default:
- `project/tensorflow/templates/tflm_arm_qemu/mods.conf`
- `project/tensorflow/templates/tflm_riscv/mods.conf`
- `project/tensorflow/templates/x86_qemu/mods.conf`
- `project/tensorflow/templates/tflm_stm32f745i-discovery/mods.conf`
- `project/tensorflow/templates/tflm_stm32f746g-discovery/mods.conf`
- `project/tensorflow/templates/tflm_stm32f769i-discovery/mods.conf`

To use another template, substitute its name in step 1, for example:
```bash
make confload-project/tensorflow/tflm_riscv
make -j$(nproc) | cat
./scripts/qemu/auto_qemu
micro_speech
```

On physical STM32 boards, follow the board-specific flashing/run instructions in the corresponding template directories.

---

## Customize or Retrain

- Train your own small KWS model (yes/no or multiple keywords) using TensorFlow.
- Convert to TFLite (int8 quantized) and then to a C array:
```bash
xxd -i your_model.tflite > your_model_data.cc
```
- Replace the model data files in `models/` and ensure the resolver includes all required ops.

---

## Troubleshooting

- Arena allocation failed → Increase tensor arena size.
- Missing headers in editor → Use the Embox build; external include paths are supplied by `third_party.lib.tflite_micro` modules.
- No live audio input → Start with bundled `testdata/`; integrate an audio driver and preprocessing path for microphones.

---

## References

- TensorFlow Lite Micro repository (`tflite-micro`)
- Embox RTOS project templates under `project/tensorflow/templates/`
- Hello World example in `project/tensorflow/hello_world/README.md`
