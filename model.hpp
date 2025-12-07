#pragma once

#include "matrix.hpp"
#include "point3d.hpp"
#include "vector.hpp"

class model
{
  public:
    model();
    ~model();

    void draw(const vector::pen& pen);
    void emit(const vector::pen& pen);

    typedef struct
    {
        int from;
        int to;
    } Edge;

    void Identity() { mMatrix.Identity(); }
    void Scale(Point3d scale) { mMatrix.Scale(scale.x, scale.y, scale.z); }
    void Translate(Point3d trans) { mMatrix.Translate(trans.x, trans.y, trans.z); }
    void Rotate(float angle) { mMatrix.Rotate(0, 0, angle); }
    void Rotate(float x, float y, float z) { mMatrix.Rotate(x, y, z); }

    Point3d* mVertexList;
    Edge* mEdgeList;

    int mNumVertex;
    int mNumEdges;

    matrix mMatrix;

    bool mIsLineLoop;
};
