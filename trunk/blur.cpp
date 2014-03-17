#include "blur.h"

// Super Fast Blur v1.1
// by Mario Klingemann <http://incubator.quasimondo.com>
// converted to C++ by Mehmet Akten, <http://www.memo.tv>
//
// Tip: Multiple invovations of this filter with a small
// radius will approximate a gaussian blur quite well.
//


#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

void superFastBlur(unsigned char *pix, int w, int h, int radius)
{
    if (radius<1) return;

    int wm=w-1;
    int hm=h-1;
    int wh=w*h;
    int div=radius+radius+1;
    unsigned char *r=new unsigned char[wh];
    unsigned char *g=new unsigned char[wh];
    unsigned char *b=new unsigned char[wh];
    int rsum,gsum,bsum,x,y,i,p,p1,p2,yp,yi,yw;
    int *vMIN = new int[max(w,h)];
    int *vMAX = new int[max(w,h)];

    unsigned char *dv=new unsigned char[256*div];
    for (i=0;i<256*div;i++) dv[i]=(i/div);

    yw=yi=0;

    for (y=0;y<h;y++)
    {
        rsum=gsum=bsum=0;
        for(i=-radius;i<=radius;i++)
        {
            p = (yi + min(wm, max(i,0))) * 3;
            rsum += pix[p];
            gsum += pix[p+1];
            bsum += pix[p+2];
        }
        for (x=0;x<w;x++)
        {

            r[yi]=dv[rsum];
            g[yi]=dv[gsum];
            b[yi]=dv[bsum];

            if(y==0)
            {
                vMIN[x]=min(x+radius+1,wm);
                vMAX[x]=max(x-radius,0);
            }
            p1 = (yw+vMIN[x])*3;
            p2 = (yw+vMAX[x])*3;

            rsum += pix[p1] - pix[p2];
            gsum += pix[p1+1] - pix[p2+1];
            bsum += pix[p1+2] - pix[p2+2];

            yi++;
        }
        yw+=w;
    }

    for (x=0;x<w;x++)
    {
        rsum=gsum=bsum=0;
        yp=-radius*w;
        for(i=-radius;i<=radius;i++)
        {
            yi=max(0,yp)+x;
            rsum+=r[yi];
            gsum+=g[yi];
            bsum+=b[yi];
            yp+=w;
        }
        yi=x;
        for (y=0;y<h;y++)
        {
            pix[yi*3] = dv[rsum];
            pix[yi*3 + 1] = dv[gsum];
            pix[yi*3 + 2] = dv[bsum];
            if(x==0)
            {
                vMIN[y]=min(y+radius+1,hm)*w;
                vMAX[y]=max(y-radius,0)*w;
            }
            p1=x+vMIN[y];
            p2=x+vMAX[y];

            rsum+=r[p1]-r[p2];
            gsum+=g[p1]-g[p2];
            bsum+=b[p1]-b[p2];

            yi+=w;
        }
    }

    delete r;
    delete g;
    delete b;

    delete vMIN;
    delete vMAX;
    delete dv;
}
