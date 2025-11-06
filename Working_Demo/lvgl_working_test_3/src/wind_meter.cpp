#include "src/wind_meter.h"
#include <lvgl.h>

lv_obj_t* wind_meter = nullptr;
lv_meter_scale_t* wind_scale = nullptr;
lv_meter_indicator_t* runwayNeedleImg = nullptr;
lv_meter_indicator_t* windNeedleImg = nullptr;

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
  lv_meter_set_scale_range(wind_meter, wind_scale, 0, 360, 360, 270);  // ✅ Now included

  // Add wind arrow as needle image
  windNeedleImg = lv_meter_add_needle_img(wind_meter, wind_scale, "S:/icons/wind_arrow.png", 100, 12);
  if (!windNeedleImg) {
    Serial.println("⚠️ Failed to load wind_arrow.png as needle.");
  }

  // Add cardinal labels
  lv_obj_add_event_cb(wind_meter, [](lv_event_t* e) {
    lv_obj_draw_part_dsc_t* dsc = (lv_obj_draw_part_dsc_t*)lv_event_get_param(e);
    if (dsc->part == LV_PART_TICKS && dsc->label_dsc) {
      switch (dsc->value) {
        case 0:   lv_snprintf(dsc->text, sizeof(dsc->text), "N");  break;
        case 45:  lv_snprintf(dsc->text, sizeof(dsc->text), "NE"); break;
        case 90:  lv_snprintf(dsc->text, sizeof(dsc->text), "E");  break;
        case 135: lv_snprintf(dsc->text, sizeof(dsc->text), "SE"); break;
        case 180: lv_snprintf(dsc->text, sizeof(dsc->text), "S");  break;
        case 225: lv_snprintf(dsc->text, sizeof(dsc->text), "SW"); break;
        case 270: lv_snprintf(dsc->text, sizeof(dsc->text), "W");  break;
        case 315: lv_snprintf(dsc->text, sizeof(dsc->text), "NW"); break;
        default:  dsc->text[0] = '\0'; break;
      }
    }
  }, LV_EVENT_DRAW_PART_BEGIN, NULL);
}

void setRunwayNeedle(int bearing) {
  if (!wind_meter || !wind_scale) return;

  const char* path = "S:/runways/02_20.png";  // ✅ Fixed image for testing

  if (!runwayNeedleImg) {
    runwayNeedleImg = lv_meter_add_needle_img(wind_meter, wind_scale, path, 100, 12);
    if (!runwayNeedleImg) {
      Serial.println("⚠️ Failed to load runway needle image.");
      return;
    }
  }

  lv_meter_set_indicator_value(wind_meter, runwayNeedleImg, bearing);
}

void setWindNeedle(int windDir) {
  if (windNeedleImg) {
    lv_meter_set_indicator_value(wind_meter, windNeedleImg, windDir);
  } else {
    Serial.println("⚠️ windNeedleImg is null — cannot rotate wind needle.");
  }
}
