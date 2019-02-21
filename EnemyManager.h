#pragma once

// A very simple manger class its only important function is
// to parse the enemies list and place the objects in the level.
// It can be expanded to check on the state of enemies
// and provide respawn or response systems.

class Game;

class EnemyManager
{
public:
    EnemyManager();
    ~EnemyManager();

public:
    void CreateEnemies(Game* TheGame);
};
