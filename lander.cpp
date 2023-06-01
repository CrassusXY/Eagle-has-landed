
#include "lander.h"

lander::lander()
{
    eagle = new QGraphicsPixmapItem(QPixmap(":/new/prefix1/pic/lander.png"));
    eagle->setScale(.1);
    eagle->setTransformOriginPoint(285, 267);
    velX = 0;
    velY = 0;
}

void lander::set_PixMap(const QString tmp)
{
    QPointF pos = eagle->pos();
    pos.setX(pos.x());
    pos.setY(pos.y());
    eagle = new QGraphicsPixmapItem(QPixmap(tmp));
    eagle->setPos(pos);
    eagle->setScale(.1);
    eagle->setTransformOriginPoint(285, 267);
}

QGraphicsPixmapItem* lander::get_eagle()
{
    return eagle;
}

void lander::set_rot(float _rot)
{
    rot = 0.8 * rot + 0.2 * _rot;
}


void lander::set_engine(bool _engine)
{
    engine = _engine;
}

void lander::tick()
{
    QPointF pos = eagle->pos();

    if(!engine) {
        //velX = velX
        velY += MOON_ACC * DT;
    }
    else {
        velX += (sin(rot * M_PI / 180) * ENGINE_ACC) * DT;
        velY -= ((cos(rot * M_PI / 180) * ENGINE_ACC ) - MOON_ACC ) * DT;
    }

    pos.setX(pos.x() + velX * DT);
    pos.setY(pos.y() + velY * DT);
    eagle->setPos(pos);
    eagle->setRotation(rot);
}
