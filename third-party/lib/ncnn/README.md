# NCNN integration for Embox

This directory contains the Embox integration layer for upstream NCNN.

## Current design
- Upstream NCNN is built via CMake
- Embox links against the produced `libncnn.a`
- Public headers are taken from `install/include/ncnn`

## Public contract
- Consumers must include headers only from `install/include/ncnn`
- Consumers must not depend on `install/src`

## Required Embox compatibility shims
- `ncnn_new_delete.cpp` — provides placement operator new/delete
- `ncnn_math_trig_shim.cpp` — provides missing math symbols required by the current Embox runtime

## Notes
- Older manual integration files and generated header artifacts are intentionally removed
- Model-specific code and assets should live under `project/ncnn/...`, not here
