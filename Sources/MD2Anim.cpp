#include "MD2.h"
#include "MD2Anim.h"
#include "MD2Model.h"


MD2Anim::MD2Anim() {
    QNextBase = -1; // 0 is a valid number
//clear up the main initial values to all 0
    CurrentFrame = NextFrame = BaseFrame = 0;
    WhatType = HOLD; // ensure no movement occurs until a sequence is triggered
    Direction = 1;
    Slave = nullptr; // its up to the creating project to set any weapons
}


// nothing really to destroy allow a clean exit
MD2Anim::~MD2Anim(){
}


bool
MD2Anim::Update(float dt, bool trigger) {
    Time -= dt;
    if(Time > 0) return false; // no animation took palce
    Time = TimeStep;
// watch out for single frames
    if((FinalFrame-BaseFrame) <= 1) {
        CurrentFrame = NextFrame = BaseFrame;
        return true;
    }
    // handle the different cycles
    switch(WhatType) {
        case HOLD: {
            NextFrame = CurrentFrame;
            break;
        }
// TRIGGER is basically a cycle but it needs the trigger
// to be set otherwise it will switch to the queued
        case TRIGGER: {
            CurrentFrame += Direction;
            if(CurrentFrame > FinalFrame) {
                CurrentFrame = BaseFrame;
            }
            if(trigger) {// trigger is in place so do normal sequence things
                NextFrame = CurrentFrame + Direction;
                if(NextFrame > FinalFrame) NextFrame = BaseFrame;
                if(NextFrame < BaseFrame)  NextFrame = FinalFrame;
                break;
            }
            else {// trigger has been released, so we need to change the animation
                BaseFrame  = QNextBase;
                FinalFrame = QNextFinal;
                WhatType   = QNextType;
                CurrentFrame = BaseFrame;
                NextFrame = BaseFrame + Direction;
                //watch out for single frames
                if (NextFrame > FinalFrame) NextFrame = FinalFrame;
                QNextType = NOACTION; // do this only once, the logic need
            }
            break;
        }
        case CYCLE: {
            CurrentFrame += Direction;
            if(CurrentFrame > FinalFrame) {
                CurrentFrame = BaseFrame;
                if(this->QNextType != NOACTION) {// we need to check if the cycle had a next  cycle
                    // there's a new sequence to trigger
                    CurrentFrame = this->QNextBase;
                    BaseFrame = CurrentFrame;
                    FinalFrame = this->QNextFinal;
                    QNextType = NOACTION; // only do this once, the logic need to check
                }
            }
            NextFrame = CurrentFrame + Direction;
            if (NextFrame > FinalFrame) NextFrame = BaseFrame;
            if (NextFrame < BaseFrame) NextFrame = FinalFrame;
            break;
        }
        case TRAVEL: {// go to the end and stop
            CurrentFrame += Direction;
            if((CurrentFrame > FinalFrame) || (CurrentFrame < BaseFrame)) {
                NextFrame = CurrentFrame = (Direction <0) ? BaseFrame:FinalFrame ;
                return true; // we're done
            }
            NextFrame = CurrentFrame + Direction;
            if(NextFrame > FinalFrame) NextFrame = FinalFrame;
            if(NextFrame < BaseFrame)  NextFrame = BaseFrame;
            break;
        }
        case BOUNCE: {
            CurrentFrame += Direction;
            if(Direction > 0) {
                if(CurrentFrame > FinalFrame) {
                    CurrentFrame = FinalFrame-1;
                    Direction = -Direction;
                }
            }
            else {
                if(CurrentFrame < BaseFrame) {
                    CurrentFrame = BaseFrame+1;
                    Direction = -Direction;
                }
            }
            break;
        }
        default: {	} 	// choose to report here?
    }// switch(WhatType)
    return true; // no issues reported
}


bool
MD2Anim::UpdateSlave() {
    if(Slave == nullptr) return false;
    Slave->CurrentFrame = CurrentFrame;
    Slave->NextFrame = NextFrame;
    // Don't let it animate just use the master values,
    // force a default/no action
    Slave->WhatType = NOACTION;
    return true;
}


bool
MD2Anim::SetSequence(const char* AnimName,
                     MD2Model* Model,
                     AnimType How,//=CYCLE
                     float TS,    // = 5.0f/60.0f
                     const char* QueuedeName)//=""
{
    MD2Animation TheData = Model->TheMD2Data->NamedAnimation[AnimName];
    if(BaseFrame == TheData.startFrame) return false; // we're already doing it

    BaseFrame = TheData.startFrame;
    FinalFrame = BaseFrame+TheData.numFrames-1;
    CurrentFrame = BaseFrame;
    WhatType = How;
    TimeStep = TS;
    Time = 0;
// If a queuded sequence is set up, put it here,
// this might be at the end of an attack, or a run
// Set up as no action to indicate possible no next sequence
    QNextType = NOACTION;

    if(*QueuedeName == '\0') return true; // no QueuedeName needed
// else set up the queued sequence
    TheData = Model->TheMD2Data->NamedAnimation[QueuedeName];
    this->QNextBase = TheData.startFrame;
    this->QNextFinal = QNextBase + TheData.numFrames - 1;
    this->QNextType = CYCLE; // this can be reset
    return true;
}


// Just changes the sequence does nothing to the time step
bool
MD2Anim::SetSequence(int start, int stop, AnimType How) {
    BaseFrame = start;
    FinalFrame = stop-1;
    CurrentFrame = BaseFrame;
    WhatType = How;
    return true;
}


// This simply sets up the next animation sequence
// should a sequence end or a trigger not be in place
bool
MD2Anim::QueueSeqence(char* AnimName, MD2Model* Model) {
    MD2Animation TheData = Model->TheMD2Data ->NamedAnimation[AnimName];
    if(BaseFrame == TheData.startFrame) return false;// we're already doing it
    BaseFrame = TheData.startFrame;
    FinalFrame = BaseFrame + TheData.numFrames - 1;
    CurrentFrame = BaseFrame;
    WhatType = CYCLE;
    TimeStep = 5.0f/60.0f;
    Time = 0;
    return true;
}
