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

  // Optional background style
  static lv_style_t style_meter_bg;
  lv_style_init(&style_meter_bg);
  lv_style_set_bg_color(&style_meter_bg, lv_color_make(230, 230, 230));
  lv_style_set_border_width(&style_meter_bg, 0);
  lv_obj_add_style(wind_meter, &style_meter_bg, LV_PART_MAIN);

  // Add scale
  wind_scale = lv_meter_add_scale(wind_meter);
  lv_meter_set_scale_ticks(wind_meter, wind_scale, 37, 1, 10, lv_color_black());
  lv_meter_set_scale_major_ticks(wind_meter, wind_scale, 9, 2, 15, lv_color_black(), -25);

  // Add wind arrow overlay
  wind_arrow = lv_img_create(wind_meter);
  lv_img_set_src(wind_arrow, "S:/icons/wind_arrow.png");
  lv_obj_align(wind_arrow, LV_ALIGN_CENTER, 0, 0);

  // Add cardinal labels
  lv_obj_add_event_cb(wind_meter, [](lv_event_t* e) {
    lv_obj_draw_part_dsc_t* dsc = lv_event_get_param(e);
    if (dsc->part != LV_PART_TICKS || dsc->id % 45 != 0) return;

    const char* label = nullptr;
    switch (dsc->id) {
      case 0: label = "N"; break;
      case 45: label = "NE"; break;
      case 90: label = "E"; break;
      case 135: label = "SE"; break;
      case 180: label = "S"; break;
      case 225: label = "SW"; break;
      case 270: label = "W"; break;
      case 315: label = "NW"; break;
    }

    if (label) {
      lv_draw_label_dsc_t label_dsc;
      lv_draw_label_dsc_init(&label_dsc);
      label_dsc.color = lv_color_black();
      lv_point_t pos = dsc->draw_area->pos;
      lv_draw_label(dsc->draw_ctx, &label_dsc, &pos, label, NULL);
    }
  }, LV_EVENT_DRAW_PART_BEGIN, NULL);
}

void setRunwayNeedle(const String& ident1, const String& ident2, int bearing, bool flip180) {
  if (!wind_meter) {
    Serial.println("⚠️ wind_meter is null — skipping runway needle setup.");
    return;
  }

  // Clear meter and rebuild scale
  lv_obj_clean(wind_meter);

  wind_scale = lv_meter_add_scale(wind_meter);
  if (!wind_scale) {
    Serial.println("⚠️ Failed to add wind_scale — aborting needle setup.");
    return;
  }

  lv_meter_set_scale_ticks(wind_meter, wind_scale, 37, 1, 10, lv_color_black());
  lv_meter_set_scale_major_ticks(wind_meter, wind_scale, 9, 2, 15, lv_color_black(), -25);

  // Recreate wind arrow
  wind_arrow = lv_img_create(wind_meter);
  if (!wind_arrow) {
    Serial.println("⚠️ Failed to create wind_arrow image.");
  } else {
    lv_img_set_src(wind_arrow, "S:/icons/wind_arrow.png");
    lv_obj_align(wind_arrow, LV_ALIGN_CENTER, 0, 0);
  }

  // Normalize identifiers
  String base1 = normalizeIdent(ident1);
  String base2 = normalizeIdent(ident2);

  char path[64];
  snprintf(path, sizeof(path), "S:/runways/%s_%s.png", base1.c_str(), base2.c_str());

  // Add runway needle
  runwayNeedleImg = lv_meter_add_needle_img(wind_meter, wind_scale, path, 120, 15);
  if (!runwayNeedleImg) {
    Serial.printf("⚠️ Failed to load runway needle image: %s\n", path);
    return;
  }

  int heading = flip180 ? (bearing + 180) % 360 : bearing;
  lv_meter_set_indicator_value(wind_meter, runwayNeedleImg, heading);

  Serial.printf("✅ Runway needle set: %s → Heading: %d\n", path, heading);
}
