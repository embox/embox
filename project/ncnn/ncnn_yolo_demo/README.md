# YOLOv8 Inference App (NCNN) for Embox

!!! Currently available only for qemu-arm

## Build and Run

```bash
make confclean
make confload-project/ncnn/ncnn_arm_qemu
make
sudo ./scripts/qemu/auto_qemu
```


Where 320 is the input size (letterbox).

```tish
ncnn_yolo_demo ./data/photos/dog.png
```