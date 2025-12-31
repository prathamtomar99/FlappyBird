#include "BirdState.h"
#include <iostream>
#include <cmath>

inline float round2(float x) {
  return std::round(x * 100.0f) / 100.0f;
}

BirdState::BirdState()
:
  next_pipe_distance_x(0.0f),
  next_pipe_distance_upy(0.0f),
  next_pipe_distance_downy(0.0f),
  bird_y(0.0f),
  bird_velocity(0.0f)
{}

void BirdState::printState(){
    std::cout<<this->next_pipe_distance_x<<": ";
    std::cout<<this->next_pipe_distance_upy<<": ";
    std::cout<<this->next_pipe_distance_downy<<": ";
    std::cout<<this->bird_y<<": ";
    std::cout<<this->bird_velocity<<": \n";
}

void BirdState::normalizePoints(){
  // this->printState();
  this->next_pipe_distance_x = round2(this->next_pipe_distance_x/700.0f);
  this->next_pipe_distance_upy = round2(this->next_pipe_distance_upy/500.0f);
  this->next_pipe_distance_downy = round2(this->next_pipe_distance_downy/500.0f);
  this->bird_y = round2(this->bird_y/600.0f);
  this->bird_velocity = round2((this->bird_velocity+10.0f)/20.0f);
  // this->printState();
}

void BirdState::reset(Bird& bird){
  this->next_pipe_distance_x = 0.0f;
  this->next_pipe_distance_upy = 0.0f;
  this->next_pipe_distance_downy = 0.0f;
  this->bird_y = 0.0f;
  this->bird_velocity = 0.0f;
}