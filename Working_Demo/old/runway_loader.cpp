#include <Arduino.h>
#include <lvgl.h>

extern lv_obj_t* runway_img;

void load_runway_overlay(const char* code, int angle_deg) {
  if (!runway_img) return;

  char path[64];
  snprintf(path, sizeof(path), "S:/runways/%s.png", code);

  lv_fs_file_t f;
  lv_fs_res_t res = lv_fs_open(&f, path, LV_FS_MODE_RD);
  if (res != LV_FS_RES_OK) {
    Serial.printf("Runway overlay not found: %s\n", path);
    return;
  }
  lv_fs_close(&f);

  Serial.printf("LVGL loading: %s\n", path);
  lv_img_set_src(runway_img, path);
  lv_img_set_angle(runway_img, angle_deg * 10);
}
