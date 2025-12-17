#pragma once

struct ColorRGB
{
    unsigned char r {};
    unsigned char g {};
    unsigned char b {};
};

void superFastBlur(ColorRGB* pix, int w, int h, int radius);
