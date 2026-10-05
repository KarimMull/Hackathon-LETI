#include "entities.h"
#include "logic.h"

Chicken* chickens[MAX_ENEMIES];

void setup() {
  Serial.begin(115200);
  GunScope scope = GunScope();
}

unsigned long lastSpawn = 0;

void loop() {
  if(millis() - lastSpawn >= SPAWN_INTERVAL){
    lastSpawn = millis();
    spawnEnemy(random(20, MAP_WIDTH - 20), random(20, 100));
  }

  delay(16);
}
