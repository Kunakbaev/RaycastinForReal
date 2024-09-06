
#include <SFML/Graphics.hpp>

enum KeyboardActions {
    NO_ACTION,
    MOVE_PLAYER_FORWARD,
    MOVE_PLAYER_BACKWARD,
    TURN_PLAYER_RIGHT,
    TURN_PLAYER_LEFT,
    QUIT_COMMAND,
};

struct Environment {
    size_t width;
    size_t height;
    sf::RenderWindow* sceneWindow;
    sf::RenderWindow* screenWindow;
};

void constructEnvironment(size_t width, size_t height, const char* sceneName, const char* screenName, Environment* env);
bool isEnvOpen(const struct Environment* env);
KeyboardActions windowEventsLoops(const struct Environment* env);
void drawCircle(Environment* env, sf::RenderWindow* window, const Point* point, float radius, sf::Color color);
void drawConvexShape(Environment* env, sf::RenderWindow* window, size_t numberOfPoints, const Point* const points, sf::Color color);
void clearWindows(Environment* env);
void drawWindows(Environment* env);
void closeWindows(Environment* env);
