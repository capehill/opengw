#define _USE_MATH_DEFINES
#include "sincos.hpp"
#include "mathutils.hpp"
#include <cmath>

#define NUM_ENTRIES (2046)

static float sin_table[NUM_ENTRIES];
static float cos_table[NUM_ENTRIES];

static float inc = RADIAN / NUM_ENTRIES;

constexpr float entriesPerRadian = NUM_ENTRIES / RADIAN;

void make_sin_cos_tables()
{
    // calculate sine lookup table
    float radian = 0;
    for (int i = 0; i < NUM_ENTRIES; i++) {
        sin_table[i] = sin(radian);
        radian += inc;
    }

    // calculate cosine lookup table
    radian = 0;
    for (int i = 0; i < NUM_ENTRIES; i++) {
        cos_table[i] = cos(radian);
        radian += inc;
    }
}

float get_sin(float i)
{
    i = mathutils::wrapRadians(i);
    // i = i/RADIAN;
    return sin_table[(int)(i * entriesPerRadian)];
}

float get_cos(float i)
{
    i = mathutils::wrapRadians(i);
    // i = i/RADIAN;
    return cos_table[(int)(i * entriesPerRadian)];
}
