#include <assert.h>
#include <stdlib.h>

#include "../include/scene.hpp"

#define FREE(x)             \
    do {                    \
        free(x);            \
        x = NULL;           \
    } while(0)

const size_t PAIRS_ARRAY_BUFFER_SIZE = 300;
const long double ROTATION_EPS                 = EPS * 10;
const int MAGIC_COLUMN_HEIGHT_SCALE_FACTOR     = 80;
const int MAGIC_COLUMN_BRIGHTNESS_SCALE_FACTOR = 50;
const int MAX_BRIGHTNESS_VALUE                 = 255; // UINT8_MAX

// ----------------------------   CONSTRUCTING SCENE and checking if all objects are valid    --------------

// obstacles should not intersect
static bool validateObstacles(size_t numberOfObstacles, const Obstacle* obstacles) {
    assert(obstacles != NULL);

    for (size_t obstacleIndex = 0; obstacleIndex < numberOfObstacles; ++obstacleIndex)
        for (size_t obstacleIndex2 = obstacleIndex + 1; obstacleIndex2 < numberOfObstacles; ++obstacleIndex2)
            if (doesObstaclesIntersect(&obstacles[obstacleIndex], &obstacles[obstacleIndex2]))
                return false;
    return true;
}

Scene constructScene(int height, int width, const Player* player, size_t numberOfObstacles, Obstacle* obstacles) {
    assert(height > 0);
    assert(width > 0);
    assert(player != NULL);
    assert(obstacles != NULL);

    // FIXME: PUT TO DEBUG
    if (!validateObstacles(numberOfObstacles, obstacles)) {
        fprintf(stderr, "obstacles are not valid\n");
        assert(false);
    }

    Scene scene = {height, width, *player, numberOfObstacles, obstacles};

    const Point sides[] = {
        constructPoint(0,     0),
        constructPoint(width, 0),
        constructPoint(width, height),
        constructPoint(0,     height),
    };
    Obstacle bounds = constructObstacle(4, sides);

    scene.numberOfObstacles = numberOfObstacles + 1;
    scene.obstacles = (Obstacle*)calloc(numberOfObstacles + 1, sizeof(Obstacle));
    for (size_t obstacleInd = 0; obstacleInd <= numberOfObstacles; ++obstacleInd) {
        if (obstacleInd != numberOfObstacles)
            scene.obstacles[obstacleInd] = obstacles[obstacleInd];
        else
            scene.obstacles[obstacleInd] = bounds;
    }

    if (!isPlayerPositionGood(&scene)) {
        fprintf(stderr, "player position is not valid\n");
        assert(false);
    }

    scene.pairsArraySize = 0;
    scene.pairsArray = (Pair*)calloc(PAIRS_ARRAY_BUFFER_SIZE, sizeof(Pair));
    assert(scene.pairsArray != NULL);

    return scene;
}

bool isPlayerPositionGood(const Scene* scene) {
    size_t arrLen = scene->numberOfObstacles;
    for (size_t obstacleIndex = 0; obstacleIndex < arrLen; ++obstacleIndex) {
        bool isInter = doesObstacleIntersectWithPlayer(
            &scene->obstacles[obstacleIndex],
            &scene->player
        );

        if ((obstacleIndex != arrLen - 1 && isInter) ||
            (obstacleIndex == arrLen - 1 && !isInter))
                return false;
    }

    long double minimalDistToWall = scene->player.bodyRadius * 1;
    for (size_t obstacleIndex = 0; obstacleIndex < arrLen; ++obstacleIndex) {
        Obstacle obj = scene->obstacles[obstacleIndex];
        for (size_t sideIndex = 0; sideIndex < obj.numberOfSides; ++sideIndex) {
            Segment segm = getSegment(&obj, sideIndex);
            long double dist = getDistanceFromPointToSegm(&scene->player.position, &segm);
            //printf("%d %d\n", obstacleIndex, sideIndex);
            //printf("segm : %Lg, %Lg     %Lg, %Lg    dist : %Lg\n", segm.p1.x, segm.p1.y, segm.p2.x, segm.p2.y, dist);
            if (sign(dist - minimalDistToWall) < 0)
                return false;
        }
    }
    //exit(0);

    return true;
}




// ---------------------------------    DISPLAYING STUFF    ---------------------------------------------

static void displayPlayer(const Scene* scene, Environment* env, int screenHeight) {
    assert(scene             != NULL);
    assert(env               != NULL);
    assert(env->sceneWindow  != NULL);
    assert(env->screenWindow != NULL);

    drawCircle(env, env->sceneWindow, &scene->player.position, (float)scene->player.bodyRadius, sf::Color::White);
    for (size_t rayInd = 1; rayInd < scene->pairsArraySize; rayInd += 2) {
        Pair p1 = scene->pairsArray[rayInd - 1];
        Pair p2 = scene->pairsArray[rayInd];

        Vector direction = constructPoint(1, 0);
        direction = rotateVectorByAngle(&direction, p1.first);
        Point one = vectorMultByConst(&direction, p1.second);
        one = addVector(&scene->player.position, &one);

        direction = constructPoint(1, 0);
        direction = rotateVectorByAngle(&direction, p2.first);
        Point two = vectorMultByConst(&direction, p2.second);
        two = addVector(&scene->player.position, &two);

        // FIXME: cringe?
        const Point points[] = {
            scene->player.position, one, two
        };
        drawConvexShape(env, env->sceneWindow, 3, points, sf::Color::Magenta);
    }
}

static void displayObstacles(size_t numberOfObstacles, const Obstacle* obstacles, Environment* env) {
    // last obstacle is bounding rect, so we don't want to display it
    for (size_t obstacleInd = 0; obstacleInd < numberOfObstacles - 1; ++obstacleInd)
        displayObstacle(&obstacles[obstacleInd], env);
}

void displayScene(Scene* scene, Environment* env) {
    assert(scene != NULL);
    assert(env   != NULL);

    displayPlayer(scene, env, scene->height);
    displayObstacles(scene->numberOfObstacles, scene->obstacles, env);
}




// --------------------------------     FINDING DISTANCES TO WALLS      ------------------------------

static int compare(const void* one, const void* two) {
    assert(one != NULL);
    assert(two != NULL);

    const struct Pair* p1 = (const Pair*)one;
    const struct Pair* p2 = (const Pair*)two;

    int si = sign(p1->first - p2->first);
    if (si != 0)
        return -si;
    return sign(p1->second - p2->second);
}

static long double findMinDistForDirection(const Vector* direction, const Scene* scene) {
    assert(direction != NULL);
    assert(scene     != NULL);

    long double bestDist = INF;
    Point origin = scene->player.position;
    for (size_t obstInd = 0; obstInd < scene->numberOfObstacles; ++obstInd) {
        Obstacle obj = scene->obstacles[obstInd];
        for (size_t sideInd = 0; sideInd < obj.numberOfSides; ++sideInd) {
            Segment segm = getSegment(&obj, sideInd);
            long double dist = distanceToSegmByDirection(&origin, direction, &segm);
            if (sign(dist - bestDist) < 0) {
                bestDist = dist;
            }
        }
    }

    return bestDist;
}

static void addMostLeftAndRightRays(Scene* scene) {
    assert(scene             != NULL);
    assert(scene->pairsArray != NULL);

    Vector leftRay = constructPoint(1, 0);
    leftRay = rotateVectorByAngle(&leftRay, scene->player.currentDirection + scene->player.FOV / 2);
    Vector rightRay = rotateVectorByAngle(&leftRay, -scene->player.FOV);
    long double leftDist = findMinDistForDirection(&leftRay, scene);
    long double rightDist = findMinDistForDirection(&rightRay, scene);

    scene->pairsArray[scene->pairsArraySize++] = {getMaxFOVangle(&scene->player), leftDist};
    scene->pairsArray[scene->pairsArraySize++] = {getMinFOVangle(&scene->player), rightDist};
}

static void findDistancesToWalls(Scene* scene) {
    assert(scene != NULL);

    scene->pairsArraySize = 0;
    Point origin = scene->player.position;
    for (size_t objInd = 0; objInd < scene->numberOfObstacles; ++objInd) {
        Obstacle obj = scene->obstacles[objInd];
        for (size_t sideInd = 0; sideInd < obj.numberOfSides; ++sideInd) {
            Point point = obj.sides[sideInd];
            Segment segm = getSegment(&obj, sideInd);

            Vector direction = subVector(&point, &origin);
            long double angle = NAN;
            bool isOk = checkIfDirectionInsideFOV(&scene->player, direction, &angle);
            if (!isOk) continue;

            long double dist = distanceToSegmByDirection(&origin, &direction, &segm);
            long double bestDist = findMinDistForDirection(&direction, scene);

            if (sign(bestDist - dist) < 0)
                continue;

            direction = rotateVectorByAngle(&direction, ROTATION_EPS);
            for (int isToRight = 0; isToRight <= 1; ++isToRight) {
                if (sign(getVectorLen(&direction)) == 0)
                    continue;

                dist = findMinDistForDirection(&direction, scene);
                Pair pair = {angle + (isToRight ? -ROTATION_EPS : ROTATION_EPS), dist};
                assert(scene->pairsArraySize < PAIRS_ARRAY_BUFFER_SIZE);
                scene->pairsArray[scene->pairsArraySize++] = pair;

                direction = rotateVectorByAngle(&direction, -2 * ROTATION_EPS);
            }
        }
    }

    addMostLeftAndRightRays(scene);
    qsort(scene->pairsArray, scene->pairsArraySize, sizeof(Pair), compare);
}

static int getColumnColorByDistance(long double dist) {
    assert(sign(dist) >= 0);

    long double koef = sign(dist) ? 1 / dist : 1;
    koef *= MAGIC_COLUMN_BRIGHTNESS_SCALE_FACTOR;
    if (sign(koef - 1) > 0)
        koef = 1.0;

    return (int)(MAX_BRIGHTNESS_VALUE * koef);
}

static int getColumnHeightByDistance(const Scene* scene, long double dist) {
    assert(scene      != NULL);
    assert(sign(dist) >= 0);

    long double koef = sign(dist) ? 1 / dist : 1;
    koef *= MAGIC_COLUMN_HEIGHT_SCALE_FACTOR;
    int height = (int)(scene->height * koef);

    if (height < 1) height = 1;
    return height;
}


// alternative codestyle
static void
drawTrapezoid(int start,
              int end,
              int startHeight,
              int endHeight,
              int screenHeight,
              Environment* env,
              int colorStart,
              int colorEnd) {
    assert(screenHeight > 0);
    assert(env          != NULL);

    sf::VertexArray vertexArray(sf::TriangleFan);
    int mid = screenHeight / 2;
    for (int i = 0; i < 4; ++i) {
        bool isLeft = i == 0 || i == 3;
        int height = (isLeft ? startHeight : endHeight) / 2;
        sf::Uint8 colorValue = (sf::Uint8)(isLeft ? colorStart : colorEnd);
        sf::Color color(colorValue, colorValue, colorValue);
        if (i <= 1) height *= -1;

        sf::Vertex vert(sf::Vector2f(isLeft ? start : end, mid + height));
        vert.color = color;
        vertexArray.append(vert);
    }

    drawVertexArray(env, env->screenWindow, vertexArray);
}

void displayScreen(Scene* scene, Environment* env) {
    assert(scene != NULL);
    assert(env   != NULL);

    int previousEnd = 0;
    findDistancesToWalls(scene);
    for (size_t rayInd = 1; rayInd < scene->pairsArraySize; rayInd += 2) {
        Pair p1 = scene->pairsArray[rayInd - 1];
        Pair p2 = scene->pairsArray[rayInd];

        long double koef1 = (p1.first - p2.first) / scene->player.FOV;
        int width         = (int)roundl(scene->width * koef1);
        int end           = previousEnd + width;
        int startHeight   = getColumnHeightByDistance(scene, p1.second);
        int endHeight     = getColumnHeightByDistance(scene, p2.second);
        int colorStart    = getColumnColorByDistance(p1.second);
        int colorEnd      = getColumnColorByDistance(p2.second);

        drawTrapezoid(previousEnd, end, startHeight, endHeight, scene->height,
                      env, colorStart, colorEnd);
        previousEnd = end;
    }
}

void destructScene(Scene* scene) {
    assert(scene != NULL);

    for (size_t obstacleInd = 0; obstacleInd < scene->numberOfObstacles; ++obstacleInd)
        destructObstacle(&scene->obstacles[obstacleInd]);
    FREE(scene->obstacles);
    scene->numberOfObstacles = 0;

    FREE(scene->pairsArray);
    scene->pairsArraySize = 0;
}
