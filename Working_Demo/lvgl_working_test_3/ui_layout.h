#pragma once
#include <Arduino.h>
#include <vector>
#include <lvgl.h>

// === UI Elements ===
extern lv_obj_t* label_station;
extern lv_obj_t* label_time;
extern lv_obj_t* label_flight;
extern lv_obj_t* label_wind;
extern lv_obj_t* label_crosswind;
extern lv_obj_t* label_vis;
extern lv_obj_t* label_ceiling;
extern lv_obj_t* label_clouds;
extern lv_obj_t* label_temp;
extern lv_obj_t* label_alt;
extern lv_obj_t* label_runways;
extern lv_obj_t* btn_airport;
extern lv_obj_t* wind_meter;
extern lv_obj_t* wind_arrow;
extern lv_obj_t* runway_row;

// === Runway UI State ===
extern String selectedRunwayPair;
extern std::vector<lv_obj_t*> runwayButtons;
extern std::vector<lv_obj_t*> runwayLabels;

// === Meter Components ===
extern lv_meter_scale_t* wind_scale;
extern lv_meter_indicator_t* runwayNeedleImg;  // PNG-based runway needle

// === Styles ===
extern lv_style_t style_vfr;
extern lv_style_t style_mvfr;
extern lv_style_t style_ifr;
extern lv_style_t style_lifr;
extern lv_style_t style_box_shadow;
extern lv_style_t style_container;

// === UI Functions ===
void setupUI();
void initFlightStyles();
void initBoxShadowStyle();
void initContainerStyle();
void showAirportInputPopup();
void drawCompassBackground();
void rebuildRunwayButtons();
void highlightRunwayButtons();
void setWindNeedle(int windDir);
