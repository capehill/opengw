//
// grid.cpp
// Initial sample grid code from http://www.gamedev.net/topic/392830-mass-spring-grid-problem/#entry3611422
// Greatly expanded upon (added attractors, non-rectangular grids, etc) by Peter Hirschberg
//


#include "grid.h"
#include "game.h"


// Static initializers
// GW is 33w x 22h
const int grid::resolution_x = ((33*4)+1);
const int grid::resolution_y = ((22*4)+1);


#define gridxy(x,y) (mGrid[(x)+(y)*resolution_x])


grid::grid()
{
    brightness = 1;
    mGrid = new gridPoint[resolution_x*resolution_y];
    q=12; // 12
    damping = 1.5; // 1.5
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
}

//#define GRID_GLOW // PERFORMANCE: Making the grid glow causes us to have to draw it twice, which is slower. This is also defined in game.cpp!
#define ENABLE_GRID // PERFORMANCE: This obviously has a MASSIVE effect on performance
#define CLUMP_ATTRACTORS // PERFORMANCE: Clump attractors that are close to each other for increased performance, but sacrificing accuracy

void grid::run()
{
    if (brightness <= 0) return;

#ifdef ENABLE_GRID

    // PERFORMANCE: This function is very slow!

	const float dt = .3;

    int x,y;

#if 0
    // My attempt at turning the loop inside out.
    // Performance was MUCH MUCH MUCH worse this way
    int xstart = 1;
    int ystart = 1;
    int xend = resolution_x-2;
    int yend = resolution_y-2;

    for(y=ystart; y<=yend; ++y)
    {
        for(x=xstart; x<=xend; ++x)
        {
            for (int a=0; a<game::mAttractors.mNumAttractors; a++)
            {
                attractor::Attractor att = game::mAttractors.mAttractors[a];
                if (att.enabled)
                {
                    Point3d gpoint = gridxy(x,y).pos;
                    Point3d apoint = att.pos;

                    float distance = mathutils::calculate2dDistance(gpoint, apoint);

                    if ((distance < att.radius) && distance > 0)
                    {
                        distance = (distance*distance);
                        if (distance < 1) distance += .1;

                        float angle = mathutils::calculate2dAngle(gpoint, apoint);
                        float strength = att.strength;

                        Point3d gravityVector(-distance * strength, 0, 0);
                        Point3d g = mathutils::rotate2dPoint(gravityVector, angle);

                        gridxy(x,y).vel.x += g.x * .005;
                        gridxy(x,y).vel.y += g.y * .005;
                    }
                }
            }
        }
    }
#endif

    // PERFORMANCE: Too many attractors will KILL performance so we
    // go through the current active list and create a smaller list
    // by clumping together any attractors that are near each other
    // into a single attractor.
    //
    // The larger the value for clumpDistance, the faster things will
    // run, but the less accurate the grid effect will be.
    // 
#ifdef CLUMP_ATTRACTORS
    const float clumpDistance = 10;
    attractor attractorList;
    for (int a=0; a<game::mAttractors.mNumAttractors; a++)
    {
        attractor::Attractor att1 = game::mAttractors.mAttractors[a];
        if (att1.enabled)
        {
            bool clumped = false;

            for (int c=0; c<attractorList.mNumAttractors; c++)
            {
                attractor::Attractor att2 = attractorList.mAttractors[c];
                if (att2.enabled)
                {
                    float distance = mathutils::calculate2dDistance(att1.pos, att2.pos);
                    if (distance < clumpDistance)
                    {
                        // Merge them
                        att2.pos = (att1.pos + att2.pos) * .5;
                        att2.strength = att1.strength + att2.strength;
                        if (att2.radius < att1.radius) att2.radius = att1.radius;
                        clumped = true;
                    }
                }
            }

            if (!clumped)
            {
                // We didn't find an attractor to clump this one to, so just create
                // a new one in our shortened list. Perhaps it will clump later as
                // we go through the rest of the list.
                attractor::Attractor* newAttractor = attractorList.getAttractor();
                if (newAttractor)
                {
                    *newAttractor = att1;
                }
            }

        }
    }
#endif

    // Apply attractors
#ifdef CLUMP_ATTRACTORS
    for (int a=0; a<attractorList.mNumAttractors; a++)
    {
        attractor::Attractor att = attractorList.mAttractors[a];
#else
    for (int a=0; a<game::mAttractors.mNumAttractors; a++)
    {
        attractor::Attractor att = game::mAttractors.mAttractors[a];
#endif
        if (att.enabled)
        {
            // Find all the grid points within the radius of this attractor

            // PERFORMANCE: Ideally we could just check around the radius of this attractor
            // and apply its force to just those grid points within the radius but the grid
            // points move around and we don't have any idea where they are at any particular
            // time when we are checking for distance from the attractor without actually 
            // evaluating every single point on the grid. There's gotta be a way. :-(
#if 0
            float radius = att.radius;
            int xstart = att.pos.x - radius;
            int xend = att.pos.x + radius;
            if (xstart>xend)
            {
                int temp = xend;
                xend = xstart;
                xstart = temp;
            }
            if (xstart < 1)
            {
                xstart = 1;
            }
            else if (xstart > resolution_x-2)
            {
                xstart = resolution_x-2;
            }
            if (xend < 1)
            {
                xend = 1;
            }
            else if (xend > resolution_x-2)
            {
                xend = resolution_x-2;
            }

            int ystart = att.pos.y - radius;
            int yend = att.pos.y + radius;
            if (ystart>yend)
            {
                int temp = yend;
                yend = ystart;
                ystart = temp;
            }
            if (ystart < 1)
            {
                ystart = 1;
            }
            else if (ystart > resolution_y-2)
            {
                ystart = resolution_y-2;
            }
            if (yend < 1)
            {
                yend = 1;
            }
            else if (yend > resolution_y-2)
            {
                yend = resolution_y-2;
            }

#else
            // Evaluate every point on the grid for this attractor
            int xstart = 1;
            int ystart = 1;
            int xend = resolution_x-2;
            int yend = resolution_y-2;
#endif      

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
                        //if (distance < 1) distance += .1; // What is this for??

                        float angle = mathutils::calculate2dAngle(gpoint, apoint);
                        float strength = att.strength;

                        Point3d gravityVector(-distance * strength, 0, 0);
                        Point3d g = mathutils::rotate2dPoint(gravityVector, angle);

                        gridxy(x,y).vel.x += g.x * .005;
                        gridxy(x,y).vel.y += g.y * .005;
                    }

                }
            }


        }
    }


    // Run the grid
    // PERFORMANCE: The grid is much more responsive if we run it more than one time per frame
    const int numFrames = 3;
    float accel_c = -q * dt;
    float damping_multiplier = exp(-dt * damping);
    for (int frame=0; frame<numFrames; frame++)
    {
        for(y=1; y<resolution_y-1; ++y)
        {
            for(x=1; x<resolution_x-1; ++x)
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
                else if (gridxy(x,y).pos.x > resolution_x-1)
                    gridxy(x,y).pos.x = resolution_x-1;
                if (gridxy(x,y).pos.y < 0)
                    gridxy(x,y).pos.y = 0;
                else if (gridxy(x,y).pos.y > resolution_y-1)
                    gridxy(x,y).pos.y = resolution_y-1;

            }
        }
    }

#endif
}

void grid::draw()
{
    if (brightness <= 0) return;

#ifdef ENABLE_GRID

    // PERFORMANCE: This function also slows things down quite a bit. LOTS of lines to be drawn!

#ifdef GRID_GLOW
    vector::pen darkColor(0.2f, 0.2f, 1.0f, (0.15f * ((scene::mPass == scene::RENDERPASS_PRIMARY) ? .75 : .25)) * brightness, 0);
    vector::pen lightColor(0.2f, 0.2f, 1.0f, (0.4f * ((scene::mPass == scene::RENDERPASS_PRIMARY) ? .75 : .25)) * brightness, 0);
#else
    vector::pen darkColor(0.2f, 0.2f, 1.0f, 0.15f * brightness, 0);
    vector::pen lightColor(.2f, 0.2f, 1.0f, 0.4f * brightness, 0);
#endif

    // Horizontal lines
    for (int y=0; y<resolution_y; ++y)
    {
        if (y%4==0) glColor4f(lightColor.r, lightColor.g, lightColor.b, lightColor.a);
        else glColor4f(darkColor.r, darkColor.g, darkColor.b, darkColor.a);

        glBegin(GL_LINE_STRIP);
        for (int x=0; x<resolution_x-1; ++x)
        {
            Point3d p1 = gridxy(x,y).pos;
            Point3d p2 = gridxy(x+1,y).pos;

            glVertex3d(p1.x, p1.y, p1.z);
            glVertex3d(p2.x, p2.y, p2.z);
        }
        glEnd();
    }

    // Vertical lines
    for (int x=0; x<resolution_x; ++x)
    {
        if (x%4==0) glColor4f(lightColor.r, lightColor.g, lightColor.b, lightColor.a);
        else glColor4f(darkColor.r, darkColor.g, darkColor.b, darkColor.a);

        glBegin(GL_LINE_STRIP);
        for (int y=0; y<resolution_y-1; ++y)
        {
            Point3d p1 = gridxy(x,y).pos;
            Point3d p2 = gridxy(x,y+1).pos;

            glVertex3d(p1.x, p1.y, p1.z);
            glVertex3d(p2.x, p2.y, p2.z);
        }
        glEnd();
    }

#endif

    // Grid outline
    glColor4f(1.0f, 1.0f, 1.0f, 0.7f * brightness); // RGBA

    glLineWidth(7);

    glBegin(GL_LINE_LOOP);

    glVertex3f(0, 0, 0 );
    glVertex3f(grid::resolution_x-1, 0, 0 );
    glVertex3f(grid::resolution_x-1, grid::resolution_y-1, 0 );
    glVertex3f(0, grid::resolution_y-1, 0 );

    glEnd();

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
    else if (pos.x > right)
    {
        if (hitPos) hitPos->x = right;
        if (speed) speed->x = -speed->x;
        hit = true;
    }

    if (pos.y < bottom)
    {
        if (hitPos) hitPos->y = bottom;
        if (speed) speed->y = -speed->y;
        hit = true;
    }
    else if (pos.y > top)
    {
        if (hitPos) hitPos->y = top;
        if (speed) speed->y = -speed->y;
        hit = true;
    }

    return hit;
}



