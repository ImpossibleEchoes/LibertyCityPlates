#pragma once
#include "addrs.h"
#include <vector>

extern int(__cdecl* g_pfnGetRandom)(int, int);

float smoothFloat(float current_value, float previous_smoothed_value, float delta_time, float smooth_time);
float smoothFloat2(float current_value, float previous_smoothed_value, float delta_time, float smooth_time);

__forceinline bool __cdecl isGameKeyboardKeyJustPressed(int a1) {
	return ((bool(__cdecl*)(int a1))(g_isGameKeyboardKeyJustPressed))(a1);
}
float normalizeAngle(float angle);

float smoothAngle(float target_angle, float current_angle, float delta_time, float smooth_time);

uint32_t jenkins_one_at_a_time_hash(const char* key);

uint32_t getOrParseHash(const char* str);

int getRandom(int min, int max);
int getRandomFast(int min, int max);

void initRandom();

template <typename _T> _T getRandomElementFromVector(std::vector<_T>& vec) {
	return vec[g_pfnGetRandom(0, vec.size() - 1)];
}
