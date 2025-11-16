#include "blur.h"

#include <vector>

// Super Fast Blur v1.1
// by Mario Klingemann <http://incubator.quasimondo.com>
// converted to C++ by Mehmet Akten, <http://www.memo.tv>
//
// Tip: Multiple invovations of this filter with a small
// radius will approximate a gaussian blur quite well.
//

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

static std::vector<unsigned char> r, g, b;
static std::vector<int> vMIN, vMAX;
static std::vector<unsigned char> dv;

// At quick test, resizing doesn't happen much.
// This should be faster than 6 * (new + delete) on every call.
static void resizeBlurBuffers(std::size_t w, std::size_t h, std::size_t wh, std::size_t div)
{
    if (wh != r.size()) {
        //puts("resize rgb");
        r.resize(wh, 0);
        g.resize(wh, 0);
        b.resize(wh, 0);
    }

    const std::size_t maxWH = max(w, h);
    if (maxWH != vMIN.size()) {
        //puts("resize vMIN");
        vMIN.resize(maxWH, 0);
        vMAX.resize(maxWH, 0);
    }

    const std::size_t div256 = div * 256;
    if (div256 != dv.size()) {
        //puts("resize dv");
        dv.resize(div256, 0);
    }
}

void superFastBlur(unsigned char *pix, int w, int h, int radius)
{
    if (radius<1) return;

    int wm=w-1;
    int hm=h-1;
    int wh=w*h;
    int div=radius+radius+1;
    int yi = 0;
    int yw = 0;

    resizeBlurBuffers(w, h, wh, div);

    for (int i=0;i<256*div;i++)
    {
        dv[i]=(i/div);
    }

    // precalc
    for (int x=0;x<w;x++)
    {
        vMIN[x]=min(x+radius+1,wm);
        vMAX[x]=max(x-radius,0);
    }

    for (int y=0;y<h;y++)
    {
        int rsum = 0;
        int gsum = 0;
        int bsum = 0;
        for(int i=-radius;i<=radius;i++)
        {
            int p = (yi + min(wm, max(i,0))) * 3;
            rsum += pix[p];
            gsum += pix[p+1];
            bsum += pix[p+2];
        }

        for (int x=0;x<w;x++)
        {
            r[yi]=dv[rsum];
            g[yi]=dv[gsum];
            b[yi]=dv[bsum];

            int p1 = (yw+vMIN[x])*3;
            int p2 = (yw+vMAX[x])*3;

            rsum += pix[p1] - pix[p2];
            gsum += pix[p1+1] - pix[p2+1];
            bsum += pix[p1+2] - pix[p2+2];

            yi++;
        }
        yw+=w;
    }

    // precalc
    for(int y=0;y<h;y++)
    {
        vMIN[y]=min(y+radius+1,hm)*w;
        vMAX[y]=max(y-radius,0)*w;
    }

    for (int x=0;x<w;x++)
    {
        int rsum = 0;
        int gsum = 0;
        int bsum = 0;
        int yp=-radius*w;
        for(int i=-radius;i<=radius;i++)
        {
            yi=max(0,yp)+x;
            rsum+=r[yi];
            gsum+=g[yi];
            bsum+=b[yi];
            yp+=w;
        }
        yi=x;
        for (int y=0;y<h;y++)
        {
            pix[yi*3] = dv[rsum];
            pix[yi*3 + 1] = dv[gsum];
            pix[yi*3 + 2] = dv[bsum];

            int p1=x+vMIN[y];
            int p2=x+vMAX[y];

            rsum+=r[p1]-r[p2];
            gsum+=g[p1]-g[p2];
            bsum+=b[p1]-b[p2];

            yi+=w;
        }
    }
}
