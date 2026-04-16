// Copyright 2017 Tencent
// SPDX-License-Identifier: BSD-3-Clause
#pragma once

#ifndef NCNN_PLATFORM_H
#define NCNN_PLATFORM_H

#define NCNN_STDIO 0
#define NCNN_STRING 0
#define NCNN_SIMPLEOCV 0
#define NCNN_SIMPLEOMP 0
#define NCNN_SIMPLESTL 1
#define NCNN_SIMPLEMATH 0
#define NCNN_THREADS 0
#define NCNN_BENCHMARK 0
#define NCNN_C_API 0
#define NCNN_PLATFORM_API 1
#define NCNN_PIXEL 1
#define NCNN_PIXEL_ROTATE 1
#define NCNN_PIXEL_AFFINE 1
#define NCNN_PIXEL_DRAWING 1
#define NCNN_VULKAN 0
#define NCNN_SIMPLEVK 1
#define NCNN_SYSTEM_GLSLANG 0
#define NCNN_RUNTIME_CPU 1
#define NCNN_GNU_INLINE_ASM 1
#define NCNN_AVX 0
#define NCNN_XOP 0
#define NCNN_FMA 0
#define NCNN_F16C 0
#define NCNN_AVX2 0
#define NCNN_AVXVNNI 0
#define NCNN_AVXVNNIINT8 0
#define NCNN_AVXVNNIINT16 0
#define NCNN_AVXNECONVERT 0
#define NCNN_AVX512 0
#define NCNN_AVX512VNNI 0
#define NCNN_AVX512BF16 0
#define NCNN_AVX512FP16 0
#define NCNN_VFPV4 0
#define NCNN_ARM82 0
#define NCNN_ARM82DOT 0
#define NCNN_ARM82FP16FML 0
#define NCNN_ARM84BF16 0
#define NCNN_ARM84I8MM 0
#define NCNN_ARM86SVE 0
#define NCNN_ARM86SVE2 0
#define NCNN_ARM86SVEBF16 0
#define NCNN_ARM86SVEI8MM 0
#define NCNN_ARM86SVEF32MM 0
#define NCNN_MSA 0
#define NCNN_LSX 0
#define NCNN_MMI 0
#define NCNN_RVV 0
#define NCNN_ZFH 0
#define NCNN_ZVFH 0
#define NCNN_XTHEADVECTOR 0
#define NCNN_INT8 1
#define NCNN_BF16 1
#define NCNN_FORCE_INLINE 1

#define NCNN_VERSION_STRING "1.0.20251104"

#include "ncnn_export.h"

#ifdef __cplusplus

#if NCNN_THREADS
#if defined _WIN32
#define WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
#endif // NCNN_THREADS

#if __ANDROID_API__ >= 26
#ifndef VK_USE_PLATFORM_ANDROID_KHR
#define VK_USE_PLATFORM_ANDROID_KHR
#endif
#endif // __ANDROID_API__ >= 26

namespace ncnn {

#if NCNN_THREADS
#if defined _WIN32
#if _WIN32_WINNT > _WIN32_WINNT_WINXP // Windows Vista and later
class NCNN_EXPORT Mutex
{
public:
    Mutex() { InitializeSRWLock(&srwlock); }
    ~Mutex() {}
    void lock() { AcquireSRWLockExclusive(&srwlock); }
    void unlock() { ReleaseSRWLockExclusive(&srwlock); }
private:
    friend class ConditionVariable;
    SRWLOCK srwlock;
};

class NCNN_EXPORT ConditionVariable
{
public:
    ConditionVariable() { InitializeConditionVariable(&condvar); }
    ~ConditionVariable() {}
    void wait(Mutex& mutex) { SleepConditionVariableSRW(&condvar, &mutex.srwlock, INFINITE, 0); }
    void broadcast() { WakeAllConditionVariable(&condvar); }
    void signal() { WakeConditionVariable(&condvar); }
private:
    CONDITION_VARIABLE condvar;
};

#else // Windows XP compatibility

class NCNN_EXPORT Mutex
{
public:
    Mutex() { InitializeCriticalSection(&cs); }
    ~Mutex() { DeleteCriticalSection(&cs); }
    void lock() { EnterCriticalSection(&cs); }
    void unlock() { LeaveCriticalSection(&cs); }
private:
    friend class ConditionVariable;
    CRITICAL_SECTION cs;
};

class NCNN_EXPORT ConditionVariable
{
public:
    ConditionVariable() 
    { 
        signal_event = CreateEvent(0, FALSE, FALSE, 0); // Auto-reset event for signal()
        broadcast_event = CreateEvent(0, TRUE, FALSE, 0); // Manual-reset event for broadcast()
    }
    ~ConditionVariable() 
    { 
        CloseHandle(signal_event); 
        CloseHandle(broadcast_event); 
    }
    void wait(Mutex& mutex)
    {
        mutex.unlock();
        HANDLE events[2] = { signal_event, broadcast_event };
        WaitForMultipleObjects(2, events, FALSE, INFINITE); // Wait for either signal or broadcast
        mutex.lock();
    }
    void broadcast() 
    { 
        SetEvent(broadcast_event); // Wake all threads
        ResetEvent(broadcast_event); // Reset after waking all threads
    }
    void signal() 
    { 
        SetEvent(signal_event); // Wake one thread
    }
private:
    HANDLE signal_event;
    HANDLE broadcast_event;
};

#endif // _WIN32_WINNT > _WIN32_WINNT_WINXP

static unsigned __stdcall start_wrapper(void* args);
class NCNN_EXPORT Thread
{
public:
    Thread(void* (*start)(void*), void* args = 0) { _start = start; _args = args; handle = (HANDLE)_beginthreadex(0, 0, start_wrapper, this, 0, 0); }
    ~Thread() {}
    void join() { WaitForSingleObject(handle, INFINITE); CloseHandle(handle); }
private:
    friend unsigned __stdcall start_wrapper(void* args)
    {
        Thread* t = (Thread*)args;
        t->_start(t->_args);
        return 0;
    }
    HANDLE handle;
    void* (*_start)(void*);
    void* _args;
};

class NCNN_EXPORT ThreadLocalStorage
{
public:
    ThreadLocalStorage() { key = TlsAlloc(); }
    ~ThreadLocalStorage() { TlsFree(key); }
    void set(void* value) { TlsSetValue(key, (LPVOID)value); }
    void* get() { return (void*)TlsGetValue(key); }
private:
    DWORD key;
};
#else // defined _WIN32
class NCNN_EXPORT Mutex
{
public:
    Mutex() { pthread_mutex_init(&mutex, 0); }
    ~Mutex() { pthread_mutex_destroy(&mutex); }
    void lock() { pthread_mutex_lock(&mutex); }
    void unlock() { pthread_mutex_unlock(&mutex); }
private:
    friend class ConditionVariable;
    pthread_mutex_t mutex;
};

class NCNN_EXPORT ConditionVariable
{
public:
    ConditionVariable() { pthread_cond_init(&cond, 0); }
    ~ConditionVariable() { pthread_cond_destroy(&cond); }
    void wait(Mutex& mutex) { pthread_cond_wait(&cond, &mutex.mutex); }
    void broadcast() { pthread_cond_broadcast(&cond); }
    void signal() { pthread_cond_signal(&cond); }
private:
    pthread_cond_t cond;
};

class NCNN_EXPORT Thread
{
public:
    Thread(void* (*start)(void*), void* args = 0) { pthread_create(&t, 0, start, args); }
    ~Thread() {}
    void join() { pthread_join(t, 0); }
private:
    pthread_t t;
};

class NCNN_EXPORT ThreadLocalStorage
{
public:
    ThreadLocalStorage() { pthread_key_create(&key, 0); }
    ~ThreadLocalStorage() { pthread_key_delete(key); }
    void set(void* value) { pthread_setspecific(key, value); }
    void* get() { return pthread_getspecific(key); }
private:
    pthread_key_t key;
};
#endif // defined _WIN32
#else // NCNN_THREADS
class NCNN_EXPORT Mutex
{
public:
    Mutex() {}
    ~Mutex() {}
    void lock() {}
    void unlock() {}
};

class NCNN_EXPORT ConditionVariable
{
public:
    ConditionVariable() {}
    ~ConditionVariable() {}
    void wait(Mutex& /*mutex*/) {}
    void broadcast() {}
    void signal() {}
};

class NCNN_EXPORT Thread
{
public:
    Thread(void* (*/*start*/)(void*), void* /*args*/ = 0) {}
    ~Thread() {}
    void join() {}
};

class NCNN_EXPORT ThreadLocalStorage
{
public:
    ThreadLocalStorage() { data = 0; }
    ~ThreadLocalStorage() {}
    void set(void* value) { data = value; }
    void* get() { return data; }
private:
    void* data;
};
#endif // NCNN_THREADS

class NCNN_EXPORT MutexLockGuard
{
public:
    MutexLockGuard(Mutex& _mutex) : mutex(_mutex) { mutex.lock(); }
    ~MutexLockGuard() { mutex.unlock(); }
private:
    Mutex& mutex;
};

static inline void swap_endianness_16(void* x)
{
    unsigned char* xx = (unsigned char*)x;
    unsigned char x0 = xx[0];
    unsigned char x1 = xx[1];
    xx[0] = x1;
    xx[1] = x0;
}

static inline void swap_endianness_32(void* x)
{
    unsigned char* xx = (unsigned char*)x;
    unsigned char x0 = xx[0];
    unsigned char x1 = xx[1];
    unsigned char x2 = xx[2];
    unsigned char x3 = xx[3];
    xx[0] = x3;
    xx[1] = x2;
    xx[2] = x1;
    xx[3] = x0;
}

} // namespace ncnn

#if NCNN_SIMPLESTL
#include "simplestl.h"
#else
#include <algorithm>
#include <list>
#include <vector>
#include <stack>
#include <string>
#endif

// simplemath
#if NCNN_SIMPLEMATH
#  ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wbuiltin-declaration-mismatch"
#  endif
#  include "simplemath.h"
#  ifdef __GNUC__
#    pragma GCC diagnostic pop
#  endif
#else
#  include <math.h>
// #include <fenv.h>
#endif


#if NCNN_VULKAN
#if NCNN_SIMPLEVK
#include "simplevk.h"
#else
#include <vulkan/vulkan.h>
#endif
#include "vulkan_header_fix.h"
#endif // NCNN_VULKAN

#endif // __cplusplus

#if NCNN_STDIO
#if NCNN_PLATFORM_API && __ANDROID_API__ >= 8
#include <android/log.h>
#define NCNN_LOGE(...) do { \
    fprintf(stderr, ##__VA_ARGS__); fprintf(stderr, "\n"); \
    __android_log_print(ANDROID_LOG_WARN, "ncnn", ##__VA_ARGS__); } while(0)
#else // NCNN_PLATFORM_API && __ANDROID_API__ >= 8
#include <stdio.h>
#define NCNN_LOGE(...) do { \
    fprintf(stderr, ##__VA_ARGS__); fprintf(stderr, "\n"); } while(0)
#endif // NCNN_PLATFORM_API && __ANDROID_API__ >= 8
#else
#define NCNN_LOGE(...)
#endif


#if NCNN_FORCE_INLINE
#ifdef _MSC_VER
    #define NCNN_FORCEINLINE __forceinline
#elif defined(__GNUC__)
    #define NCNN_FORCEINLINE inline __attribute__((__always_inline__))
#elif defined(__CLANG__)
    #if __has_attribute(__always_inline__)
        #define NCNN_FORCEINLINE inline __attribute__((__always_inline__))
    #else
        #define NCNN_FORCEINLINE inline
    #endif
#else
    #define NCNN_FORCEINLINE inline
#endif
#else
    #define NCNN_FORCEINLINE inline
#endif

#endif // NCNN_PLATFORM_H

#if !NCNN_SIMPLEMATH 

#ifndef FE_TONEAREST
#define FE_TONEAREST 0
#endif

#ifndef FE_DOWNWARD
#define FE_DOWNWARD 0
#endif

#ifndef FE_UPWARD
#define FE_UPWARD 0
#endif

#ifndef FE_TOWARDZERO
#define FE_TOWARDZERO 0
#endif

static inline int feclearexcept(int) { return 0; }
static inline int fetestexcept(int)  { return 0; }
static inline int fesetround(int)    { return 0; }
static inline int fegetround(void)   { return FE_TONEAREST; }


#ifdef fmaxf
#undef fmaxf
#endif

static inline float fmaxf(float x, float y) {
    return x > y ? x : y;
}

/* --- наши фоллбеки для отсутствующих float-функций newlib --- */

/* если нет sinf/cosf/tanf/... */

#ifdef sinf
#undef sinf
#endif
static inline float sinf(float x) { return (float)sin((double)x); }

#ifdef cosf
#undef cosf
#endif
static inline float cosf(float x) { return (float)cos((double)x); }

#ifdef tanf
#undef tanf
#endif
static inline float tanf(float x) { return (float)tan((double)x); }

#ifdef asinf
#undef asinf
#endif
static inline float asinf(float x) { return (float)asin((double)x); }

#ifdef acosf
#undef acosf
#endif
static inline float acosf(float x) { return (float)acos((double)x); }

#ifdef atanf
#undef atanf
#endif
static inline float atanf(float x) { return (float)atan((double)x); }

#ifdef atan2f
#undef atan2f
#endif
static inline float atan2f(float y, float x) {
    return (float)atan2((double)y, (double)x);
}

#ifdef sinhf
#undef sinhf
#endif
static inline float sinhf(float x) { return (float)sinh((double)x); }

#ifdef coshf
#undef coshf
#endif
static inline float coshf(float x) { return (float)cosh((double)x); }

// tanhf мы уже переопределяли, но сделаем по тому же паттерну
#ifdef tanhf
#undef tanhf
#endif
static inline float tanhf(float x) {
    float e2x = expf(2.0f * x);
    return (e2x - 1.0f) / (e2x + 1.0f);
}

#ifdef truncf
#undef truncf
#endif
static inline float truncf(float x) { return (float)trunc((double)x); }

static inline float acoshf(float x) {
    double xd = (double)x;
    return (float)log(xd + sqrt((xd - 1.0) * (xd + 1.0)));
}

static inline float asinhf(float x) {
    double xd = (double)x;
    return (float)log(xd + sqrt(xd * xd + 1.0));
}

/* Приближённая erf(x) — формула Абрамовица–Стегуна 7.1.26 */
static inline float erff(float x) {
    double xd = (double)x;
    double t = 1.0 / (1.0 + 0.5 * fabs(xd));
    double tau = t * exp(
        -xd * xd - 1.26551223 +
        t * (1.00002368 +
        t * (0.37409196 +
        t * (0.09678418 +
        t * (-0.18628806 +
        t * (0.27886807 +
        t * (-1.13520398 +
        t * (1.48851587 +
        t * (-0.82215223 +
        t * 0.17087277))))))))
    );
    double res = 1.0 - tau;
    return (float)(xd >= 0.0 ? res : -res);
}

#ifdef erfcf
#undef erfcf
#endif
static inline float erfcf(float x) {
    // erfc(x) = 1 - erf(x)
    return 1.0f - erff(x);
}

#ifdef nearbyintf
#undef nearbyintf
#endif
static inline float nearbyintf(float x) {
    double xd = (double)x;
    if (xd >= 0.0)
        return (float)floor(xd + 0.5);
    else
        return (float)ceil(xd - 0.5);
}

static inline float log10f(float x) {
    return (float)log10((double)x);
}

static inline float fmodf(float x, float y) {
    return (float)fmod((double)x, (double)y);
}
#endif