#define MAP_WIDTH 200
#define MAP_HEIGHT 100

#define BULLET_DMG 10
#define PLAYER_HP 100

#define CHICKEN_DMG 20
#define CHICKEN_HP 30

enum class ChickenMove {Vertically, Diagonally};

class Player {
  private:
    int _x, _y;
    int _health;
  public:
    Player();
};

class Chicken {
  private:
    int _x, _y;
    int _dmg;
    int _health;
    ChickenMove _moveType;
  public:
    Chicken(int x, int y, ChickenMove moveType);
};

class Bullet {
  private:
    int _x, _y;
    int _dmg;
  public:
    Bullet(int x, int y);
};
