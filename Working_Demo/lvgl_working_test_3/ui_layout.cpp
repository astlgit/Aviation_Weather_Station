#include <Arduino.h>
#include <vector>
#include "ui_layout.h"
#include "metar_data.h"
#include "src/airport_popup.h"
#include "src/runway_data.h"
#include <lvgl.h>
#include "src/wind_meter.h"


// Meter components
lv_meter_scale_t* wind_scale = nullptr;
lv_meter_indicator_t* runwayNeedleImg = nullptr;
lv_obj_t* wind_arrow = nullptr;
lv_obj_t* wind_meter = nullptr;

// Runway UI
lv_obj_t* runway_row = nullptr;
std::vector<lv_obj_t*> runwayButtons;
std::vector<lv_obj_t*> runwayLabels;
String selectedRunwayPair = "";

// Global UI elements
lv_obj_t* label_station;
lv_obj_t* label_time;
lv_obj_t* label_flight;
lv_obj_t* label_wind;
lv_obj_t* label_crosswind;
lv_obj_t* label_vis;
lv_obj_t* label_ceiling;
lv_obj_t* label_clouds;
lv_obj_t* label_temp;
lv_obj_t* label_alt;
lv_obj_t* btn_airport;

// Styles
lv_style_t style_vfr, style_mvfr, style_ifr, style_lifr;
lv_style_t style_box_shadow;
lv_style_t style_container;

 static const lv_point_t wind_arrow_points[] = {
  {10, 0},   // tip
  {10, 80},    // left base
  {3, 60},   // center base
  {15, 100},
  {28, 60},
  {20, 80},
  {20, 0},     // right base
};

void initFlightStyles() {
  lv_style_init(&style_vfr);
  lv_style_set_bg_color(&style_vfr, lv_color_hex(0x00A000));
  lv_style_set_text_color(&style_vfr, lv_color_white());

  lv_style_init(&style_mvfr);
  lv_style_set_bg_color(&style_mvfr, lv_color_hex(0x0060FF));
  lv_style_set_text_color(&style_mvfr, lv_color_white());

  lv_style_init(&style_ifr);
  lv_style_set_bg_color(&style_ifr, lv_color_hex(0xD00000));
  lv_style_set_text_color(&style_ifr, lv_color_white());

  lv_style_init(&style_lifr);
  lv_style_set_bg_color(&style_lifr, lv_color_hex(0x7F00B2));
  lv_style_set_text_color(&style_lifr, lv_color_white());
}

void initBoxShadowStyle() {
  lv_style_init(&style_box_shadow);
  lv_style_set_bg_color(&style_box_shadow, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 2));
  lv_style_set_bg_opa(&style_box_shadow, LV_OPA_COVER);
  lv_style_set_border_width(&style_box_shadow, 2);
  lv_style_set_border_color(&style_box_shadow, lv_palette_darken(LV_PALETTE_BLUE, 3));
  lv_style_set_radius(&style_box_shadow, 6);
  lv_style_set_pad_all(&style_box_shadow, 6);
  lv_style_set_text_color(&style_box_shadow, lv_color_black());
  lv_style_set_shadow_width(&style_box_shadow, 8);
  lv_style_set_shadow_color(&style_box_shadow, lv_color_hex(0x444444));
  lv_style_set_shadow_ofs_x(&style_box_shadow, 2);
  lv_style_set_shadow_ofs_y(&style_box_shadow, 2);
}

void initContainerStyle() {
  lv_style_init(&style_container);
  lv_style_set_bg_color(&style_container, lv_palette_lighten(LV_PALETTE_BLUE, 2));
  lv_style_set_bg_opa(&style_container, LV_OPA_COVER);
  lv_style_set_border_width(&style_container, 0);
  lv_style_set_pad_all(&style_container, 0);
  lv_style_set_radius(&style_container, 0);
}

String normalizeIdent(const String& ident) {
  if (ident.length() > 2 && (ident.endsWith("L") || ident.endsWith("R") || ident.endsWith("C"))) {
    return ident.substring(0, ident.length() - 1);
  }
  return ident;
}

void setRunwayNeedle(const String& ident1, const String& ident2, int bearing, bool flip180) {
  // Remove old needle if it exists
  if (runwayNeedleImg) {
    lv_meter_remove_indicator(wind_meter, runwayNeedleImg);
    runwayNeedleImg = nullptr;
  }

  // Normalize identifiers for PNG filename
  String base1 = normalizeIdent(ident1);
  String base2 = normalizeIdent(ident2);

  char path[64];
  snprintf(path, sizeof(path), "S:/runways/%s_%s.png", base1.c_str(), base2.c_str());

  // Create new needle image
  runwayNeedleImg = lv_meter_add_needle_img(
    wind_meter,
    wind_scale,
    path,
    120, 15  // Pivot for 240×30 image
  );

  int heading = flip180 ? (bearing + 180) % 360 : bearing;
  lv_meter_set_indicator_value(wind_meter, runwayNeedleImg, heading);

  Serial.printf("Runway needle: %s → Heading: %d\n", path, heading);
}

void rebuildRunwayButtons() {
  if (!runway_row) return;

  lv_obj_clean(runway_row);
  runwayButtons.clear();

  for (const RunwayPair& rwy : runwayPairs) {
    String label = rwy.ident1 + "/" + rwy.ident2;

    lv_obj_t* btn = lv_btn_create(runway_row);
    lv_obj_set_width(btn, LV_SIZE_CONTENT);
    lv_obj_set_height(btn, 30);
    lv_obj_add_style(btn, &style_box_shadow, 0);

    lv_obj_t* lbl = lv_label_create(btn);
    lv_label_set_text(lbl, label.c_str());
    lv_obj_center(lbl);

    runwayButtons.push_back(btn);

    lv_obj_add_event_cb(btn, [](lv_event_t* e) {
      const char* label = lv_label_get_text(lv_obj_get_child(e->target, 0));
      selectedRunwayPair = String(label);

      lv_meter_set_scale_ticks(wind_meter, wind_scale, 37, 1, 10, lv_color_black());
      lv_meter_set_scale_major_ticks(wind_meter, wind_scale, 9, 2, 15, lv_color_black(), -25);

      for (const RunwayPair& rwy : runwayPairs) {
        String expected = rwy.ident1 + "/" + rwy.ident2;
        String reversed = rwy.ident2 + "/" + rwy.ident1;

        if (String(label) == expected || String(label) == reversed) {
          bool flip180 = (String(label) == reversed);
          int bearing = flip180 ? rwy.bearing2 : rwy.bearing1;

          setRunwayNeedle(rwy.ident1, rwy.ident2, bearing, flip180);
          break;
        }
      }

      highlightRunwayButtons();
    }, LV_EVENT_CLICKED, NULL);
  }
}

void highlightRunwayButtons() {
  for (lv_obj_t* btn : runwayButtons) {
    const char* label = lv_label_get_text(lv_obj_get_child(btn, 0));

    if (String(label) == selectedRunwayPair) {
      lv_obj_set_style_bg_color(btn, lv_color_hex(0x0060FF), 0);
      lv_obj_set_style_text_font(lv_obj_get_child(btn, 0), &lv_font_montserrat_16, 0);
    } else {
      lv_obj_set_style_bg_color(btn, lv_palette_lighten(LV_PALETTE_LIGHT_BLUE, 2), 0);
      lv_obj_set_style_text_font(lv_obj_get_child(btn, 0), &lv_font_montserrat_14, 0);
    }
  }
}

void setupUI() {
  lv_obj_t* scr = lv_scr_act();
  lv_obj_set_style_bg_color(scr, lv_palette_lighten(LV_PALETTE_BLUE, 2), 0);
  lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);

  initFlightStyles();
  initBoxShadowStyle();
  initContainerStyle();

  // === ROOT FLEX CONTAINER ===
  lv_obj_t* root = lv_obj_create(scr);
  lv_obj_set_size(root, lv_pct(100), lv_pct(100));
  lv_obj_set_flex_flow(root, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(root, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_all(root, 0, 0);
  lv_obj_add_style(root, &style_container, 0);

  // === TOP BAR ===
  lv_obj_t* top_bar = lv_obj_create(root);
  lv_obj_set_size(top_bar, lv_pct(100), 60);
  lv_obj_set_flex_flow(top_bar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(top_bar, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_all(top_bar, 10, 0);
  lv_obj_add_style(top_bar, &style_container, 0);

  btn_airport = lv_btn_create(top_bar);
  lv_obj_set_size(btn_airport, 120, 40);
  lv_obj_add_style(btn_airport, &style_box_shadow, 0);
  lv_obj_add_event_cb(btn_airport, [](lv_event_t* e) {
    showAirportInputPopup();
  }, LV_EVENT_CLICKED, NULL);

  label_station = lv_label_create(btn_airport);
  lv_label_set_text(label_station, "KRDU");
  lv_obj_center(label_station);

  label_flight = lv_label_create(top_bar);
  lv_obj_add_style(label_flight, &style_box_shadow, 0);
  lv_label_set_text(label_flight, "Flight: —");

  label_time = lv_label_create(top_bar);
  lv_obj_add_style(label_time, &style_box_shadow, 0);
  lv_label_set_text(label_time, "Updated: —");

  // === MAIN BODY ===
  lv_obj_t* main_body = lv_obj_create(root);
  lv_obj_set_size(main_body, lv_pct(100), lv_pct(100));
  lv_obj_set_flex_flow(main_body, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(main_body, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_set_style_pad_all(main_body, 10, 0);
  lv_obj_add_style(main_body, &style_container, 0);

  // === LEFT COLUMN ===
  lv_obj_t* col_left = lv_obj_create(main_body);
  lv_obj_set_size(col_left, 320, lv_pct(100));
  lv_obj_set_flex_flow(col_left, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(col_left, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_add_style(col_left, &style_container, 0);
  lv_obj_set_style_pad_row(col_left, 10, 0);

  // === WIND METER CONTAINER ===
wind_meter = lv_meter_create(col_left);
lv_obj_set_size(wind_meter, 240, 240);

// === BACKGROUND STYLE ===
static lv_style_t style_meter_bg;
lv_style_init(&style_meter_bg);
lv_style_set_bg_color(&style_meter_bg, lv_color_hex(0x63666A));
lv_style_set_bg_opa(&style_meter_bg, LV_OPA_COVER);
lv_obj_add_style(wind_meter, &style_meter_bg, LV_PART_MAIN);

// === WIND ARROW (RED LINE) ===
wind_arrow = lv_line_create(wind_meter);
lv_line_set_points(wind_arrow, wind_arrow_points, sizeof(wind_arrow_points) / sizeof(wind_arrow_points[0]));
lv_obj_set_style_line_color(wind_arrow, lv_palette_main(LV_PALETTE_RED), 0);
lv_obj_set_style_line_width(wind_arrow, 4, 0);
lv_obj_set_style_line_rounded(wind_arrow, false, 0);
lv_obj_set_style_transform_pivot_x(wind_arrow, 19, 0);  // Adjust based on arrow shape
lv_obj_set_style_transform_pivot_y(wind_arrow, 50, 0);
lv_obj_align(wind_arrow, LV_ALIGN_CENTER, 1, 6);

// === SCALE SETUP ===
wind_scale = lv_meter_add_scale(wind_meter);
lv_meter_set_scale_ticks(wind_meter, wind_scale, 37, 1, 10, lv_color_black());
lv_meter_set_scale_major_ticks(wind_meter, wind_scale, 9, 2, 15, lv_color_black(), -25);
lv_meter_set_scale_range(wind_meter, wind_scale, 0, 360, 360, 270);

/*// === RUNWAY PNG NEEDLE ===
runwayNeedleImg = lv_meter_add_needle_img(
  wind_meter,
  wind_scale,
  "S:/runways/01_19_1.png",  // PNG must point right
  100, 12  // Pivot point (adjust based on image dimensions)
);*/

// === CARDINAL LABELS ===
lv_obj_add_event_cb(wind_meter, [](lv_event_t* e) {
  lv_obj_draw_part_dsc_t* dsc = (lv_obj_draw_part_dsc_t*)lv_event_get_param(e);
  if (dsc->part == LV_PART_TICKS && dsc->label_dsc) {
    switch (dsc->value) {
      case 0: lv_snprintf(dsc->text, sizeof(dsc->text), "N"); break;
      case 90: lv_snprintf(dsc->text, sizeof(dsc->text), "E"); break;
      case 180: lv_snprintf(dsc->text, sizeof(dsc->text), "S"); break;
      case 270: lv_snprintf(dsc->text, sizeof(dsc->text), "W"); break;
      default: dsc->text[0] = '\0'; break;
    }
  }
}, LV_EVENT_DRAW_PART_BEGIN, NULL);

  
  // === RUNWAY BUTTONS ===
  runway_row = lv_obj_create(col_left);  // ✅ assigns to the global variable
  lv_obj_set_size(runway_row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
  lv_obj_set_flex_flow(runway_row, LV_FLEX_FLOW_ROW_WRAP);
  lv_obj_set_flex_align(runway_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_pad_row(runway_row, 5, 0);
  lv_obj_set_style_pad_column(runway_row, 5, 0);
  lv_obj_add_style(runway_row, &style_container, 0);

  //drawRunwayLabels();
  rebuildRunwayButtons();
  highlightRunwayButtons();
  

  // === CENTER COLUMN ===
  lv_obj_t* col_center = lv_obj_create(main_body);
  lv_obj_set_size(col_center, 200, lv_pct(100));
  lv_obj_set_flex_flow(col_center, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(col_center, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_add_style(col_center, &style_container, 0);
  lv_obj_set_style_pad_row(col_center, 10, 0);

  label_wind = lv_label_create(col_center);
  lv_obj_add_style(label_wind, &style_box_shadow, 0);
  lv_label_set_text(label_wind, "Wind —");

  label_crosswind = lv_label_create(col_center);
  lv_obj_add_style(label_crosswind, &style_box_shadow, 0);
  //lv_label_set_long_mode(label_crosswind, LV_LABEL_LONG_WRAP);
  //lv_obj_set_width(label_crosswind, 200);
  lv_label_set_text(label_crosswind, "Runway winds —");

  label_vis = lv_label_create(col_center);
  lv_obj_add_style(label_vis, &style_box_shadow, 0);
  lv_label_set_text(label_vis, "Vis: —");

  label_ceiling = lv_label_create(col_center);
  lv_obj_add_style(label_ceiling, &style_box_shadow, 0);
  lv_label_set_text(label_ceiling, "Ceil: —");

  label_temp = lv_label_create(col_center);
  lv_obj_add_style(label_temp, &style_box_shadow, 0);
  lv_label_set_text(label_temp, "Temp: —");

  // === RIGHT COLUMN ===
  lv_obj_t* col_right = lv_obj_create(main_body);
  lv_obj_set_size(col_right, 240, lv_pct(100));
  lv_obj_set_flex_flow(col_right, LV_FLEX_FLOW_COLUMN);
  lv_obj_set_flex_align(col_right, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
  lv_obj_add_style(col_right, &style_container, 0);
  lv_obj_set_style_pad_row(col_right, 10, 0);

  label_clouds = lv_label_create(col_right);
  lv_obj_add_style(label_clouds, &style_box_shadow, 0);
  lv_label_set_text(label_clouds, "Clouds: —");

  label_alt = lv_label_create(col_right);
  lv_obj_add_style(label_alt, &style_box_shadow, 0);
  lv_label_set_text(label_alt, "Alt: —");
}
