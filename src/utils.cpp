#include "utils.h"
#include <random>

// 全局引擎，程序生命周期内只构造一次
static std::mt19937 g_gen(std::random_device{}());

float random(float min, float max)
{
    std::uniform_real_distribution<float> dist(min, max);
    return dist(g_gen);
}

int random(int min, int max)
{
    std::uniform_int_distribution<int> dist(min, max);
    return dist(g_gen);
}

