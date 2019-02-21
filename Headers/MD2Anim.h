#pragma once

class MD2Model;

class MD2Anim
{
public:
    typedef enum {
        HOLD     = 12313,
        TRIGGER, // increment as a cycle each call
        TRAVEL,  // go from start to finsh then stop
        BOUNCE,	 // bounce from start to finish to start
        CYCLE,   // loop from start to fish then back to start
        NOACTION
    } AnimType;

    MD2Anim();
    ~MD2Anim();

public:
    bool Update(float dt,bool trigger = true); // do what you need to do
    bool SetSequence(const char *AnimName, MD2Model* Model, AnimType How = CYCLE, float Step = 5.0f/60.0f, const char *Queued = "");
    bool SetSequence(int start, int stop, AnimType How);
    bool QueueSeqence(char*name, MD2Model* Model);
    bool QueueSequence(int start, int stop, AnimType How);
    bool UpdateSlave();

public:
    AnimType WhatType;
    int CurrentFrame; // what to draw
    int NextFrame;	  // used by interpolaiton
    int BaseFrame;    // start of sequence
    int FinalFrame;   // end of sequence
    int Direction;    // forward or back travel for the sequence
    float Time;
    float TimeStep;
    MD2Anim* Slave;   // main models can set weapons

private:
    int QNextBase;
    int QNextFinal;
    AnimType QNextType;
};
