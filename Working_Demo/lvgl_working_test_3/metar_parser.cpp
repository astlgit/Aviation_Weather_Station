#include "metar_parser.h"
#include "ui_layout.h"
#include <math.h>
#include "src/runway_data.h"
#include "src/wind_meter.h"

extern std::vector<RunwayPair> runwayPairs;

extern lv_style_t style_vfr, style_mvfr, style_ifr, style_lifr;

namespace MetarParser {

void setWindNeedle(int windDir) {
  if (windNeedleImg) {
    lv_meter_set_indicator_value(wind_meter, windNeedleImg, windDir);
  } else {
    Serial.println("⚠️ windNeedleImg is null — cannot rotate wind needle.");
  }
}

void computeWindComponents(int wind_dir, float wind_spd, int rw_heading, float &head, float &cross) {
  int diff = (wind_dir - rw_heading + 360) % 360;
  float rad = diff * 3.14159 / 180.0;
  head = wind_spd * cos(rad);
  cross = wind_spd * sin(rad);
}

void parseCeiling(JsonDocument& doc) {
  JsonArray clouds = doc["clouds"].as<JsonArray>();
  const char* ceiling_repr = nullptr;
  int ceiling_ft = -1;

  for (JsonObject c : clouds) {
    const char* type = c["type"] | "";
    const char* repr = c["repr"] | nullptr;
    int altitude = c["altitude"] | -1;

    if ((strcmp(type, "BKN") == 0 || strcmp(type, "OVC") == 0) && altitude >= 0) {
      int ft = altitude * 100;
      if (ceiling_ft == -1 || ft < ceiling_ft) {
        ceiling_ft = ft;
        ceiling_repr = repr ? repr : type;
      }
    }
  }

  if (ceiling_ft >= 0) {
    String text = "Ceil: ";
    text += ceiling_repr;
    text += " (";
    text += ceiling_ft;
    text += " ft)";
    lv_label_set_text(label_ceiling, text.c_str());
  } else {
    lv_label_set_text(label_ceiling, "Ceil: None");
  }
}

void parseAltimeter(JsonDocument& doc) {
  float alt_value = doc["altimeter"]["value"] | -1.0;
  if (alt_value > 0) {
    String text = "Alt: ";
    text += String(alt_value, 2);
    text += " inHg";
    lv_label_set_text(label_alt, text.c_str());
  } else {
    lv_label_set_text(label_alt, "Alt: N/A");
  }
}

void parseFlightCategory(JsonDocument& doc) {
  const char* cat = doc["flight_rules"] | "...";
  lv_label_set_text_fmt(label_flight, "Flight: %s", cat);

  lv_obj_remove_style_all(label_flight);
  lv_obj_add_style(label_flight, &style_box_shadow, 0);

  if (strcmp(cat, "VFR") == 0) {
    lv_obj_add_style(label_flight, &style_vfr, 0);
  } else if (strcmp(cat, "MVFR") == 0) {
    lv_obj_add_style(label_flight, &style_mvfr, 0);
  } else if (strcmp(cat, "IFR") == 0) {
    lv_obj_add_style(label_flight, &style_ifr, 0);
  } else if (strcmp(cat, "LIFR") == 0) {
    lv_obj_add_style(label_flight, &style_lifr, 0);
  }
}

void parseWind(JsonDocument& doc) {
  int wind_dir = doc["wind_direction"]["value"] | 0;
  float wind_spd = doc["wind_speed"]["value"] | 0.0;
  Serial.printf("Parsed wind direction: %d\n", wind_dir);
  lv_label_set_text_fmt(label_wind, "Wind %03d° %d kt", wind_dir, (int)wind_spd);
  setWindNeedle(wind_dir);
}

void parseRunways(const String& json) {
  runwayPairs.clear();
  StaticJsonDocument<2048> doc;
  DeserializationError err = deserializeJson(doc, json);
  if (err) {
    Serial.print("Runway JSON error: ");
    Serial.println(err.c_str());
    return;
  }

  JsonArray rwys = doc["runways"].as<JsonArray>();
  std::vector<RunwayPair> parsedRunways;

  for (JsonVariant v : rwys) {
    JsonObject rw = v.as<JsonObject>();

    const char* ident1 = rw["ident1"] | "";
    const char* ident2 = rw["ident2"] | "";
    float bearing1 = rw["bearing1"] | -1.0;
    float bearing2 = rw["bearing2"] | -1.0;

    if (strlen(ident1) > 0 && strlen(ident2) > 0 &&
        bearing1 >= 0 && bearing2 >= 0) {
      parsedRunways.push_back({ String(ident1), (int)bearing1, String(ident2), (int)bearing2 });
    }
  }

  preloadRunwaysForAirport(parsedRunways);
}

void parseRunwayWind(JsonDocument& doc) {
  int wind_dir = doc["wind_direction"]["value"] | 0;
  float wind_spd = doc["wind_speed"]["value"] | 0.0;

  if (runwayPairs.empty()) {
    lv_label_set_text(label_crosswind, "No runway data available");
    return;
  }

  String rwText = "";
  for (auto& rw : runwayPairs) {
    float head1, cross1, head2, cross2;
    computeWindComponents(wind_dir, wind_spd, rw.bearing1, head1, cross1);
    computeWindComponents(wind_dir, wind_spd, rw.bearing2, head2, cross2);

    rwText += rw.ident1 + "/" + rw.ident2 + ": ";
    rwText += "head " + String((int)head1) + "/" + String((int)head2) + " kt, ";
    rwText += "cross " + String((int)fabs(cross1)) + "/" + String((int)fabs(cross2)) + " kt\n";
  }

  lv_label_set_text(label_crosswind, rwText.c_str());
}

void parseVisibility(JsonDocument& doc) {
  const char* vis = doc["visibility"]["repr"] | "";
  lv_label_set_text_fmt(label_vis, "Vis: %s sm", vis);
}

void parseClouds(JsonDocument& doc) {
  JsonArray clouds = doc["clouds"].as<JsonArray>();
  String cltxt = "";

  for (JsonObject c : clouds) {
    const char* type = c["type"] | "";
    const char* repr = c["repr"] | nullptr;
    int altitude = c["altitude"] | -1;

    if (strlen(type) > 0) {
      if (repr) {
        cltxt += String(type) + " " + String(repr) + " ft\n";
      } else if (altitude >= 0) {
        cltxt += String(type) + " " + String(altitude * 100) + " ft\n";
      } else {
        cltxt += String(type) + " — ft\n";
      }
    }
  }

  if (cltxt.length() == 0) cltxt = "No clouds reported";
  lv_label_set_text(label_clouds, cltxt.c_str());
}

void parseTemperature(JsonDocument& doc) {
  int tC = doc["temperature"]["value"] | INT32_MIN;
  int dC = doc["dewpoint"]["value"] | INT32_MIN;
  lv_label_set_text_fmt(label_temp, "Temp %d°C\n  Dew %d°C", tC, dC);
}

void parseTimestamp(JsonDocument& doc) {
  const char* dt = doc["time"]["repr"] | "";
  lv_label_set_text_fmt(label_time, "Updated: %sZ", dt);
}

}  // namespace MetarParser
