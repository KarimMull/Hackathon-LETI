#include "entities.h"

Player::Player() : _x(MAP_WIDTH), _y(MAP_HEIGHT / 2), _health(PLAYER_HP){}
Chicken::Chicken(int x, int y, ChickenMove moveType) : _x(x), _y(y), _moveType(moveType), _dmg(CHICKEN_DMG), _health(CHICKEN_HP){}
Bullet::Bullet(int x, int y) : _x(x), _y(x), _dmg(BULLET_DMG){}