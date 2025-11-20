// third-party/lib/ncnn/ncnn_math_trig_shim.cpp
//
// Минимальные заглушки для тригонометрии и некоторых математических функций,
// чтобы ncnn на Embox успешно линкуется без полноценного libm.
//
// Важно: здесь мы реализуем ТОЛЬКО double-версии (sin/cos/...),
// а float-версии (sinf/cosf/...) уже определены в platform.h
// как inline-обёртки через наши double-функции.
//
// Эти реализации достаточно грубые и могут быть неточными,
// но для YOLOv8, которая не использует sin/cos в своём графе,
// они критичны только с точки зрения "чтобы всё слинковалось".

extern "C" {

// --- Базовые тригонометрические функции (double) ---

double sin(double x) {
    (void)x;
    return 0.0;   // заглушка
}

double cos(double x) {
    (void)x;
    return 1.0;   // заглушка
}

double tan(double x) {
    (void)x;
    return 0.0;   // заглушка
}

double asin(double x) {
    (void)x;
    return 0.0;   // заглушка
}

double acos(double x) {
    (void)x;
    return 0.0;   // заглушка
}

double atan(double x) {
    (void)x;
    return 0.0;   // заглушка
}

double atan2(double y, double x) {
    (void)y;
    (void)x;
    return 0.0;   // заглушка
}

// --- Гиперболические функции (double) ---

double sinh(double x) {
    (void)x;
    return 0.0;   // заглушка
}

double cosh(double x) {
    (void)x;
    return 1.0;   // заглушка
}

double atanh(double x) {
    (void)x;
    return 0.0;   // заглушка
}

// --- atanhf (float) ---

float atanhf(float x) {
    (void)x;
    return 0.0f;  // заглушка
}

// --- trunc (double) ---

double trunc(double x) {
    // Отбрасываем дробную часть (toward zero).
    // Приведение к long в Си/C++ ведёт себя как trunc по направлению к нулю.
    long i = (long)x;
    return (double)i;
}

} // extern "C"
