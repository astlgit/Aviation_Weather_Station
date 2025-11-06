#pragma once
#include <Arduino.h>
#include <vector>

// Represents a runway pair with identifiers and bearings
struct RunwayPair {
  String ident1;
  int bearing1;
  String ident2;
  int bearing2;
};

// Global runway pair list
extern std::vector<RunwayPair> runwayPairs;

void preloadRunwaysForAirport(const std::vector<RunwayPair>& runways);

// Load default runway pairs
void preloadDefaultRunways();

// Rotate the runway needle using a heading
void showRunwayNeedle(const String& ident1, const String& ident2, int bearing);
