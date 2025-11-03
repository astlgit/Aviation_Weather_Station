#pragma once
#include <Arduino.h>
#include <vector>

struct RunwayPair {
  String ident1;
  int bearing1;
  String ident2;
  int bearing2;
};


extern std::vector<RunwayPair> runwayPairs;

