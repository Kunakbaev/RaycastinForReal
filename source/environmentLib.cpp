#include <assert.h>

#include "../include/environmentLib.hpp"

const sf::Keyboard::Key QUIT_SYMBOL = sf::Keyboard::Q;

void constructEnvironment(size_t width, size_t height, sf::RenderWindow* sceneWindow,
                          sf::RenderWindow* screenWindow, Environment* env) {
    assert(sceneWindow  != NULL);
    assert(screenWindow != NULL);

    //printf("%d\n", screenWindow.isOpen());
    Environment result = { width, height, sceneWindow, screenWindow };
    *env = result;
    //printf("%d\n", result.sceneWindow->isOpen());
}

bool isEnvOpen(const struct Environment* env) {
    assert(env               != NULL);
    assert(env->screenWindow != NULL);
    assert(env->sceneWindow  != NULL);

    return env->screenWindow->isOpen() &&
           env->sceneWindow->isOpen();
}

// KeyboardActions getKeyboardAction(int keyCode) {
//     if (keyCode) printf("code : %d\n", keyCode);
//     switch (keyCode) {
//         case sf::Keyboard::Up   :
//         case sf::Keyboard::W    :   return MOVE_PLAYER_FORWARD;
//         case sf::Keyboard::Down :
//         case sf::Keyboard::S    :   return MOVE_PLAYER_BACKWARD;
//         case sf::Keyboard::D    :
//         case sf::Keyboard::Right:   return TURN_PLAYER_RIGHT;
//         case sf::Keyboard::A    :
//         case sf::Keyboard::Left :   return TURN_PLAYER_LEFT;
//         case sf::Keyboard::Q    :   return QUIT_COMMAND;
//         default                 :   return NO_ACTION;
//     }
// }

// FIXME: cringe???
// #define BE sf::Keyboard::isKeyPressed(sf::Keyboard
// #define IS_PRESSED(x) BE ##x)

bool isForwardMove() {
    return sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    //return IS_PRESSED(W) || IS_PRESSED(Up);
}

bool isBackwardMove() {
return sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    //return IS_PRESSED(S) || IS_PRESSED(Down);
}

bool isTurnRight() {
    return sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
}

bool isTurnLeft() {
    return sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
}

bool isQuitPressed() {
    return sf::Keyboard::isKeyPressed(QUIT_SYMBOL);
}

static KeyboardActions windowEventsLoopHelper(sf::RenderWindow* window) {
    //assert(env    != NULL);
    assert(window != NULL);

    sf::Event event = {};
    KeyboardActions result = NO_ACTION;
    while (window->pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window->close();
//         if (event.type == sf::Event::KeyPressed) {
//             KeyboardActions action = getKeyboardAction(event.key.code);
//             if (action == MOVE_PLAYER_BACKWARD)
//                 env->movementState = MOVE_BACKWARD;
//
//         }
    }

    //if (sf::Keyboard::isKeyPressed(sf::Keyboard::W

    //if (result) printf("be : %d\n", result);
    return result;
}

KeyboardActions windowEventsLoops(const struct Environment* env) {
    KeyboardActions action = NO_ACTION;
    action = windowEventsLoopHelper(env->sceneWindow);
    if (action != NO_ACTION)
        return action;

    return windowEventsLoopHelper(env->screenWindow);
}

void drawCircle(Environment* env, sf::RenderWindow* window, const Point* point, float radius, sf::Color color) {
    assert(env          != NULL);
    assert(window       != NULL);
    assert(point        != NULL);
    assert(sign(radius) > 0);

    sf::CircleShape playerCircle(radius);
    playerCircle.setPosition(
        (float)point->x - radius,
        (float)(env->height - (float)point->y - 1 - radius)
    );
    playerCircle.setFillColor(color);
    window->draw(playerCircle);
}

void drawConvexShape(Environment* env, sf::RenderWindow* window, size_t numberOfPoints, const Point* const points, sf::Color color) {
    assert(env != NULL);
    assert(window != NULL);
    assert(numberOfPoints >= 3);
    assert(points != NULL);

    sf::ConvexShape poly;
    poly.setPointCount(numberOfPoints);
    poly.setFillColor(color);
    for (size_t pointIndex = 0; pointIndex < numberOfPoints; ++pointIndex) {
        sf::Vector2f vert(points[pointIndex].x,
                          env->height - points[pointIndex].y - 1);
        poly.setPoint(pointIndex, vert);
    }
    window->draw(poly);
}

void drawVertexArray(Environment* env, sf::RenderWindow* window, sf::VertexArray array) {
    assert(env != NULL);
    assert(window != NULL);
    // assert(array != NULL);

    // for (size_t pointIndex = 0; pointIndex < array.getVertexCount(); ++pointIndex) {
    //     int y = array[pointIndex].position.y;
    //     array[pointIndex].position.y = env->height - y - 1;
    // }
    window->draw(array);
}

void clearWindows(Environment* env) {
    assert(env               != NULL);
    assert(env->sceneWindow  != NULL);
    assert(env->screenWindow != NULL);

    env->sceneWindow->clear();
    env->screenWindow->clear();
}

void drawWindows(Environment* env) {
    assert(env               != NULL);
    assert(env->sceneWindow  != NULL);
    assert(env->screenWindow != NULL);

    env->sceneWindow->display();
    env->screenWindow->display();
}

void closeWindows(Environment* env) {
    assert(env               != NULL);
    assert(env->sceneWindow  != NULL);
    assert(env->screenWindow != NULL);

    env->sceneWindow->close();
    env->screenWindow->close();
}
