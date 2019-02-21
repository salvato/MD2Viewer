#pragma once

//=====================================================
// a placement system for our enemies in the test maze.
//=====================================================


typedef struct EnemyData {
    int Xpos;
    int Ypos;
    int Zpos;
    int Type;
    // this struct has a constructor
    EnemyData(int p1, int p2, int p3, int p4) {
        Xpos = p1;
        Ypos = p2;
        Zpos = p3;
        Type = p4;
    }
} EnemyData;


enum Types {
    TURRET = 0,
    BATTLE,
    KNIGHT
};


// Here is a nice list of all our baddies,
// our EnemyManager class can instantiate these.
static
EnemyData Enemies[] = {
    EnemyData(-157, 15, 30, KNIGHT),
    EnemyData(-120, 15, 30, KNIGHT),
    EnemyData(-120, 15, -1, TURRET),
    EnemyData(-132, 15, -1, BATTLE),
    EnemyData(-124, 15, -1, TURRET),
    EnemyData(-126, 15, -1, TURRET),
    EnemyData(-128, 15, -1, TURRET),
    EnemyData(-130, 15, -1, TURRET)
};
