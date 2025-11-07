#include "metar_data.h"
#include "metar_parser.h"
#include "ui_layout.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "src/runway_data.h"

String station = "KRDU";
String apiKey  = "-D0GYKSukOV6-m33arp2Q6XfJBO8DdP-yrYfnC1-ptQ";

void setStation(const String& icao) {
  station = icao;
}

String fetchMetarJson() {
  HTTPClient http;
  String url = "https://avwx.rest/api/metar/" + station +
               "?token=" + apiKey +
               "&options=info";
  http.begin(url);
  int code = http.GET();
  String payload = "{}";
  if (code == 200) payload = http.getString();
  http.end();
  return payload;
}

/*String fetchStationJson() {
  HTTPClient http;
  String url = "https://avwx.rest/api/station/" + station +
               "?token=" + apiKey +
               "&format=json&filter=runways,bearing1,bearing2,ident1,ident2";
  http.begin(url);
  int code = http.GET();
  String payload = "{}";
  if (code == 200) payload = http.getString();
  http.end();
  return payload;
}*/

void updateDisplay(const String& icao) {
  setStation(icao);

  // Fetch METAR JSON
  String metarJson = fetchMetarJson();
  StaticJsonDocument<4096> metarDoc;
  DeserializationError metarErr = deserializeJson(metarDoc, metarJson);
  if (metarErr) {
    Serial.print("METAR JSON error: ");
    Serial.println(metarErr.c_str());
    lv_label_set_text(label_station, "METAR error");
    return;
  }

  // Fetch Station JSON
  /*String stationJson = fetchStationJson();
  DynamicJsonDocument stationDoc(8192);
  DeserializationError stationErr = deserializeJson(stationDoc, stationJson);
  if (stationErr) {
    Serial.print("Station JSON error: ");
    Serial.println(stationErr.c_str());
    lv_label_set_text(label_crosswind, "Runway data unavailable");
    return;
  }*/

  // Parse and update runway data before calculating wind alignment
  MetarParser::parseRunways(metarJson);
  rebuildRunwayButtons();
  //drawRunwayLabels();
  //Serial.printf("Rebuilt %d runway buttons\n", runwayPairs.size());

  // Parse METAR fields
  MetarParser::parseCeiling(metarDoc);
  MetarParser::parseAltimeter(metarDoc);
  MetarParser::parseFlightCategory(metarDoc);
  MetarParser::parseWind(metarDoc);
  MetarParser::parseRunwayWind(metarDoc);  // Now uses updated runwayPairs
  MetarParser::parseVisibility(metarDoc);
  MetarParser::parseClouds(metarDoc);
  MetarParser::parseTemperature(metarDoc);
  MetarParser::parseTimestamp(metarDoc);

  // Update station label
  lv_label_set_text(label_station, station.c_str());
}
