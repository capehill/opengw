#ifndef GRID_H
#define GRID_H

#include "point3d.h"

class grid
{
public:

    static const int resolution_x;
    static const int resolution_y;

    typedef struct gridPoint
    {
        Point3d pos;
        Point3d vel;
        Point3d center;
    }GridPoint;

    grid();

    void run();
    void draw();

    int extentX() { return resolution_x; }
    int extentY() { return resolution_y; }

	bool hitTest(const Point3d& pos, float radius, Point3d* hitPos=0, Point3d* speed=0);

    float brightness;

private:
    float q;
    float damping;

    GridPoint* mGrid;
};

#endif // GRID_H






#if 0

#ifndef GRID_H
#define GRID_H

#include "point3d.h"
#include "vector.h"

class grid
{
public:

    typedef struct
    {
        Point3d pos;
        Point3d vel;
    }gridPoint;

    typedef struct
    {
        vector::pen brightGridColor;
        vector::pen dimGridColor;

        gridPoint* grid;

        int divsX;
        int divsY;

        Point3d* poly;
        int polyPoints;

        float scale;

        Point3d playerSpawnPoint;
        Point3d enemySpawnPoints[4];

    }gridInfo;

	enum
	{
		FADESTATUS_IDLE = 0,
		FADESTATUS_FADEOUT,
		FADESTATUS_FADEIN
	};

    grid();
    ~grid();

    void run();
    void draw();

	bool hitTest(const Point3d& pos, float radius, Point3d* hitPos=0, Point3d* speed=0);

	void startLevel(int level);
	void endLevel();

    int extentX() { return mGridExtentX; }
    int extentY() { return mGridExtentY; }

    float brightness() { return mBrightness; }

private:
    float q;
    float damping;

    float mBrightness;
	int mLevel;
	int mFadeStatus;

    float mGridPointScaleX;
    float mGridPointScaleY;

    int mGridExtentX;
    int mGridExtentY;

    gridInfo mGridInfo;

    void initGrid(int level);
};

#endif // GRID_H
#endif