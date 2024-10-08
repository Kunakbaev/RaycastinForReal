#ifndef GEOMETRY_LIB
#define GEOMETRY_LIB

#include <math.h>

const long double EPS = 1e-6;
const long double INF = (long double)1e6;
const long double PIE = acosl((long double)-1.0);

// --------------------------------     HELPER FUNCTIONS        ------------------------------

long double sq(long double x);
int sign(long double x);


// Vector and Point is basicly the same thing
#define Vector Point

struct Point {
    long double x;
    long double y;
};

//void constructPoint(Point* point, long double x, long double y);

Point constructPoint(long double x, long double y);
Vector addVector(const Vector* v1, const Vector* v2);
Vector subVector(const Vector* v1, const Vector* v2);
long double scalarMult(const Point* one, const Point* two);
long double crossMult(const Point* one, const Point* two);
Vector vectorMultByConst(const Vector* vector, long double koef);
long double getVectorLen(const Vector* vector);
Vector normalizeVector(const Vector* vector);
long double getVectorAngle(const Vector* vector);

struct Segment {
    Point p1;
    Point p2;
};

//void constructSegment(Point* point, long double x, long double y);
Segment constructSegment(const Point* p1, const Point* p2);
bool doesSegmentsIntersect(const Segment* segm1, const Segment* segm2);

Vector rotateVectorByAngle(const Vector* vector, long double angle);
bool doesRayIntersectSegm(const Point* origin, const Vector* vector, const Segment* segment);
long double distanceToSegmByDirection(const Point* origin, const Vector* direction, const Segment* segment);
long double getDistanceFromPointToSegm(const Point* point, const Segment* segm);
bool isInsideAngle(const Point* point, const Point* origin, const Point* p1, const Point* p2);
bool isInsideTriangle(const Point* point, const Point* p1, const Point* p2, const Point* p3);


#endif
