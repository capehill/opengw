#include "mathutils.h"
#include "sincos.h"


namespace mathutils
{
    float wrapRadians(float radians)
    {
        radians = fmodf(radians, RADIAN);
        if (radians < 0.0) radians += RADIAN;
        return radians;
    }

    float calculate2dDistance(Point3d p1, Point3d p2)
    {
        float x1 = p1.x;
        float y1 = p1.y;
        float x2 = p2.x;
        float y2 = p2.y;

        float dy = x1 - x2;
        float dz = y1 - y2;
        float distance = sqrt((dz*dz) + (dy*dy));

        return distance;
    }

    float calculate2dAngle(Point3d from, Point3d to)
    {
        float angle = 0.0;

        float x1 = from.x;
        float y1 = from.y;
        float x2 = to.x;
        float y2 = to.y;

        float dx = x2 - x1;
        float dy = y2 - y1;

        if (dx == 0)
        {
            if (dy == 0)
                angle = 0;
            else if (dy > 0)
                angle = PI/2;
            else
                angle = PI * 3.0 / 2.0;
        }
        else if (dy == 0)
        {
            if (dx > 0)
                angle = 0;
            else
                angle = PI;
        }
        else
        {
            if (dx < 0)
                angle = atan(dy/dx) + PI;
            else if (dy < 0)
                angle = atan(dy/dx) + (2*PI);
            else
                angle = atan(dy/dx);
        }

        if (angle >= RADIAN) angle-=RADIAN;

        return angle;
    }

    float diffAngles(float angle1, float angle2)
    {
      angle1 = mathutils::wrapRadians(angle1);
      angle2 = mathutils::wrapRadians(angle2);

      float angle = angle1-angle2;

      if (angle >= mathutils::DegreesToRads(180))
        angle -= mathutils::DegreesToRads(360);
      else if (angle <= -mathutils::DegreesToRads(180))
        angle += mathutils::DegreesToRads(360);

      return angle;
    }

    Point3d clamp2dVector(const Point3d& vector, float max)
    {
        Point3d clamped = vector;
        float distance = mathutils::calculate2dDistance(Point3d(0,0,0), clamped);
        if (distance > max)
        {
            float r = max / distance;
            clamped.x *= r;
            clamped.y *= r;
        }
        return clamped;
    }

    Point3d rotate2dPoint(const Point3d& point, float angle)
    {
        float normalx = point.x;
        float normaly = point.y;
        Point3d pt(normalx * get_cos(angle) - normaly * get_sin(angle), normalx * get_sin(angle) + normaly * get_cos(angle));
        return pt;
    }

    Point3d translate2dPoint(const Point3d& point, float tx, float ty)
    {
        Point3d pt(point.x + tx, point.y + ty);
        return pt;
    }

    Point3d translate2dPoint(const Point3d& point, Point3d translation)
    {
        Point3d pt(point.x + translation.x, point.y + translation.y);
        return pt;
    }

    Point3d translate2dPointFromPointByRadius(const Point3d& point, const Point3d& fromPoint, float distance)
    {
        float angle = mathutils::calculate2dAngle(point, fromPoint);

        Point3d vPoint(distance,0,0);
        Point3d rPoint = mathutils::rotate2dPoint(vPoint, angle);

        return rPoint;
    }

    int randFromTo(int from, int to)
    {
        int range = to-from;
        return (int) ((frandFrom0To1()*(range+1))+from);
    }

    float frandFrom0To1()
    {
        return (float)rand()/RAND_MAX;
    }

	//  Globals which should be set before calling this function:
	//
	//  int    polySides  =  how many corners the polygon has
	//  float  polyX[]    =  horizontal coordinates of corners
	//  float  polyY[]    =  vertical coordinates of corners
	//  float  x, y       =  point to be tested
	//
	//  (Globals are used in this example for purposes of speed.  Change as
	//  desired.)
	//
	//  The function will return YES if the point x,y is inside the polygon, or
	//  NO if it is not.  If the point is exactly on the edge of the polygon,
	//  then the function may return YES or NO.
	//
	//  Note that division by zero is avoided because the division is protected
	//  by the "if" clause which surrounds it.
	//
	// From http://alienryderflex.com/polygon/

	bool pointInPolygon(const Point3d& testPoint, Point3d poly[], int numPoints, float scale)
	{
		bool oddNodes=FALSE;

		Point3d from(poly[0] * scale);
	    for (int i=1; i<numPoints; i++)
	    {
            Point3d to(poly[i] * scale);

            if (to.z != 0)
            {
                i++;
    		    from = (poly[i] * scale);
                continue;
            }

			if ((from.y < testPoint.y && to.y >= testPoint.y || to.y < testPoint.y && from.y >= testPoint.y) && (from.x <= testPoint.x || to.x <= testPoint.x))
			{
				oddNodes ^= (from.x + (testPoint.y-from.y) / (to.y - from.y) * (to.x - from.x) < testPoint.x);
			}

            from = to;
        }

		return oddNodes;

	}

    Point3d closestPointOnLineSegment(const Point3d& lineFrom, const Point3d& lineTo, const Point3d& testPoint)
    {
        Point3d lineDiffVect = lineTo - lineFrom;
        float length = calculate2dDistance(Point3d(0,0,0), lineDiffVect);
 
        const Point3d lineToPointVect = testPoint - lineFrom;
        const float dotProduct = Point3d::dot(lineDiffVect, lineToPointVect);
        const float percAlongLine = dotProduct / (length*length);
 
        // If point is outside of the line segment, clamp it to one end or another
        if (percAlongLine < 0.0f)
        {
            return lineFrom;
        }
        else if (percAlongLine > 1.0f)
        {
            return lineTo;
        }

        return (lineFrom + (Point3d(lineDiffVect.x * percAlongLine, lineDiffVect.y * percAlongLine, 0)));
    }

    float pointLineDistance(const Point3d& lineFrom, const Point3d& lineTo, const Point3d& testPoint)
    {
        Point3d lineDiffVect = lineTo - lineFrom;
        float length = calculate2dDistance(Point3d(0,0,0), lineDiffVect);
 
        const Point3d lineToPointVect = testPoint - lineFrom;
        const float dotProduct = Point3d::dot(lineDiffVect, lineToPointVect);
        const float percAlongLine = dotProduct / (length*length);
 
        // If point is outside of the line segment, clamp it to one end or another
        if (percAlongLine < 0.0f)
        {
            return fabs(calculate2dDistance(lineFrom, testPoint));
        }
        else if (percAlongLine > 1.0f)
        {
            return fabs(calculate2dDistance(lineTo, testPoint));
        }

        Point3d intersectPoint = lineFrom + (Point3d(lineDiffVect.x * percAlongLine, lineDiffVect.y * percAlongLine, 0));
        return fabs(calculate2dDistance(intersectPoint, testPoint));
    }

    bool isPointOnLine(const Point3d& lineFrom, const Point3d& lineTo, const Point3d& testPoint)
    {
        float distance = pointLineDistance(lineFrom, lineTo, testPoint);
        return distance < 1 ? true : false;
    }

    bool approximatelyEqual(float testValue, float eval, float tolerance)
    {
        return ((eval > (testValue - tolerance)) && (eval < (testValue + tolerance)));
    }
};


