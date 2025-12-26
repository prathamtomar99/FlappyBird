#pragma once
#include "Globals.h"
#include "Bird.h"

class BirdState{
public:
    BirdState();
    float next_pipe_distance_x,next_pipe_distance_upy,next_pipe_distance_downy;
    float bird_y,bird_velocity;
    void printState();
    void normalizePoints();
    void reset(Bird&);
};

