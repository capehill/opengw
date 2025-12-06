#pragma once

struct Point3d;

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

	bool hitTest(const Point3d& pos, float radius, Point3d* hitPos = nullptr, Point3d* speed = nullptr);

    float brightness = 1.0f;
};
