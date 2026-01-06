#pragma once

#include "entity.hpp"

class entityLine;
class game;
struct Point3d;

const int numEnemyWanderer = 100;
const int numEnemyGrunt = 200;
const int numEnemySpinner = 100;
const int numEnemyTinySpinner = 100;
const int numEnemyWeaver = 200;
const int numEnemySnake = 50;
const int numEnemyBlackHole = 8;
const int numEnemyRepulsor = 4;
const int numEnemyMayfly = 400;
const int numEnemyProton = 200;

#define NUM_ENEMIES (numEnemyWanderer + numEnemyGrunt + numEnemySpinner + numEnemyTinySpinner + numEnemyWeaver + numEnemySnake + numEnemyBlackHole + numEnemyRepulsor + numEnemyMayfly + numEnemyProton)
#define NUM_LINES   250

class enemies
{
  public:
    enemies(const game& gameRef);
    ~enemies();

    void run();
    void draw();

    entity* getUnusedEnemyOfType(const entity::EntityType& type);

    int getNumActiveEnemiesOfType(const entity::EntityType& type);

    entity* hitTestEnemiesAtPosition(const Point3d& point, float radius, bool includeSpawning = false);

    void disableAllEnemies();
    void disableAllLines();

    entityLine* getUnusedLine();
    void explodeEntity(entity& e);

    int getBlackHoleStart() const;

    entity* mEnemies[NUM_ENEMIES];
    entityLine* mLines[NUM_LINES];

    const game& mGame;
};
