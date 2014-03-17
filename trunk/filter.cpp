#include "filter.h"

namespace filter
{
    #define NZEROS 1
    #define NPOLES 1

    void lowpass(const float* input, float* output, float cutoff, int len)
    {
        if (cutoff > .999)
        {
            for (int i=0; i<len; i++)
            {
                output[i] = input[i];
            }
            return;
        }

        static float xv[NZEROS+1], yv[NPOLES+1];

        cutoff *= .5; // stupid game-specific hack

        float cutoffVal = 1-cutoff;
        float gain = cutoff;

        for (int i=0; i<len; i++)
        {
            xv[0] = xv[1]; 
            xv[1] = input[i] * gain;
            yv[0] = yv[1]; 
            yv[1] = (xv[0] + xv[1]) + (cutoffVal * yv[0]);
            output[i] = yv[1];
        }
    }

    #define BPNZEROS 2
    #define BPNPOLES 2

    void bandpass(const float* input, float* output, float cutoff, int len)
    {
        if (cutoff > .999)
        {
            for (int i=0; i<len; i++)
            {
                output[i] = input[i];
            }
            return;
        }

        static float xv[BPNZEROS+1], yv[BPNPOLES+1];

        cutoff *= .3; // stupid game-specific hack

        float cutoffVal = (((1-cutoff) * 2) - 1) * 1.8647010906;

        float gain = 0.4e+01;

        for (int i=0; i<len; i++)
        {
            xv[0] = xv[1]; xv[1] = xv[2]; 
            xv[2] = input[i] / gain;
            yv[0] = yv[1]; yv[1] = yv[2]; 
            yv[2] = (xv[2] - xv[0]) + (-0.8667884395 * yv[0]) + (cutoffVal * yv[1]);
            output[i] = yv[2];
        }
    }

    void mix(const float* s1, const float* s2, float* output, int len)
    {
        for (int i=0; i<len; i++)
        {
            output[i] = (s1[i] + s2[i])/2;
        }
    }



};
