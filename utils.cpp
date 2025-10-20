#include <math.h>
#include "rage_math.h"

float smoothFloat2(float current_value, float previous_smoothed_value, float delta_time, float smooth_time) {
	if (current_value == previous_smoothed_value)
		return current_value;

	if (current_value > previous_smoothed_value) {
		if (current_value > previous_smoothed_value + (smooth_time * delta_time))
			current_value = previous_smoothed_value + (smooth_time * delta_time);
	}
	else if (current_value < previous_smoothed_value) {
		if (current_value < previous_smoothed_value - (smooth_time * delta_time))
			current_value = previous_smoothed_value - (smooth_time * delta_time);
	}
	return current_value;
}

float smoothFloat(float current_value, float previous_smoothed_value, float delta_time, float smooth_time) {
	if (smooth_time <= 0.0001f || delta_time <= 0.0f)
		return current_value;

	float alpha = 1.0f - expf(-delta_time / smooth_time);
	return alpha * current_value + (1.0f - alpha) * previous_smoothed_value;
}

float normalizeAngle(float angle) {
	angle = fmodf(angle, 2.0f * RAGE_PI);
	if (angle > RAGE_PI)
		angle -= 2.0f * RAGE_PI;
	else if (angle < -RAGE_PI)
		angle += 2.0f * RAGE_PI;
	return angle;
}

float smoothAngle(float target_angle, float current_angle, float delta_time, float smooth_time) {
	if (current_angle == target_angle)
		return current_angle;
	else if (smooth_time <= 0.0001f || delta_time <= 0.0f)
		return target_angle;

	float angle_diff = normalizeAngle(target_angle - current_angle);

	float alpha = 1.0f - expf(-delta_time / smooth_time);
	float smoothed_diff = alpha * angle_diff;

	//return current_angle + smoothed_diff;
	return normalizeAngle(current_angle + smoothed_diff);
}

uint32_t jenkins_one_at_a_time_hash(const char* key) {
	size_t i = 0;
	uint32_t hash = 0;
	int length = strlen(key);
	while (i != length) {
		char k = key[i++];
		if (k >= 'A' && k <= 'Z')
			k += 0x20;

		hash += k;
		hash += hash << 10;
		hash ^= hash >> 6;
	}
	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;
	return hash;
}