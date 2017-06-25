/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    28.05.2015
 */

#include <kalman_filter.h>

void kalman_filter_init(struct kalman_filter *filter, float value) {
    filter->value = value;
    filter->bias = 0.0f;

    filter->R = 0.03f;

    filter->Q[0] = 0.001f;
    filter->Q[1] = 0.003f;

    filter->P[0][0] = 0.0f;
    filter->P[0][1] = 0.0f;
    filter->P[1][0] = 0.0f;
    filter->P[1][1] = 0.0f;
};

float kalman_filter_iterate(struct kalman_filter *filter, float value,
	float rate, float dt) {
	float S, y, K[2];

    /* PHASE: Predict */
    /* a priori: State estimate */
    filter->value += dt * (rate - filter->bias);
    /* a priori: Estimate covariance */
    filter->P[0][0] += dt * (dt*filter->P[1][1] -filter->P[0][1] -
    							filter->P[1][0] + filter->Q[0]);
    filter->P[0][1] -= dt * filter->P[1][1];
    filter->P[1][0] -= dt * filter->P[1][1];
    filter->P[1][1] += filter->Q[1] * dt;

    /* PHASE: Update */
    /* Measurement residual */
    y = value - filter->value;
    /* Residual covariance */
    S = filter->P[0][0] + filter->R;
    /* Optimal Kalman gain */
    K[0] = filter->P[0][0] / S;
    K[1] = filter->P[1][0] / S;
    /* a posteriori: State estimate */
    filter->value += K[0] * y;
    filter->bias += K[1] * y;
    /* a posteriori: Estimate covariance */
    filter->P[0][0] -= K[0] * filter->P[0][0];
    filter->P[0][1] -= K[0] * filter->P[0][1];
    filter->P[1][0] -= K[1] * filter->P[0][0];
    filter->P[1][1] -= K[1] * filter->P[0][1];

    return value;
};
