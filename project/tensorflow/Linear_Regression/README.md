# Custom Model – End‑to‑End: Train, Convert, Deploy (Linear Regression)

This guide shows how to create your own TinyML model from scratch and deploy it on Embox RTOS using TensorFlow Lite Micro (TFLM). We’ll use a minimal Linear Regression example to demonstrate the full workflow: train → convert → embed → build → run.

---

## 1) Train a Simple Model (Python)

Create and train a tiny model that maps x → y (e.g., y ≈ ax + b). You can do this with pure NumPy/Scikit‑learn or TensorFlow. Below is a minimal TensorFlow example that exports a TFLite model (float), then optionally quantizes to int8.


---

## 2) Convert the .tflite File to a C Array

Use `xxd -i` to embed the model into your firmware as a C array:

```bash
xxd -i linear_regression.tflite > Linear_regression_model_data.cc
# or for int8 model
xxd -i linear_regression_int8.tflite > Linear_regression_model_data.cc
```

Place the resulting file in:
```
project/tensorflow/Linear_Regression/models/Linear_regression_model_data.cc
```

Ensure the symbols in the generated file match those used by the app. If needed, adjust the symbol names (e.g., `g_linear_regression_model_data`) to match includes in `main_functions.cc` and `models/Linear_regression_model_data.h`.

---

## 3) App Sources and Build Integration

This example is already wired as an Embox command:
- Sources: `main.cc`, `main_functions.cc`, `output_handler.*`, and `models/Linear_regression_model_data.*`
- Build file: `project/tensorflow/Linear_Regression/Mybuild`
- Command name: `linear_regression`

If you add new source files or rename symbols, update `Mybuild` accordingly:
```text
package project.tensorflow.linear_regression
@AutoCmd
@Cmd(name = "linear_regression", help = "TinyML linear regression demo")
@Build(stage=2)
@BuildDepends(third_party.lib.tflite_micro.libtflm)
@BuildDepends(third_party.lib.tflite_micro.tflite_micro_build_headers)

module linear_regression {
    @Cflags("-Wno-undef")
    @Cflags("-Imodels")

    source "main.cc"
    source "main_functions.cc"
    source "output_handler.cc"
    source "models/Linear_regression_model_data.cc"
}
```

---

## 3.1) Code Structure

- `main_functions.cc`:
  - Sets up TFLM: loads the model (`g_linear_regression_model_data`), checks `TFLITE_SCHEMA_VERSION`.
  - Defines tensor arena and creates `tflite::MicroInterpreter`.
  - Registers only required ops (e.g., `FullyConnected`, `Add`).
  - Provides `init_model()` and `run_inference()`; the latter feeds random inputs and prints outputs.

- `output_handler.h/.cc`:
  - Minimal printing helpers (stdout) to display input/output values.
  - Kept free of TensorFlow headers in the header to avoid include propagation.

- `main.cc`:
  - Standard C entry point.
  - Calls `init_model()` and `run_inference()` once (can be looped if desired).

- `models/Linear_regression_model_data.cc/.h`:
  - C array with the embedded TFLite model produced by `xxd -i`.
  - Ensure symbol names match those referenced in `main_functions.cc`.

---

## 4) Enable in a Template (e.g., QEMU ARM)

The TensorFlow templates include TFLM and example apps. `linear_regression` is already enabled in ARM QEMU. To build and run:

```bash
make confload-project/tensorflow/tflm_arm_qemu | cat
make -j$(nproc) | cat
./scripts/qemu/auto_qemu
```

Then in the QEMU shell:
```bash
linear_regression
```

You should see lines similar to:
```
Embox TFLite Linear Regression Demo
Input: 0.123 -> Output: 0.742
...
```

To enable on other templates, add to the template’s `mods.conf` after TFLM deps:
```conf
include project.tensorflow.linear_regression.linear_regression
```

---

## 5) Match Ops to Your Model

The example uses a minimal resolver (e.g., `Add`, `FullyConnected`). If your model uses other ops (e.g., `Mul`, `Relu`), register them in `main_functions.cc`:
```cpp
static tflite::MicroMutableOpResolver<4> resolver;
resolver.AddFullyConnected();
resolver.AddAdd();
resolver.AddMul();
resolver.AddRelu();
```
Increase the template parameter (`<4>`) to accommodate the number of ops.

---

## 6) Memory Tuning

- Adjust tensor arena size in `main_functions.cc`:
```cpp
#define TENSOR_ARENA_SIZE 2048 // increase if AllocateTensors fails
```
- On low‑memory targets, simplify the model or reduce input size.

---

## 7) Troubleshooting

- Missing headers in editor: Use the Embox build; TFLM include paths are provided via `third_party.lib.tflite_micro` modules.
- `AllocateTensors failed.`: Increase `TENSOR_ARENA_SIZE` or reduce model complexity.
- Undefined symbols: Ensure the C array symbol names in `models/Linear_regression_model_data.*` match the references in code.
- No output: Confirm `linear_regression` is included in the selected template’s `mods.conf` and the model file is present.

---

## 8) Extend to Your Custom Model

- Train any small regression/classification model (e.g., shallow MLP) in Python.
- Export as TFLite (float or int8). Prefer int8 for better embedded performance.
- Convert to C array with `xxd -i` and drop into `models/`.
- Register the ops you actually use. Keep the resolver minimal.
- Test first in QEMU, then migrate to your hardware template.

---

## References

- TensorFlow Lite Micro (tflite‑micro)
- Embox TensorFlow templates: `project/tensorflow/templates/`
- Example apps: `hello_world`, `micro_speech`, `people_detection`
