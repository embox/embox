# People Detection – Person/No-Person on Embedded Targets

This example demonstrates on-device person detection using TensorFlow Lite Micro (TFLM) on Embox RTOS. It runs an int8-quantized tiny vision model on grayscale image windows and reports whether a person is present.

- Task: Binary classification – person vs no-person
- Format: TFLM (int8) with optimized kernels
- Targets: STM32 Discovery boards, QEMU ARM, x86, and RISC-V (template dependent)

---

## Steps to Run (QEMU ARM)

1. Load the TFLite template for QEMU ARM:
```bash
make confload-project/tensorflow/tflm_arm_qemu | cat
```

2. Build the project:
```bash
make -j$(nproc) | cat
```

3. Run in QEMU:
```bash
./scripts/qemu/auto_qemu
```

4. In the QEMU terminal, launch the application:
```bash
people_detection
```

You will see console logs with scores and decisions for the bundled test image(s). On capable hardware targets, you can implement live camera input by adapting `image_provider.*`.

---

## What This Example Does

- Loads an int8 TFLM person detection model
- Initializes an op resolver with the required operators
- Acquires an input image (from bundled testdata or a provider)
- Invokes the model and prints per-frame scores via `MicroPrintf`

---

## Model and Files

- Model: `models/person_detect_model_data.cc/.h` – int8 model embedded as a C array
- Utility and app sources:
  - `main.cc`, `main_functions.cc`, `model_settings.*`
  - `image_provider.*` (stub/provider for image capture)
  - `detection_responder.*` (handles output/logging)
- Typical operators used (int8):
  - DepthwiseConv2D, Conv2D, AveragePool2D, Reshape, Softmax

---

## Memory and Performance Notes

- The model requires a larger tensor arena than audio/sine examples. If you observe `AllocateTensors` failures, increase the arena size in `main_functions.cc`.
- Int8 quantization reduces RAM/flash while enabling optimized kernels (e.g., for ARM/AVX paths via ruy/gemmlowp).
- On STM32 boards, ensure sufficient heap in the template configuration.

---

## Running on Other Targets

Templates that commonly include this example:
- `project/tensorflow/templates/tflm_stm32f745i-discovery/mods.conf` (enabled)
- `project/tensorflow/templates/x86_qemu/mods.conf` (enabled)
- Others (ARM QEMU, RISC-V) can include it by adding:
```conf
include project.tensorflow.people_detection.people_detection
```
…to the corresponding `mods.conf` after the TFLM dependencies.

Then rebuild and run the same steps as above for that template.

---

## Using Real Images / Camera

- Implement `GetImage(...)` in `image_provider.*` to acquire frames from a camera/sensor.
- Convert/crop/resize to the expected model input size (see `model_settings.*`).
- Ensure buffer layout (int8) matches the model’s expected input tensor.

---

## Customize or Retrain

- You can replace the embedded model with your own tiny person/no-person detector or a multiclass detector trained in TensorFlow.
- Convert to TFLite int8 and then to a C array for embedding:
```bash
xxd -i your_person_model.tflite > person_detect_model_data.cc
```
- Update any input size/channel assumptions in `model_settings.*` and verify all required ops are registered in the resolver.

---

## Troubleshooting

- Arena allocation failed → Increase tensor arena size; reduce model size or input resolution.
- Missing headers in editor → Build via Embox; external include paths are injected by `third_party.lib.tflite_micro` modules.
- Low performance on QEMU → This is expected; use a real board for accurate performance.
- Wrong scores → Verify image pre-processing (size, channels, normalization) matches training.

---

## References

- TensorFlow Lite Micro (tflite-micro)
- Example sources under `project/tensorflow/people_detection/`
- Template configs under `project/tensorflow/templates/`
- See also `hello_world` and `micro_speech` READMEs for workflow and tips
