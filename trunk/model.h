#ifndef MODEL_H
#define MODEL_H

#include "point3d.h"
#include "vector.h"
#include "matrix.h"

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
    }Edge;

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

    BOOL mIsLineLoop;

};

#endif // MODEL_H
