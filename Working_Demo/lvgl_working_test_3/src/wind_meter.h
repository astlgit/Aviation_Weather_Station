#pragma once
#include <lvgl.h>
#include <Arduino.h>

// Shared meter components
extern lv_obj_t* wind_meter;
extern lv_meter_scale_t* wind_scale;
extern lv_meter_indicator_t* runwayNeedleImg;
extern lv_obj_t* wind_arrow;

// Setup and update functions
void initWindMeter(lv_obj_t* parent);
void setRunwayNeedle(const String& ident1, const String& ident2, int bearing, bool flip180);
void setWindNeedle(int windDir);
