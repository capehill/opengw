#include "model.hpp"

#include <SDL3/SDL_opengl.h>

model::model()
{
    mMatrix.Identity();

    mIsLineLoop = false;
}

model::~model()
{
}

void model::draw(const vector::pen& pen)
{
    if (mIsLineLoop) {
        glColor4f(pen.r, pen.g, pen.b, pen.a);
        glLineWidth(pen.lineRadius * .3);

        glBegin(GL_LINE_STRIP);

        Point3d from = mVertexList[mEdgeList[0].from];
        mMatrix.TransformVertex(from, &from);
        glVertex3f(from.x, from.y, 0);

        for (std::size_t i = 1; i < mEdgeList.size(); i++) {
            Point3d to = mVertexList[mEdgeList[i].to];

            mMatrix.TransformVertex(to, &to);

            glVertex3f(to.x, to.y, 0);
        }

        glEnd();
    } else {
        glColor4f(pen.r, pen.g, pen.b, pen.a);
        glLineWidth(pen.lineRadius * .3);

        glBegin(GL_LINES);

        for (std::size_t i = 0; i < mEdgeList.size(); i++) {
            Point3d from = mVertexList[mEdgeList[i].from];
            Point3d to = mVertexList[mEdgeList[i].to];

            mMatrix.TransformVertex(from, &from);
            mMatrix.TransformVertex(to, &to);

            glVertex3f(from.x, from.y, 0);
            glVertex3f(to.x, to.y, 0);
        }

        glEnd();
    }
}

void model::emit(const vector::pen& pen)
{
    glColor4f(pen.r, pen.g, pen.b, pen.a);

    for (std::size_t i = 0; i < mEdgeList.size(); i++) {
        Point3d from = mVertexList[mEdgeList[i].from];
        Point3d to = mVertexList[mEdgeList[i].to];

        mMatrix.TransformVertex(from, &from);
        mMatrix.TransformVertex(to, &to);

        glVertex3f(from.x, from.y, 0);
        glVertex3f(to.x, to.y, 0);
    }
}
