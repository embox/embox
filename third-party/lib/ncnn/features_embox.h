// third_party/lib/ncnn/features_embox.h
#pragma once

// Конфиг ncnn для bare-metal / Embox

// Никаких системных API / dlopen / pthread и т.п.
#ifndef NCNN_PLATFORM_API
#define NCNN_PLATFORM_API 0
#endif

// Не используем runtime-detect CPU фич
#ifndef NCNN_RUNTIME_CPU
#define NCNN_RUNTIME_CPU 0
#endif

// stdio отключен
// #ifndef NCNN_STDIO
// #define NCNN_STDIO 0
// #endif

// Потоков тоже нет
// #ifndef NCNN_THREADS
// #define NCNN_THREADS 0
// #endif

// Бенчмарки не нужны
#ifndef NCNN_BENCHMARK
#define NCNN_BENCHMARK 0
#endif

// Используем внутреннюю лёгкую STL
// #ifndef NCNN_SIMPLESTL
// #define NCNN_SIMPLESTL 1
// #endif

// #ifndef NCNN_STRING
// #define NCNN_STRING 0
// #endif

// Все x86-векторизации выключаем
#ifndef NCNN_AVX
#define NCNN_AVX 0
#endif

#ifndef NCNN_AVX2
#define NCNN_AVX2 0
#endif

#ifndef NCNN_AVX512
#define NCNN_AVX512 0
#endif

// Пиксельные утилиты (simpleocv и пр.) нам не нужны
#ifndef NCNN_PIXEL
#define NCNN_PIXEL 0
#endif

#ifndef NCNN_PIXEL_ROTATE
#define NCNN_PIXEL_ROTATE 0
#endif

#ifndef NCNN_PIXEL_AFFINE
#define NCNN_PIXEL_AFFINE 0
#endif

#ifndef NCNN_PIXEL_DRAWING
#define NCNN_PIXEL_DRAWING 0
#endif

#ifndef NCNN_VULKAN
#define NCNN_VULKAN 0
#endif
