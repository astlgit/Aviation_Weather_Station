#ifndef METAR_DATA_H
#define METAR_DATA_H

#include <Arduino.h>


void updateDisplay(const String& icao);
String fetchMetarJson();
String fetchStationJson();
void setStation(const String& icao);
//void fetchAvwxData(const char* icao);

#endif // METAR_DATA_H
