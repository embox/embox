# YOLOv8 Inference App (NCNN) for Embox

!!! Currently available only for qemu-arm

## Converting an Image to PPM (RGB, 8-bit)

!!! JPEG/PNG input is not supported

### Option 1 — ffmpeg (recommended)

```bash
ffmpeg -y -i photos/dog.png -f image2 -vcodec ppm -pix_fmt rgb24 photos/dog.ppm
```

Check the “magic bytes” and header:

```bash
xxd -l 2 photos/dog.ppm   # should output: P6
head -n 3 photos/dog.ppm  # lines: P6 / <W> <H> / 255
```

### Option 2 — ImageMagick

```bash
magick photos/dog.png -alpha off -depth 8 -define ppm:format=raw -compress none ppm:photos/dog.ppm
```

Make sure the result is P6, not text-based P3 — otherwise the parser will fail or the colors will be incorrect.

## Build and Run

```bash
make confclean
make confload-project/ncnn/ncnn_arm_qemu
make
sudo ./scripts/qemu/auto_qemu
```


Where 320 is the input size (letterbox). You may also try 640.

```tish
ncnn_yolo_demo ./data/photos/dog.ppm
```