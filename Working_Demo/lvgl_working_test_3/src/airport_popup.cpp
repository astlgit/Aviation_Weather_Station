#include "src/airport_popup.h"
#include "ui_layout.h"
#include "metar_data.h"
#include <lvgl.h>
#include <Arduino.h>

void showAirportInputPopup() {
  lv_obj_t* popup = lv_obj_create(lv_scr_act());
  lv_obj_set_size(popup, 700, 360);  // Optimized for 800x400 screen
  lv_obj_center(popup);
  lv_obj_set_style_bg_color(popup, lv_palette_lighten(LV_PALETTE_GREY, 2), 0);
  lv_obj_set_style_pad_all(popup, 12, 0);
  lv_obj_set_style_radius(popup, 10, 0);

  lv_obj_t* title = lv_label_create(popup);
  lv_label_set_text(title, "Enter ICAO Code");
  lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
  lv_obj_set_style_text_font(title, &lv_font_montserrat_22, 0);

  lv_obj_t* airport_input = lv_textarea_create(popup);
  lv_obj_set_size(airport_input, 600, 60);
  lv_obj_align(airport_input, LV_ALIGN_TOP_MID, 0, 60);
  lv_textarea_set_placeholder_text(airport_input, "e.g. KJFK");
  lv_textarea_set_one_line(airport_input, true);
  lv_textarea_set_max_length(airport_input, 4);
  lv_textarea_set_text(airport_input, "");
  lv_obj_add_state(airport_input, LV_STATE_FOCUSED);
  lv_obj_set_style_text_font(airport_input, &lv_font_montserrat_20, 0);

  lv_obj_t* kb = lv_keyboard_create(popup);
  lv_obj_set_size(kb, 680, 180);
  lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, -10);
  lv_keyboard_set_textarea(kb, airport_input);
  lv_keyboard_set_mode(kb, LV_KEYBOARD_MODE_TEXT_UPPER);

  lv_obj_t* ok_btn = lv_btn_create(popup);
  lv_obj_set_size(ok_btn, 100, 40);
  lv_obj_align(ok_btn, LV_ALIGN_TOP_RIGHT, -20, -10);
  lv_obj_set_style_bg_color(ok_btn, lv_palette_main(LV_PALETTE_BLUE), 0);
  lv_obj_set_style_radius(ok_btn, 8, 0);

  lv_obj_add_event_cb(ok_btn, [](lv_event_t* e) {
    lv_obj_t* input = (lv_obj_t*)lv_event_get_user_data(e);
    const char* code = lv_textarea_get_text(input);
    if (strlen(code) == 4) {
      updateDisplay(String(code));  // Triggers METAR fetch + UI update
    } else {
      lv_label_set_text(label_station, "Invalid ICAO");
    }
    lv_obj_del(lv_obj_get_parent(lv_event_get_target(e)));  // Close popup
  }, LV_EVENT_CLICKED, airport_input);

  lv_obj_t* ok_label = lv_label_create(ok_btn);
  lv_label_set_text(ok_label, "OK");
  lv_obj_center(ok_label);

  lv_obj_t* cancel_btn = lv_btn_create(popup);
  lv_obj_set_size(cancel_btn, 100, 40);
  lv_obj_align(cancel_btn, LV_ALIGN_TOP_LEFT, 20, -10);
  lv_obj_set_style_bg_color(cancel_btn, lv_palette_main(LV_PALETTE_GREY), 0);
  lv_obj_set_style_radius(cancel_btn, 8, 0);

  lv_obj_add_event_cb(cancel_btn, [](lv_event_t* e) {
    lv_obj_del(lv_obj_get_parent(lv_event_get_target(e)));  // Close popup
  }, LV_EVENT_CLICKED, NULL);

  lv_obj_t* cancel_label = lv_label_create(cancel_btn);
  lv_label_set_text(cancel_label, "Cancel");
  lv_obj_center(cancel_label);
}
