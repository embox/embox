#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
MOD_DIR="$ROOT_DIR/third-party/lib/ncnn"
WORK_DIR="$MOD_DIR/.regen-work"
SRC_DIR="$WORK_DIR/src"
BUILD_DIR="$WORK_DIR/build"
OUT_DIR="$MOD_DIR/generated"

PKG_NAME="ncnn"
PKG_VER="20250916"
ARCHIVE_URL="https://github.com/Tencent/${PKG_NAME}/archive/refs/tags/${PKG_VER}.tar.gz"

rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR" "$OUT_DIR"

echo "[1/5] download $ARCHIVE_URL"
curl -L "$ARCHIVE_URL" -o "$WORK_DIR/${PKG_NAME}.tar.gz"

echo "[2/5] extract"
tar -xf "$WORK_DIR/${PKG_NAME}.tar.gz" -C "$WORK_DIR"
mv "$WORK_DIR/${PKG_NAME}-${PKG_VER}" "$SRC_DIR"

echo "[3/5] configure cmake"
cmake -S "$SRC_DIR" -B "$BUILD_DIR" \
  -DNCNN_BUILD_TOOLS=OFF \
  -DNCNN_BUILD_EXAMPLES=OFF \
  -DNCNN_BUILD_BENCHMARK=OFF \
  -DNCNN_BUILD_TESTS=OFF \
  -DNCNN_SHARED_LIB=OFF \
  -DNCNN_VULKAN=OFF \
  -DNCNN_OPENMP=OFF \
  -DNCNN_THREADS=OFF \
  -DNCNN_RUNTIME_CPU=OFF \
  -DNCNN_C_API=OFF \
  -DNCNN_STDIO=OFF \
  -DNCNN_STRING=OFF

echo "[4/5] build generated headers"
cmake --build "$BUILD_DIR" -j"$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 4)" >/dev/null

echo "[5/5] copy generated files"
cp "$BUILD_DIR/src/layer_type_enum.h" "$OUT_DIR/layer_type_enum.h"
cp "$BUILD_DIR/src/layer_declaration.h" "$OUT_DIR/layer_declaration.h"
cp "$BUILD_DIR/src/layer_registry.h" "$OUT_DIR/layer_registry.h"
cp "$BUILD_DIR/src/layer_shader_registry.h" "$OUT_DIR/layer_shader_registry.h"
cp "$BUILD_DIR/src/layer_shader_spv_data.h" "$OUT_DIR/layer_shader_spv_data.h"
cp "$BUILD_DIR/src/layer_shader_type_enum.h" "$OUT_DIR/layer_shader_type_enum.h"
cp "$BUILD_DIR/src/ncnn_export.h" "$OUT_DIR/ncnn_export.h"

echo "Done. Updated files in $OUT_DIR"
