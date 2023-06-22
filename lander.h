
#ifndef LANDER_H
#define LANDER_H
#include <QGraphicsPixmapItem>
#include <QTransform>

#define SCALER 20
#define MOON_ACC SCALER*2.62
#define ENGINE_ACC SCALER*4.5
#define DT 0.01

class lander
{
public:
    int count = 0;
    lander();
    void set_PixMap(const QString tmp);
    void tick();
    void render();
    QGraphicsPixmapItem *get_eagle();
    void set_rot(float _rot);
    void set_engine(bool _engine);
    bool get_engine();
    /**
 * @brief Updates the score based on the current level and fuel.
 *
 * This function calculates the score by multiplying the level with the remaining fuel and adds it to the current score.
 */
    void set_score() { score += level * fuel; }

    /**
 * @brief Returns the horizontal velocity of the lander.
 * @return The horizontal velocity.
 */
    float get_velX() { return velX; }

    /**
 * @brief Returns the vertical velocity of the lander.
 * @return The vertical velocity.
 */
    float get_velY() { return velY; }

    /**
 * @brief Returns the current score.
 * @return The score.
 */
    int get_score() { return score; }

    /**
 * @brief Checks if the lander has landed.
 * @return True if the lander has landed, False otherwise.
 */
    bool get_landed() { return landed; }
    int get_fuel();
    void reset_state();
private:
    float level;
    int score;
    QGraphicsPixmapItem *eagle;
    int fuel;
    float velX, velY, rot;
    bool engine;
    bool landed;
};

#endif // LANDER_H
