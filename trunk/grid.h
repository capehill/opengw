#ifndef GRID_H
#define GRID_H

#include "point3d.h"

class grid
{
public:

    static const int resolution_x;
    static const int resolution_y;

    grid();
    ~grid();

    void run();
    void draw();

    int extentX() { return resolution_x; }
    int extentY() { return resolution_y; }

	bool hitTest(const Point3d& pos, float radius, Point3d* hitPos=0, Point3d* speed=0);

    float brightness;
};

#endif // GRID_H

