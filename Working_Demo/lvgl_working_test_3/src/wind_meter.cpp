#include "src/wind_meter.h"

lv_obj_t* wind_meter = nullptr;
lv_meter_scale_t* wind_scale = nullptr;
lv_meter_indicator_t* runwayNeedleImg = nullptr;
lv_obj_t* wind_arrow = nullptr;

String normalizeIdent(const String& ident) {
  if (ident.length() > 2 && (ident.endsWith("L") || ident.endsWith("R") || ident.endsWith("C"))) {
    return ident.substring(0, ident.length() - 1);
  }
  return ident;
}

void initWindMeter(lv_obj_t* parent) {
  wind_meter = lv_meter_create(parent);
  lv_obj_set_size(wind_meter, 240, 240);
  lv_obj_align(wind_meter, LV_ALIGN_CENTER, 0, 0);
  lv_obj_clear_flag(wind_meter, LV_OBJ_FLAG_SCROLLABLE);

  wind_scale = lv_meter_add_scale(wind_meter);
  lv_meter_set_scale_ticks(wind_meter, wind_scale, 37, 1, 10, lv_color_black());
  lv_meter_set_scale_major_ticks(wind_meter, wind_scale, 9, 2, 15, lv_color_black(), -25);

  wind_arrow = lv_img_create(wind_meter);
  lv_img_set_src(wind_arrow, "S:/icons/wind_arrow.png");  // Adjust path as needed
  lv_obj_align(wind_arrow, LV_ALIGN_CENTER, 0, 0);
}

void setRunwayNeedle(const String& ident1, const String& ident2, int bearing, bool flip180) {
  if (!wind_meter) return;

  // Clear meter and rebuild
  lv_obj_clean(wind_meter);

  wind_scale = lv_meter_add_scale(wind_meter);
  lv_meter_set_scale_ticks(wind_meter, wind_scale, 37, 1, 10, lv_color_black());
  lv_meter_set_scale_major_ticks(wind_meter, wind_scale, 9, 2, 15, lv_color_black(), -25);

  wind_arrow = lv_img_create(wind_meter);
  lv_img_set_src(wind_arrow, "S:/icons/wind_arrow.png");
  lv_obj_align(wind_arrow, LV_ALIGN_CENTER, 0, 0);

  String base1 = normalizeIdent(ident1);
  String base2 = normalizeIdent(ident2);

  char path[64];
  snprintf(path, sizeof(path), "S:/runways/%s_%s.png", base1.c_str(), base2.c_str());

  runwayNeedleImg = lv_meter_add_needle_img(
    wind_meter,
    wind_scale,
    path,
    120, 15
  );

  int heading = flip180 ? (bearing + 180) % 360 : bearing;
  lv_meter_set_indicator_value(wind_meter, runwayNeedleImg, heading);

  Serial.printf("Runway needle: %s → Heading: %d\n", path, heading);
}
