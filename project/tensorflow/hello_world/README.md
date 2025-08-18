# Hello World – Sine Wave Prediction  

This example demonstrates a **basic sine wave prediction model** running on **TensorFlow Lite Micro (TFLM)** within **Embox RTOS**, targeting embedded devices.  

It showcases:  
- Training a simple neural network to approximate sine values.  
- Converting the trained model to TensorFlow Lite Micro format.  
- Running inference on a microcontroller (or via QEMU simulation).  

The program prints predicted sine values for inputs up to **6π**, serving as a minimal end-to-end workflow for deploying ML models on embedded systems.  

### Below is the architecture of the model used : 


![arch_nn_sine_wave](project/tensorflow/hello_world/assets/sine_wave_nn.png )

> 💡 Use this example as a **reference project** for building and testing your own custom ML models with TensorFlow Lite Micro in the EmboxRTOS environment.  

---

## Steps to Run the Hello World Example  

1. Load the TFLite configuration for your target architecture (here, QEMU ARM):  
   ```bash
   make confload-project/tensorflow/arm-qemu

2. Build the project :
    ```bash 
    make

3. Run in QEMU after the build completes: 
    ```bash 
    ./scripts/qemu/auto_qemu

4. In qemu terminal, launch the application: 
    ```bash 
    hello_world


## Example output :

![output](project/tensorflow/hello_world/assets/output.png)


## Train the example on your own :

You can use the the official [tflite-micro hello world](https://colab.research.google.com/github/Mjrovai/UNIFEI-IESTI01-TinyML-2022.1/blob/main/00_Curse_Folder/2_Applications_Deploy/Class_16/TFLite-Micro-Hello-World/train_TFL_Micro_hello_world_model.ipynb) script to train your own model with custom changes (like for cosine or other math function) and follow similar pipeline to deploy the model using emboxRTOS.
