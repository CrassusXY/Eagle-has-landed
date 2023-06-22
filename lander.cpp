
#include "lander.h"

/**
 * @brief Constructor for the Lander class.
 *
 * This constructor initializes the Lander object by creating a QGraphicsPixmapItem representing the lander sprite
 * and setting its initial properties such as scale, transform origin point, score, velocity in the X and Y directions,
 * fuel level, landing status, and level.
 */
lander::lander()
{
    eagle = new QGraphicsPixmapItem(QPixmap(":/new/prefix1/pic/lander.png"));
    eagle->setScale(.08);
    eagle->setTransformOriginPoint(410, 410);
    score = 0;
    velX = 80;
    velY = 80;
    fuel = 1000;
    landed = false;
    level = 1;
}

/**
 * @brief Resets the state of the lander.
 *
 * This function resets the state of the lander by updating its properties. It increases the level by 0.5, creates a new
 * QGraphicsPixmapItem representing the lander sprite with the initial properties, updates the velocity in the X and Y
 * directions based on the level, resets the fuel level, and sets the landing status to false.
 */
void lander::reset_state()
{
    level += 0.5;
    eagle = new QGraphicsPixmapItem(QPixmap(":/new/prefix1/pic/lander.png"));
    eagle->setScale(.08);
    eagle->setTransformOriginPoint(410, 410);
    velX = level*80;
    velY = level*80;
    fuel = 1000;
    landed = false;
}

/**
 * @brief Sets the pixmap of the lander.
 *
 * This function sets the pixmap of the lander by creating a new QGraphicsPixmapItem with the specified pixmap
 * and updating its properties such as scale, transform origin point, position, and rotation. If the lander has already
 * landed, the function returns early without making any changes.
 *
 * @param tmp The path to the new pixmap.
 */
void lander::set_PixMap(const QString tmp)
{
    if(landed)
        return;

    QPointF pos = eagle->pos();
    int r = eagle->rotation();
    eagle = new QGraphicsPixmapItem(QPixmap(tmp));
    eagle->setScale(.08);
    eagle->setTransformOriginPoint(410, 410);
    eagle->setPos(pos);
    eagle->setRotation(r);
}

/**
 * @brief Returns a pointer to the lander's QGraphicsPixmapItem.
 *
 * This function returns a pointer to the QGraphicsPixmapItem representing the lander's sprite.
 *
 * @return A pointer to the lander's QGraphicsPixmapItem.
 */
QGraphicsPixmapItem* lander::get_eagle()
{
    return eagle;
}

/**
 * @brief Sets the rotation of the lander.
 *
 * This function sets the rotation of the lander to the specified rotation value.
 *
 * @param _rot The rotation value to set.
 */
void lander::set_rot(float _rot)
{
    rot = _rot;
}

/**
 * @brief Sets the engine status of the lander.
 *
 * This function sets the engine status of the lander to the specified value.
 *
 * @param _engine The engine status to set.
 */
void lander::set_engine(bool _engine)
{
    engine = _engine;
}

/**
 * @brief Returns the engine status of the lander.
 *
 * This function returns the current engine status of the lander.
 *
 * @return The engine status of the lander.
 */
bool lander::get_engine()
{
    return engine;
}

/**
 * @brief Returns the fuel level of the lander.
 *
 * This function returns the current fuel level of the lander.
 *
 * @return The fuel level of the lander.
 */
int lander::get_fuel()
{
    return fuel;
}

/**
 * @brief Updates the state of the lander.
 *
 * This function updates the state of the lander based on its current position, velocity, rotation, engine status, and fuel level.
 * It adjusts the velocity in the X and Y directions based on the engine status and applies gravity when the engine is off or fuel is depleted.
 * The lander's position is updated accordingly, and if it lands successfully, the score is set, the landed status is updated,
 * and the lander's pixmap is set to the default lander image.
 */
void lander::tick()
{
    QPointF pos = eagle->pos();

    if(landed){
        return;
    }

    if(!engine || fuel == 0) {
        velY += MOON_ACC * DT;
    }
    else {
        velX += (sin(rot * M_PI / 180) * ENGINE_ACC) * DT;
        velY -= ((cos(rot * M_PI / 180) * ENGINE_ACC ) - MOON_ACC ) * DT;
        fuel -= 0.01;
    }

    pos.setX(pos.x() + velX * DT);
    pos.setY(pos.y() + velY * DT);
    eagle->setPos(pos);
    eagle->setRotation(rot);

    if(abs(pos.y() - 480) < 1) {
        set_score();
        landed = true;
        set_PixMap(":/new/prefix1/pic/lander.png");
    }
}
