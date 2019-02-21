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
    EnemyData(-30, 10, -5, BATTLE),
    EnemyData(-30, 11,  5, BATTLE),
    EnemyData(-30, 12, 10, TURRET),
    EnemyData(-10, 13, 10, BATTLE),
    EnemyData( 10, 14,  5, TURRET),
    EnemyData( 15, 15,  0, TURRET),
    EnemyData(-25, 1, -25, TURRET)
};
