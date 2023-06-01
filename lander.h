
#ifndef LANDER_H
#define LANDER_H
#include <QGraphicsPixmapItem>
#include <QTransform>

#define SCALER 10
#define MOON_ACC SCALER*2.62
#define ENGINE_ACC SCALER*4.5
#define DT 0.01

class lander
{
public:
    lander();
    void set_PixMap(const QString tmp);
    void tick();
    void render();
    QGraphicsPixmapItem *get_eagle();
    void set_rot(float _rot);
    void set_engine(bool _engine);
    bool get_engine();
    float get_velX(){return velX;};
    float get_velY(){return velY;};
    int get_fuel();
    bool get_landed(){return landed;};
private:
    QGraphicsPixmapItem *eagle;
    int count;
    int fuel;
    float velX, velY, rot;
    bool engine;
    bool landed;
};

#endif // LANDER_H
