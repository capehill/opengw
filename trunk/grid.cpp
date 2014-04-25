//
// grid.cpp
// Initial sample grid code from http://www.gamedev.net/topic/392830-mass-spring-grid-problem/#entry3611422
// Greatly expanded upon (added attractors, non-rectangular grids, etc) by Peter Hirschberg
//


#include "grid.h"
#include "game.h"


// The Grid
// GW is 33w x 22h
const int grid::resolution_x = ((33*4)+1);
const int grid::resolution_y = ((22*4)+1);

#define gridxy(x,y) (mGrid[(x)+(y)*grid::resolution_x])


// Stuff for our OpenGL grid arrays

typedef struct
{
	GLfloat x, y, z;
}Vertex3f;

typedef struct
{
	GLfloat r, g, b, a;
}Color4f;

static Vertex3f*        gridVertexArrayX;
static Vertex3f*        gridVertexArrayY;
static Color4f*         gridColorArrayX;
static Color4f*         gridColorArrayY;
static unsigned int     numGridLinesX;
static unsigned int     numGridLinesY;

/*
static SDL_mutex* game::mAttractors.mMutex;
static SDL_mutex* mDrawMutex;
*/

static SDL_Thread* mRunThread = NULL;

static float q;
static float damping;
static const float dt = .3;

typedef struct gridPoint
{
    Point3d pos;
    Point3d vel;
    Point3d center;
}GridPoint;

static GridPoint* mGrid;

static bool mRunFlag = false;

//#define GRID_GLOW // PERFORMANCE: Making the grid glow causes us to have to draw it twice, which is slower. This is also defined in game.cpp!

static int runThread(void *ptr)
{
    int x,y;

    while(1)
    {
        while (!mRunFlag)
        {
            Sleep(1);
        };
        mRunFlag = false;

        game::mAttractors.lock();
        {
            // Apply attractors
            for (int a=0; a<game::mAttractors.mNumAttractors; a++)
            {
                attractor::Attractor att = game::mAttractors.mAttractors[a];
                if (att.enabled)
                {
                    // Evaluate every point on the grid for this attractor
                    int xstart = 1;
                    int ystart = 1;
                    int xend = grid::resolution_x-2;
                    int yend = grid::resolution_y-2;

                    for(y=ystart; y<=yend; ++y)
                    {
                        for(x=xstart; x<=xend; ++x)
                        {
                            Point3d gpoint = gridxy(x,y).pos;
                            Point3d apoint = att.pos;

                            float distance = mathutils::calculate2dDistance(gpoint, apoint);

                            if ((distance < att.radius) && distance > 0)
                            {
                                distance = (distance*distance); // Simulate gravity with distance squared

                                float angle = mathutils::calculate2dAngle(gpoint, apoint);
                                float strength = att.strength;

                                Point3d gravityVector(-distance * strength, 0, 0);
                                Point3d g = mathutils::rotate2dPoint(gravityVector, angle);

                                gridxy(x,y).vel.x += g.x * .005;
                                gridxy(x,y).vel.y += g.y * .005;
                            }

                        }
                    }

                    // Now that we've processed this attractor we can clear it out
                    game::mAttractors.mAttractors[a].enabled = false;

                }
            }
        }
        game::mAttractors.unlock();

        // Run the grid
        float accel_c = -q * dt;
        float damping_multiplier = exp(-dt * damping);
        for(y=1; y<grid::resolution_y-1; ++y)
        {
            for(x=1; x<grid::resolution_x-1; ++x)
            {
                // Weigh against neighbors
                Point3d p1 = gridxy(x-1,y).pos;
                Point3d p2 = gridxy(x+1,y).pos;
                Point3d p3 = gridxy(x,y-1).pos;
                Point3d p4 = gridxy(x,y+1).pos;

                // Average the point
                // avg_pos = (p1+p2+p3+p4) * .25;
                Point3d avg_pos;
                avg_pos.x = (p1.x + p2.x + p3.x + p4.x) / 4;
                avg_pos.y = (p1.y + p2.y + p3.y + p4.y) / 4;

                gridxy(x,y).vel += (gridxy(x,y).pos - avg_pos) * accel_c;
                gridxy(x,y).vel *= damping_multiplier;
                gridxy(x,y).pos += gridxy(x,y).vel * dt;

                // Keep the points in bounds
                if (gridxy(x,y).pos.x < 0)
                    gridxy(x,y).pos.x = 0;
                else if (gridxy(x,y).pos.x > grid::resolution_x-1)
                    gridxy(x,y).pos.x = grid::resolution_x-1;
                if (gridxy(x,y).pos.y < 0)
                    gridxy(x,y).pos.y = 0;
                else if (gridxy(x,y).pos.y > grid::resolution_y-1)
                    gridxy(x,y).pos.y = grid::resolution_y-1;

            }
        }
    };


    return 0;
}

grid::grid()
{
    brightness = 1;
    mGrid = new gridPoint[resolution_x * resolution_y];
    q=12; // 12
    damping = 1.5; // 1.5

    // Create our array of grid points
    int x,y;
    for(y=0;y<resolution_y;++y)
    {
        for(x=0;x<resolution_x;++x)
        {
            gridxy(x,y).pos=Point3d(x,y,0);
            gridxy(x,y).center=Point3d(x,y,0);
            gridxy(x,y).vel=Point3d(0,0,0);
        }
    }


    numGridLinesX = 0;
    numGridLinesY = 0;

    // Horizontal line count
    for (int y=0; y<resolution_y; ++y)
    {
        for (int x=0; x<resolution_x; ++x)
        {
            ++numGridLinesX;
        }
    }
    // Vertical line count
    for (int x=0; x<resolution_x; ++x)
    {
        for (int y=0; y<resolution_y; ++y)
        {
            ++numGridLinesY;
        }
    }

    // Create our OpenGL vertex and color arrays
    gridVertexArrayX = new Vertex3f[numGridLinesX*2];
    gridVertexArrayY = new Vertex3f[numGridLinesY*2];
    gridColorArrayX = new Color4f[numGridLinesX*2];
    gridColorArrayY = new Color4f[numGridLinesY*2];

    // Init the grid line colors up front so it only has to happen once

#ifdef GRID_GLOW
    vector::pen darkColor(0.2f, 0.2f, 1.0f, (0.15f * ((scene::mPass == scene::RENDERPASS_PRIMARY) ? .75 : .25)) * brightness, 0);
    vector::pen lightColor(0.2f, 0.2f, 1.0f, (0.4f * ((scene::mPass == scene::RENDERPASS_PRIMARY) ? .75 : .25)) * brightness, 0);
#else
    vector::pen darkColor(0.2f, 0.2f, 1.0f, 0.12f * brightness, 0);
    vector::pen lightColor(.2f, 0.2f, 1.0f, 0.3f * brightness, 0);
#endif

    unsigned int i=0;

    // Horizontal lines
    for (y=0; y<resolution_y; ++y)
    {
        for (x=0; x<resolution_x-1; ++x)
        {
            if (y%4==0)
            {
                gridColorArrayX[i].r = lightColor.r;
                gridColorArrayX[i].g = lightColor.g;
                gridColorArrayX[i].b = lightColor.b;
                gridColorArrayX[i].a = lightColor.a;

                i++;

                gridColorArrayX[i].r = lightColor.r;
                gridColorArrayX[i].g = lightColor.g;
                gridColorArrayX[i].b = lightColor.b;
                gridColorArrayX[i].a = lightColor.a;

                i++;
            }
            else
            {
                gridColorArrayX[i].r = darkColor.r;
                gridColorArrayX[i].g = darkColor.g;
                gridColorArrayX[i].b = darkColor.b;
                gridColorArrayX[i].a = darkColor.a;

                i++;

                gridColorArrayX[i].r = darkColor.r;
                gridColorArrayX[i].g = darkColor.g;
                gridColorArrayX[i].b = darkColor.b;
                gridColorArrayX[i].a = darkColor.a;

                i++;
            }
        }
    }

    i=0;

    // Vertical lines
    for (x=0; x<resolution_x; ++x)
    {
        for (y=0; y<resolution_y-1; ++y)
        {
            if (x%4==0)
            {
                gridColorArrayY[i].r = lightColor.r;
                gridColorArrayY[i].g = lightColor.g;
                gridColorArrayY[i].b = lightColor.b;
                gridColorArrayY[i].a = lightColor.a;

                i++;

                gridColorArrayY[i].r = lightColor.r;
                gridColorArrayY[i].g = lightColor.g;
                gridColorArrayY[i].b = lightColor.b;
                gridColorArrayY[i].a = lightColor.a;

                i++;
            }
            else
            {
                gridColorArrayY[i].r = darkColor.r;
                gridColorArrayY[i].g = darkColor.g;
                gridColorArrayY[i].b = darkColor.b;
                gridColorArrayY[i].a = darkColor.a;

                i++;

                gridColorArrayY[i].r = darkColor.r;
                gridColorArrayY[i].g = darkColor.g;
                gridColorArrayY[i].b = darkColor.b;
                gridColorArrayY[i].a = darkColor.a;

                i++;
            }
        }
    }

    // Thread stuff
    mRunThread = SDL_CreateThread(runThread, "runThread");
    if (!mRunThread)
    {
        OutputDebugString(L"Couldn't create grid run thread\n");
    }

}

grid::~grid()
{
    delete gridVertexArrayX;
    delete gridVertexArrayY;
    delete gridColorArrayX;
    delete gridColorArrayY;
}

void grid::run()
{
    mRunFlag = true;
}

void grid::draw()
{
    if (brightness <= .05) return;

    glLineWidth(5);

    unsigned int idxVertex = 0;

    // Horizontal lines
    int x,y;
    for (y=0; y<resolution_y; ++y)
    {
        for (x=0; x<resolution_x-1; ++x)
        {
            Point3d from = gridxy(x,y).pos;
            Point3d to = gridxy(x+1,y).pos;

            gridVertexArrayX[idxVertex].x = from.x;
            gridVertexArrayX[idxVertex].y = from.y;
            gridVertexArrayX[idxVertex].z = 0;
            ++idxVertex;

            gridVertexArrayX[idxVertex].x = to.x;
            gridVertexArrayX[idxVertex].y = to.y;
            gridVertexArrayX[idxVertex].z = 0;
            ++idxVertex;
        }
    }

    idxVertex = 0;

    // Vertical lines
    for (x=0; x<resolution_x; ++x)
    {
        for (y=0; y<resolution_y-1; ++y)
        {
            Point3d from = gridxy(x,y).pos;
            Point3d to = gridxy(x,y+1).pos;

            gridVertexArrayY[idxVertex].x = from.x;
            gridVertexArrayY[idxVertex].y = from.y;
            gridVertexArrayY[idxVertex].z = 0;
            ++idxVertex;

            gridVertexArrayY[idxVertex].x = to.x;
            gridVertexArrayY[idxVertex].y = to.y;
            gridVertexArrayY[idxVertex].z = 0;
            ++idxVertex;
        }
    }

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

    // Draw horizontal lines array
	glVertexPointer(3, GL_FLOAT, 0, gridVertexArrayX);
	glColorPointer(4, GL_FLOAT, 0, gridColorArrayX);
    glDrawArrays(GL_LINES, 0, numGridLinesX*2);

    // Draw vertical lines array
	glVertexPointer(3, GL_FLOAT, 0, gridVertexArrayY);
	glColorPointer(4, GL_FLOAT, 0, gridColorArrayY);
    glDrawArrays(GL_LINES, 0, numGridLinesY*2);

	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

    // Grid outline

    glColor4f(1,1,1,1); // RGBA

    glLineWidth(4);

    glBegin(GL_LINE_LOOP);

    glVertex3f(0, 0, 0 );
    glVertex3f(grid::resolution_x-1, 0, 0 );
    glVertex3f(grid::resolution_x-1, grid::resolution_y-1, 0 );
    glVertex3f(0, grid::resolution_y-1, 0 );

    glEnd();

    // If the brightness has been lowered, cover the grid with a semi transparent scrim
    // since all our grid colors are locked.

    if (brightness < .99)
    {
        glColor4f(0, 0, 0, 1-brightness);

        glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_QUADS);

        glVertex3f(-10, -10, 0 );
        glVertex3f(grid::resolution_x+10, -10, 0 );
        glVertex3f(grid::resolution_x+10, grid::resolution_y+10, 0 );
        glVertex3f(-10, grid::resolution_y+10, 0 );

        glEnd();

	    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

}

bool grid::hitTest(const Point3d& pos, float radius, Point3d* hitPos/*=0*/, Point3d* speed/*=0*/)
{
    bool hit = false;

	if (hitPos) *hitPos = pos;

    float left = 0 + radius;
    float bottom = 0 + radius;
    float right = resolution_x - radius;
    float top = resolution_y - radius;

    if (pos.x < left)
    {
        if (hitPos) hitPos->x = left;
        if (speed) speed->x = -speed->x;
        hit = true;
    }
    else if (pos.x > right-1)
    {
        if (hitPos) hitPos->x = right-1;
        if (speed) speed->x = -speed->x;
        hit = true;
    }

    if (pos.y < bottom)
    {
        if (hitPos) hitPos->y = bottom;
        if (speed) speed->y = -speed->y;
        hit = true;
    }
    else if (pos.y > top-1)
    {
        if (hitPos) hitPos->y = top-1;
        if (speed) speed->y = -speed->y;
        hit = true;
    }

    return hit;
}



