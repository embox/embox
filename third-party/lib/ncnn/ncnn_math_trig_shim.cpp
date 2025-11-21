extern "C" {

double sin(double x) {
    (void)x;
    return 0.0;
}

double cos(double x) {
    (void)x;
    return 1.0;
}

double tan(double x) {
    (void)x;
    return 0.0;
}

double asin(double x) {
    (void)x;
    return 0.0;
}

double acos(double x) {
    (void)x;
    return 0.0;
}

double atan(double x) {
    (void)x;
    return 0.0;
}

double atan2(double y, double x) {
    (void)y;
    (void)x;
    return 0.0;
}


double sinh(double x) {
    (void)x;
    return 0.0;
}

double cosh(double x) {
    (void)x;
    return 1.0;
}

double atanh(double x) {
    (void)x;
    return 0.0;
}

// --- atanhf (float) ---

float atanhf(float x) {
    (void)x;
    return 0.0f;
}

// --- trunc (double) ---

double trunc(double x) {
    long i = (long)x;
    return (double)i;
}

} // extern "C"
