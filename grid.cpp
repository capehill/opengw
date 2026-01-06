//
// grid.cpp
// Initial sample grid code from http://www.gamedev.net/topic/392830-mass-spring-grid-problem/#entry3611422
// Greatly expanded upon (added attractors, non-rectangular grids, etc) by Peter Hirschberg
//

#include "attractor.hpp"
#include "game.hpp"
#include "grid.hpp"
#include "mathutils.hpp"
#include "point3d.hpp"
#include "profiler.hpp"
#include "settings.hpp"

#include <atomic>
//#include <mutex>
#include <vector>

#include <cstdio>

#include "SDL_opengl.h"

// The Grid
// GW is 33w x 22h
const int grid::resolution_x = ((33 * 4) + 1);
const int grid::resolution_y = ((22 * 4) + 1);

// Stuff for our OpenGL grid arrays

struct Vertex
{
    GLfloat x = 0.0f;
    GLfloat y = 0.0f;

    GLfloat r = 0.0f;
    GLfloat g = 0.0f;
    GLfloat b = 0.0f;
    GLfloat a = 0.0f;
};

static std::vector<Vertex> gridVertices;
static std::vector<GLushort> gridElements;

static const unsigned int numGridLinesX = grid::resolution_x * grid::resolution_y;
static const unsigned int numGridLinesY = numGridLinesX;

/*
static SDL_mutex* game::mAttractors.mMutex;
static SDL_mutex* mDrawMutex;
*/

static SDL_Thread* mRunThread = nullptr;
static std::atomic_bool mRunFlag { false };
static std::atomic_bool quitFlag { false };

static float q;
static float damping;
static const float dt = .3;

struct GridPoint
{
    Point3d pos;
    Point3d vel;
};

static std::vector<GridPoint> mGrid;

// static std::mutex m;

// #define GRID_GLOW // PERFORMANCE: Making the grid glow causes us to have to draw it twice, which is slower. This is also defined in game.cpp!

static int runThread(void* /*ptr*/)
{
    profiler prof("grid");

    printf("Grid thread running\n");

    while (!quitFlag) {
        while (!mRunFlag && !quitFlag) {
            SDL_Delay(1);
        }
        mRunFlag = false;

        prof.start();

        // TODO: fix data races
        // std::unique_lock<std::mutex> lock(m);

        // Apply attractors
        for (attractor::Attractor& att: theGame->mAttractors->mAttractors) {
            if (att.enabled) {
                // Evaluate every point on the grid for this attractor
                const int xstart = 1;
                const int ystart = 1;
                const int xend = grid::resolution_x - 2;
                const int yend = grid::resolution_y - 2;

                const Point3d& apoint = att.pos;

                const float arSquared = att.radius * att.radius;

                for (int y = ystart; y <= yend; ++y) {
                    GridPoint* p = &mGrid[y * grid::resolution_x];

                    for (int x = xstart; x <= xend; ++x) {
                        // Point3d gpoint = (gridxy(x,y).pos + gridxy(x,y).center) * .5;
                        p++;
                        const Point3d& gpoint = p->pos;

                        const float distance = mathutils::calculate2dDistanceSquared(gpoint, apoint);

                        if (distance < arSquared && distance > 0.0f) {
                            // distance = (distance*distance); // Simulate gravity with distance squared

                            const float angle = mathutils::calculate2dAngle(gpoint, apoint);
                            const float strength = att.strength;

                            const Point3d gravityVector(-distance * strength, 0.0f, 0.0f);
                            const Point3d g = mathutils::rotate2dPoint(gravityVector, angle);

                            p->vel.x += g.x * .005f;
                            p->vel.y += g.y * .005f;
                        }
                    }
                }

                // Now that we've processed this attractor we can clear it out
                att.enabled = false;
            }
        }

        const float accel_c = -q * dt;
        const float damping_multiplier = exp(-dt * damping);

        // Run the grid
        for (int pass = 0; pass < settings::get().mGridPasses; pass++) {
            for (int y = 1; y < grid::resolution_y - 1; ++y) {
                GridPoint* p = &mGrid[y * grid::resolution_x];

                for (int x = 1; x < grid::resolution_x - 1; ++x) {
                    p++;
                    // Weigh against neighbors
                    const Point3d& p1 = (p - 1)->pos;
                    const Point3d& p2 = (p + 1)->pos;
                    const Point3d& p3 = (p - grid::resolution_x)->pos;
                    const Point3d& p4 = (p + grid::resolution_x)->pos;

                    // Average the point
                    // avg_pos = (p1+p2+p3+p4) * .25;
                    const Point3d avg_pos((p1.x + p2.x + p3.x + p4.x) * 0.25f,
                                          (p1.y + p2.y + p3.y + p4.y) * 0.25f,
                                          0.0f);

                    p->vel += (p->pos - avg_pos) * accel_c;
                    p->vel *= damping_multiplier;
                    p->pos += p->vel * dt;

                    // Keep the points in bounds
                    if (p->pos.x < 0)
                        p->pos.x = 0;
                    else if (p->pos.x > grid::resolution_x - 1)
                        p->pos.x = grid::resolution_x - 1;
                    if (p->pos.y < 0)
                        p->pos.y = 0;
                    else if (p->pos.y > grid::resolution_y - 1)
                        p->pos.y = grid::resolution_y - 1;
                }
            }
        }

        prof.stop();
    }

    printf("Grid thread exiting\n");

    return 0;
}

grid::grid()
{
    mGrid.resize(resolution_x * resolution_y);
    q = 12;
    damping = 1.5f;

    // Create our array of grid points
    for (int y = 0; y < resolution_y; ++y) {
        for (int x = 0; x < resolution_x; ++x) {
            GridPoint& p = mGrid[x + y * grid::resolution_x];

            p.pos = Point3d(x, y, 0);
            p.vel = Point3d(0, 0, 0);
        }
    }

    // Create our OpenGL vertex and color array
    gridVertices.reserve(resolution_x * resolution_y);
    gridElements.reserve(numGridLinesX + numGridLinesY);

    initializeVertices();
    initializeElements();

    // Thread stuff
    mRunThread = SDL_CreateThread(runThread, "grid", nullptr);
    if (!mRunThread) {
        printf("Couldn't create grid run thread: %s\n", SDL_GetError());
    }
}

grid::~grid()
{
    quitFlag = true;

    int status = 0;
    SDL_WaitThread(mRunThread, &status);

    printf("Grid thread exited with status %d\n", status);
}

void grid::initializeVertices()
{
    // Init the grid line colors up front so it only has to happen once

#ifdef GRID_GLOW
    const vector::pen darkColor(0.2f, 0.2f, 1.0f, (0.15f * ((scene::mPass == scene::RENDERPASS_PRIMARY) ? 0.75f : 0.25f)) * brightness, 0);
    const vector::pen lightColor(0.2f, 0.2f, 1.0f, (0.4f * ((scene::mPass == scene::RENDERPASS_PRIMARY) ? 0.75f : 0.25f)) * brightness, 0);
#else
    const vector::pen darkColor(0.4f, 0.4f, 1.0f, 0.15f * brightness, 0);
    const vector::pen lightColor(0.4f, 0.4f, 1.0f, 0.4f * brightness, 0);
#endif

    // Vertices for dark lines
    for (int i = 0; i < resolution_x * resolution_y; i++) {
        Vertex v {};
        v.r = darkColor.r;
        v.g = darkColor.g;
        v.b = darkColor.b;
        v.a = darkColor.a;
        gridVertices.push_back(v);
    }

    lightStartHorizontal = gridVertices.size();

    // Horizontal light lines
    for (int y = 0; y < resolution_y; y += 4) {
        for (int x = 0; x < resolution_x; x++) {
            Vertex v {};
            v.r = lightColor.r;
            v.g = lightColor.g;
            v.b = lightColor.b;
            v.a = lightColor.a;
            gridVertices.push_back(v);
        }
    }

    lightStartVertical = gridVertices.size();

    // Vertical light lines
    for (int x = 0; x < resolution_x; x+= 4) {
        for (int y = 0; y < resolution_y; y++) {
            Vertex v {};
            v.r = lightColor.r;
            v.g = lightColor.g;
            v.b = lightColor.b;
            v.a = lightColor.a;
            gridVertices.push_back(v);
        }
    }
}

void grid::initializeElements()
{
    // Horizontal dark
    for (int y = 0; y < resolution_y; y++) {
        GLuint vertex = y * resolution_x;
        for (int x = 0; x < resolution_x - 1; x++) {
            if (y % 4 > 0) {
                gridElements.push_back(vertex++);
                gridElements.push_back(vertex);
            }
        }
    }

    // Vertical dark
    for (int x = 0; x < resolution_x; x++) {
        GLuint vertex = x;
        for (int y = 0; y < resolution_y - 1; y++) {
            if (x % 4 > 0) {
                gridElements.push_back(vertex);
                vertex += resolution_x;
                gridElements.push_back(vertex);
            }
        }
    }

    // Horizontal light
    for (int y = 0; y < resolution_y; y++) {
        GLuint vertex = lightStartHorizontal + y * resolution_x / 4;
        for (int x = 0; x < resolution_x - 1; x++) {
            if (y % 4 == 0) {
                gridElements.push_back(vertex++);
                gridElements.push_back(vertex);
            }
        }
    }

    // Vertical light
    for (int x = 0; x < resolution_x; x++) {
        GLuint vertex = lightStartVertical + x * resolution_y / 4;
        for (int y = 0; y < resolution_y - 1; y++) {
            if (x % 4 == 0) {
                gridElements.push_back(vertex++);
                gridElements.push_back(vertex);
            }
        }
    }
}

void grid::run()
{
    mRunFlag = true;
}

static profiler prof1("grid_draw_update");
static profiler prof2("grid_draw_opengl");

void grid::draw()
{
    if (brightness <= 0.05f)
        return;

    // std::unique_lock<std::mutex> lock(m);
    prof1.start();

    glLineWidth(5.0f);

    std::size_t vertex = 0;

    // Update dark lines positions
    for (int y = 0; y < resolution_y; ++y) {
        GridPoint* p = &mGrid[y * resolution_x];

        for (int x = 0; x < resolution_x; ++x) {
            gridVertices[vertex].x = p->pos.x;
            gridVertices[vertex].y = p->pos.y;
            vertex++;
            p++;
        }
    }

    vertex = lightStartHorizontal;

    // Update horizontal light lines
    for (int y = 0; y < resolution_y; y += 4) {
        GridPoint* p = &mGrid[y * resolution_x];

        for (int x = 0; x < resolution_x; x++) {
            gridVertices[vertex].x = p->pos.x;
            gridVertices[vertex].y = p->pos.y;
            vertex++;
            p++;
        }
    }

    vertex = lightStartVertical;

    // Update vertical light lines
    for (int x = 0; x < resolution_x; x += 4) {
        GridPoint* p = &mGrid[x];

        for (int y = 0; y < resolution_y; y++) {
            gridVertices[vertex].x = p->pos.x;
            gridVertices[vertex].y = p->pos.y;
            vertex++;
            p += resolution_x;
        }
    }

    prof1.stop();
    prof2.start();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    // Draw lines
    glVertexPointer(2, GL_FLOAT, sizeof(Vertex), gridVertices.data());
    glColorPointer(4, GL_FLOAT, sizeof(Vertex), ((char*)gridVertices.data() + 2 * sizeof(GLfloat)));
    glDrawElements(GL_LINES, gridElements.size(), GL_UNSIGNED_SHORT, gridElements.data());

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    prof2.stop();

    // Grid outline

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // RGBA

    glLineWidth(4.0f);

    glBegin(GL_LINE_LOOP);

    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(grid::resolution_x - 1, 0.0f, 0.0f);
    glVertex3f(grid::resolution_x - 1, grid::resolution_y - 1, 0.0f);
    glVertex3f(0.0f, grid::resolution_y - 1, 0.0f);

    glEnd();

    // If the brightness has been lowered, cover the grid with a semi transparent scrim
    // since all our grid colors are locked.

    if (brightness < 0.99f) {
        glColor4f(0.0f, 0.0f, 0.0f, 1 - brightness);

        glBlendFunc(GL_DST_COLOR, GL_ONE_MINUS_SRC_ALPHA);

        glBegin(GL_QUADS);

        glVertex3f(-10.0f, -10.0f, 0.0f);
        glVertex3f(grid::resolution_x + 10, -10.0f, 0.0f);
        glVertex3f(grid::resolution_x + 10, grid::resolution_y + 10, 0.0f);
        glVertex3f(-10.0f, grid::resolution_y + 10, 0.0f);

        glEnd();

        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
}

bool grid::hitTest(const Point3d& pos, float radius, Point3d* hitPos, Point3d* speed)
{
    bool hit = false;

    if (hitPos)
        *hitPos = pos;

    const float left = 0 + radius;
    const float bottom = 0 + radius;
    const float right = resolution_x - radius;
    const float top = resolution_y - radius;

    if (pos.x < left) {
        if (hitPos)
            hitPos->x = left;
        if (speed)
            speed->x = -speed->x;
        hit = true;
    } else if (pos.x > right - 1) {
        if (hitPos)
            hitPos->x = right - 1;
        if (speed)
            speed->x = -speed->x;
        hit = true;
    }

    if (pos.y < bottom) {
        if (hitPos)
            hitPos->y = bottom;
        if (speed)
            speed->y = -speed->y;
        hit = true;
    } else if (pos.y > top - 1) {
        if (hitPos)
            hitPos->y = top - 1;
        if (speed)
            speed->y = -speed->y;
        hit = true;
    }

    return hit;
}
