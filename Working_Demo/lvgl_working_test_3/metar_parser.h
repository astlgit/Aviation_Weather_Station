#pragma once
#include <ArduinoJson.h>

namespace MetarParser {
  void parseCeiling(JsonDocument& doc);
  void parseAltimeter(JsonDocument& doc);
  void parseFlightCategory(JsonDocument& doc);
  void parseWind(JsonDocument& doc);
  void parseRunways(const String& json);
  void parseRunwayWind(JsonDocument& doc);
  void parseVisibility(JsonDocument& doc);
  void parseClouds(JsonDocument& doc);
  void parseTemperature(JsonDocument& doc);
  void parseTimestamp(JsonDocument& doc);
  
}
