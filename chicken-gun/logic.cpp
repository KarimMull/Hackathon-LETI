#include "entities.h"

bool spawnEnemy(int x, int y) {
  for (int i = 0; i < MAX_ENEMIES; i++) {
    if (chickens[i] == nullptr) {         
      chickens[i] = new Chicken(x, y);
      return true;
    }
  }
  return false;
}