#pragma once
#include <lvgl.h>
#include <Arduino.h>

extern lv_obj_t* wind_meter;
extern lv_meter_scale_t* wind_scale;
extern lv_meter_indicator_t* runwayNeedleImg;
extern lv_meter_indicator_t* windNeedleImg;
extern lv_obj_t* wind_arrow;

void initWindMeter(lv_obj_t* parent);
void setWindNeedle(int windDir);
void setRunwayNeedle(int bearing);

