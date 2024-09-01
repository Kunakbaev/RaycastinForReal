#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/geometryLib.hpp"

const long double EPS = 1e-6;
const long double INF = 1e18;

// --------------------------------     HELPER FUNCTIONS        ------------------------------

long double sq(long double x) {
    return x * x;
}

int sign(long double x) {
    if (x < -EPS) return -1;
    return x > EPS;
}

//void constructPoint(Point* point, long double x, long double y);

Point constructPoint(long double x, long double y) {
    Point result = {x, y};
    return result;
}

Vector addVector(const Vector* v1, const Vector* v2) {
    Vector result = {
        v1->x + v2->x,
        v1->y + v2->y,
    };

    return result;
}

Vector subVector(const Vector* v1, const Vector* v2) {
    Vector result = {
        v1->x - v2->x,
        v1->y - v2->y,
    };

    return result;
}

Vector vectorMultByConst(const Vector* vector, long double koef) {
    Vector result = {
        vector->x * koef,
        vector->y * koef
    };

    return result;
}

long double getVectorLen(const Vector* vector) {
    return sqrtl(sq(vector->x) + sq(vector->y));
}

Vector normalizeVector(const Vector* vector) {
    assert(vector != NULL);

    long double len = getVectorLen(vector);
    if (sign(len) == 0) {
        fprintf(stderr, "Error: couldn't normalize vector, it's length is 0");
        assert(false);
    }

    Vector norm = constructPoint(
        vector->x / len,
        vector->y / len
    );
    return norm;
}

long double getVectorAngle(const Vector* vector) {
    return atan2(vector->y, vector->x);
}

//void constructSegment(Point* point, long double x, long double y);
Segment constructSegment(const Point* p1, const Point* p2) {
    assert(p1 != NULL);
    assert(p2 != NULL);

    Segment result = {*p1, *p2};
    return result;
}


long double scalarMult(const Point* p1, const Point* p2) {
    assert(p1 != NULL);
    assert(p2 != NULL);

    return p1->x * p2->x + p1->y * p2->y;
}

long double crossMult(const Point* p1, const Point* p2) {
    assert(p1 != NULL);
    assert(p2 != NULL);

    return p1->x * p2->y - p1->y * p2->x;
}

Vector rotateVectorByAngle(const Vector* vector, long double angle) {
    assert(vector != NULL);

    long double cosAngle = cosl(angle);
    long double sinAngle = sinl(angle);

    Vector result = {
        vector->x * cosAngle - vector->y * sinAngle,
        vector->x * sinAngle + vector->y * cosAngle
    };

    return result;
}

bool doesRayIntersectSegm(const Point* origin, const Vector* vector, const Segment* segment) {
    assert(origin  != NULL);
    assert(vector  != NULL);
    assert(segment != NULL);

    Vector origin2p1 = subVector(&segment->p1, origin);
    Vector origin2p2 = subVector(&segment->p2, origin);

    long double cross1 = crossMult(&origin2p1, vector);
    long double cross2 = crossMult(&origin2p2, vector);

    int si = sign(cross1) * sign(cross2);
    return si <= 0;
}

long double distanceToSegmByDirection(const Point* origin, const Vector* direction, const Segment* segment) {
    assert(origin    != NULL);
    assert(direction != NULL);
    assert(segment   != NULL);

    if (!doesRayIntersectSegm(origin, direction, segment))
        return INF;

    // FIXME: how to do this properly??? by binary search, not very optimal ?

    Vector norm = normalizeVector(direction);
    long double l = 0.0, r = INF;
    while (sign(r - l) > 0) {
        long double mid = (l + r) / 2;
        Vector vector = vectorMultByConst(&norm, mid);
        Point segmEnd = addVector(origin, &vector);
        Segment raySegment = constructSegment(origin, &segmEnd);

        if (doesSegmentsIntersect(segment, &raySegment))
            r = mid;
        else
            l = mid;
    }

    return r;
}

static bool doesSegmentsIntersectHelper(const Segment* segm1, const Segment* segm2) {
    assert(segm1 != NULL);
    assert(segm2 != NULL);

    Vector tmp = subVector(&segm2->p1, &segm1->p1);
    Vector direction = subVector(&segm1->p2, &segm1->p1);
    long double cross1 = crossMult(&tmp, &direction);

    tmp = subVector(&segm2->p2, &segm1->p1);
    long double cross2 = crossMult(&tmp, &direction);

    return sign(cross1) * sign(cross2) <= 0;
}

bool doesSegmentsIntersect(const Segment* segm1, const Segment* segm2) {
    assert(segm1 != NULL);
    assert(segm2 != NULL);

    bool ok1 = doesSegmentsIntersectHelper(segm1, segm2);
    bool ok2 = doesSegmentsIntersectHelper(segm2, segm1);
    return ok1 && ok2;
}

bool isInsideAngle(const Point* point, const Point* origin, const Point* p1, const Point* p2) {
    Vector direction = subVector(point, origin);
    Vector origin2p1 = subVector(p1,    origin);
    Vector origin2p2 = subVector(p2,    origin);

    long double one = crossMult(&origin2p1, &direction);
    long double two = crossMult(&origin2p2, &direction);
    return sign(one) * sign(two) <= 0;
}

bool isInsideTriangle(const Point* point, const Point* p1, const Point* p2, const Point* p3) {
    return isInsideAngle(point, p1, p2, p3) &&
           isInsideAngle(point, p2, p1, p3) &&
           isInsideAngle(point, p3, p1, p2);
}
