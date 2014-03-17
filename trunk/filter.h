#pragma once

namespace filter
{
    void lowpass(const float* input, float* output, float cutoff, int len);
    void bandpass(const float* input, float* output, float cutoff, int len);
    void mix(const float* s1, const float* s2, float* output, int len);
};


