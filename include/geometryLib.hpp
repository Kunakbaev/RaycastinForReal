#ifndef INCLUDE_GEOMETRY_LIB_HPP
#define INCLUDE_GEOMETRY_LIB_HPP

#include <math.h>

const long double EPS = (long double)1e-6;
const long double INF = (long double)1e6;
const long double PIE = acosl((long double)-1.0);

// --------------------------------     HELPER FUNCTIONS        ------------------------------

long double sq(long double x);
int sign(long double x);


// Vector and Point are basically the same thing,
// but to understand better with what we are dealing right now
// we will call them differently
#define Vector Point

struct Point {
    long double x;
    long double y;
};

Point constructPoint(long double x, long double y);
Vector addVector(const Vector* v1, const Vector* v2);
Vector subVector(const Vector* v1, const Vector* v2);
long double scalarMult(const Point* one, const Point* two);
long double crossMult(const Point* one, const Point* two);
Vector vectorMultByConst(const Vector* vector, long double coef);
long double getVectorLen(const Vector* vector);
Vector normalizeVector(const Vector* vector);
long double getVectorAngle(const Vector* vector);

struct Segment {
    Point p1;
    Point p2;
};

Segment constructSegment(const Point* p1, const Point* p2);
bool areSegmentsIntersecting(const Segment* segm1, const Segment* segm2);

Vector rotateVectorByAngle(const Vector* vector, long double angle);
bool doesRayIntersectSegm(const Point* origin, const Vector* vector, const Segment* segment);
long double distanceToSegmByDirection(const Point* origin, const Vector* direction, const Segment* segment);
long double getDistanceFromPointToSegm(const Point* point, const Segment* segm);
bool isInsideAngle(const Point* point, const Point* origin, const Point* p1, const Point* p2);
bool isInsideTriangle(const Point* point, const Point* p1, const Point* p2, const Point* p3);


#endif
