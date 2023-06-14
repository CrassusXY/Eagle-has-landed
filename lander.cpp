
#include "lander.h"

lander::lander()
{
    eagle = new QGraphicsPixmapItem(QPixmap(":/new/prefix1/pic/lander.png"));
    eagle->setScale(.08);
    eagle->setTransformOriginPoint(410, 410);
    velX = 80;
    velY = 80;
    fuel = 1000;
    landed = false;
}

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

QGraphicsPixmapItem* lander::get_eagle()
{
    return eagle;
}

void lander::set_rot(float _rot)
{
    rot =  _rot;
}

void lander::set_engine(bool _engine)
{
    engine = _engine;
}

bool lander::get_engine()
{
    return engine;
}

int lander::get_fuel()
{
    return fuel;
}

void lander::tick()
{
    QPointF pos = eagle->pos();

    if(landed){
        set_PixMap(":/new/prefix1/pic/lander.png");
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

    if( abs(pos.y() - 480) < 1)
        landed = true;
}
