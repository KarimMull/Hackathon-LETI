#include "entities.h"

GunScope::GunScope() : _x(MAP_WIDTH), _y(MAP_HEIGHT / 2), _ammo(MAX_AMMO){}
Chicken::Chicken(int x, int y) : _x(x), _y(y){}
Bullet::Bullet(int x, int y) : _x(x), _y(x){}