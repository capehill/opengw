#include "vector.h"
#include "mathutils.h"


static const float thicknessConstant = .0165;

namespace vector
{
    // Not currently used but this will draw a line as a texture-mapped triangle strip.
    // The idea was to texture map the triangle strip with a glow texture so I'd
    // have nice glowy vectors without having to use the current buffer blur function
    // but I never got it looking as good as the buffer blur approach.
    void drawVector(const Point3d& from, const Point3d& to, const pen& penStyle)
    {
/*
        float lineWidth = penStyle.lineRadius * thicknessConstant;

        float angle = mathutils::calculate2dAngle(from, to);
        float length = mathutils::calculate2dDistance(from, to);

        Point3d mid((from.x + to.x) / 2, (from.y + to.y) / 2);

        //////////////////////////////////////////////
        // Create the prime points

        Point3d primeP1(-length/2, 0);
        Point3d primeP2(length/2, 0);

        //////////////////////////////////////////////
        // Create the initial triangle strip points

        Point3d p1, p2, p3, p4, p5, p6, p7, p8;

        p1.x = primeP1.x - lineWidth;
        p1.y = primeP1.y + lineWidth;

        p2.x = primeP1.x - lineWidth;
        p2.y = primeP1.y - lineWidth;

        p3.x = primeP1.x;
        p3.y = primeP1.y + lineWidth;

        p4.x = primeP1.x;
        p4.y = primeP1.y - lineWidth;

        p5.x = primeP2.x;
        p5.y = primeP2.y + lineWidth;

        p6.x = primeP2.x;
        p6.y = primeP2.y - lineWidth;

        p7.x = primeP2.x + lineWidth;
        p7.y = primeP2.y + lineWidth;

        p8.x = primeP2.x + lineWidth;
        p8.y = primeP2.y - lineWidth;

        //////////////////////////////////////////////
        // Rotate

        p1 = mathutils::rotate2dPoint(p1, angle);
        p2 = mathutils::rotate2dPoint(p2, angle);
        p3 = mathutils::rotate2dPoint(p3, angle);
        p4 = mathutils::rotate2dPoint(p4, angle);
        p5 = mathutils::rotate2dPoint(p5, angle);
        p6 = mathutils::rotate2dPoint(p6, angle);
        p7 = mathutils::rotate2dPoint(p7, angle);
        p8 = mathutils::rotate2dPoint(p8, angle);

        //////////////////////////////////////////////
        // Translate

        p1 = mathutils::translate2dPoint(p1, mid);
        p2 = mathutils::translate2dPoint(p2, mid);
        p3 = mathutils::translate2dPoint(p3, mid);
        p4 = mathutils::translate2dPoint(p4, mid);
        p5 = mathutils::translate2dPoint(p5, mid);
        p6 = mathutils::translate2dPoint(p6, mid);
        p7 = mathutils::translate2dPoint(p7, mid);
        p8 = mathutils::translate2dPoint(p8, mid);

        //////////////////////////////////////////////
        // Draw

        float totalLength = length + lineWidth;
        float endCapTexturePoint1 = (1 / (totalLength / lineWidth));
        float endCapTexturePoint2 = 1-(1 / (totalLength / lineWidth));


        // Set the color
        glColor4f(penStyle.r, penStyle.g, penStyle.b, penStyle.a);


        glBegin(GL_TRIANGLE_STRIP);

        // P1
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(p1.x, p1.y, 0);

        // P2
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(p2.x, p2.y, 0);

        // P3
        glTexCoord2f(endCapTexturePoint1, 1.0f);
        glVertex3f(p3.x, p3.y, 0);

        // P4
        glTexCoord2f(endCapTexturePoint1, 0.0f);
        glVertex3f(p4.x, p4.y, 0);

        // P5
        glTexCoord2f(endCapTexturePoint2, 1.0f);
        glVertex3f(p5.x, p5.y, 0);

        // P6
        glTexCoord2f(endCapTexturePoint2, 0.0f);
        glVertex3f(p6.x, p6.y, 0);

        // P7
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(p7.x, p7.y, 0);

        // P8
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(p8.x, p8.y, 0);

        glEnd();
*/
    }


    void extendVector(Point3d* from, Point3d* to, double amount)
    {
        double angle = mathutils::calculate2dAngle(*from, *to) - mathutils::DegreesToRads(90);
        float distance = mathutils::calculate2dDistance(*from, *to) * amount;
        Point3d midPoint((from->x + to->x) / 2, (from->y + to->y) / 2, 0);

        Point3d vector1(0,distance/2,0);
        Point3d vector2(0,-distance/2,0);

        vector1 = mathutils::rotate2dPoint(vector1, angle);
        vector2 = mathutils::rotate2dPoint(vector2, angle);

        vector1 += midPoint;
        vector2 += midPoint;

        from->x = vector1.x;
        from->y = vector1.y;

        to->x = vector2.x;
        to->y = vector2.y;
    }

};

