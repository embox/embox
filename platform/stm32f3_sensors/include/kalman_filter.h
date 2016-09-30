/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    28.05.2015
 */

struct kalman_filter {
	float value;
	float bias;

	float Q[2]; /* correspond to diagonale matrix of dim 2 */
	float R;
	float P[2][2];
};

extern void kalman_filter_init(struct kalman_filter *filter, float value);
extern float kalman_filter_iterate(struct kalman_filter *filter, float value,
	float rate, float dt);
