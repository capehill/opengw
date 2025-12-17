#include "blur.hpp"
#include "profiler.hpp"

#include <vector>

// Super Fast Blur v1.1
// by Mario Klingemann <http://incubator.quasimondo.com>
// converted to C++ by Mehmet Akten, <http://www.memo.tv>
//
// Tip: Multiple invovations of this filter with a small
// radius will approximate a gaussian blur quite well.
//

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

static std::vector<ColorRGB> rgb;
static std::vector<int> vMIN, vMAX;
static std::vector<unsigned char> dv;

// At quick test, resizing doesn't happen much.
// This should be faster than 6 * (new + delete) on every call.
static void resizeBlurBuffers(std::size_t w, std::size_t h, std::size_t wh, std::size_t div)
{
    if (wh != rgb.size()) {
        // puts("resize rgb");
        rgb.resize(wh);
    }

    const std::size_t maxWH = max(w, h);
    if (maxWH != vMIN.size()) {
        // puts("resize vMIN");
        vMIN.resize(maxWH, 0);
        vMAX.resize(maxWH, 0);
    }

    const std::size_t div256 = div * 256;
    if (div256 != dv.size()) {
        // puts("resize dv");
        dv.resize(div256, 0);

        for (std::size_t i = 0; i < div256; i++) {
            dv[i] = (i / div);
        }
    }
}

static profiler prof("blur");

void superFastBlur(ColorRGB* pix, int w, int h, int radius)
{
    if (radius < 1)
        return;

    prof.start();

    const int wm = w - 1;
    const int hm = h - 1;
    const int wh = w * h;
    const int div = radius + radius + 1;
    int yi = 0;
    int yw = 0;

    resizeBlurBuffers(w, h, wh, div);

    // precalc
    for (int x = 0; x < w; x++) {
        vMIN[x] = min(x + radius + 1, wm);
        vMAX[x] = max(x - radius, 0);
    }

    for (int y = 0; y < h; y++) {
        int rsum = 0;
        int gsum = 0;
        int bsum = 0;
        for (int i = -radius; i <= radius; i++) {
            const int p = (yi + min(wm, max(i, 0)));
            const ColorRGB& pixel = pix[p];
            rsum += pixel.r;
            gsum += pixel.g;
            bsum += pixel.b;
        }

        for (int x = 0; x < w; x++) {
            ColorRGB& color = rgb[yi];
            color.r = dv[rsum];
            color.g = dv[gsum];
            color.b = dv[bsum];

            const int p1 = (yw + vMIN[x]);
            const int p2 = (yw + vMAX[x]);

            const ColorRGB& pixel1 = pix[p1];
            const ColorRGB& pixel2 = pix[p2];

            rsum += pixel1.r - pixel2.r;
            gsum += pixel1.g - pixel2.g;
            bsum += pixel1.b - pixel2.b;

            yi++;
        }
        yw += w;
    }

    // precalc
    for (int y = 0; y < h; y++) {
        vMIN[y] = min(y + radius + 1, hm) * w;
        vMAX[y] = max(y - radius, 0) * w;
    }

    for (int x = 0; x < w; x++) {
        int rsum = 0;
        int gsum = 0;
        int bsum = 0;
        int yp = -radius * w;
        for (int i = -radius; i <= radius; i++) {
            yi = max(0, yp) + x;
            const ColorRGB& color = rgb[yi];
            rsum += color.r;
            gsum += color.g;
            bsum += color.b;
            yp += w;
        }
        yi = x;
        for (int y = 0; y < h; y++) {
            ColorRGB& pixel = pix[yi];
            pixel.r = dv[rsum];
            pixel.g = dv[gsum];
            pixel.b = dv[bsum];

            const int p1 = x + vMIN[y];
            const int p2 = x + vMAX[y];

            const ColorRGB& rgb1 = rgb[p1];
            const ColorRGB& rgb2 = rgb[p2];

            rsum += rgb1.r - rgb2.r;
            gsum += rgb1.g - rgb2.g;
            bsum += rgb1.b - rgb2.b;

            yi += w;
        }
    }

    prof.stop();
}
