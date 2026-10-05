#ifndef ENTITIES
#define ENTITIES

#define MAP_WIDTH 50
#define MAP_HEIGHT 25

#define MAX_AMMO 5
#define MAX_ENEMIES 10

class GunScope {
  private:
    int _x, _y;
    int _ammo;
  public:
    GunScope();
};

class Chicken {
  private:
    int _x, _y;
  public:
    Chicken(int x, int y);
};

extern Chicken* chickens[MAX_ENEMIES];

class Bullet {
  private:
    int _x, _y;
    int _dmg;
  public:
    Bullet(int x, int y);
};

#endif