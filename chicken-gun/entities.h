#define MAP_WIDTH 200
#define MAP_HEIGHT 100

#define MAX_AMMO 5

class GunScope {
  private:
    int _ammo;
    int _x, _y;
  public:
    GunScope();
    void update();
};

class Chicken {
  private:
    int _x, _y;
  public:
    Chicken(int x, int y);
    void update();
};

class Bullet {
  private:
    int _x, _y;
    int _dmg;
  public:
    Bullet(int x, int y);
    void update();
};