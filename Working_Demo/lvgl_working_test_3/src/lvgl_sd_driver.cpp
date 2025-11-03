#include "src/lvgl_sd_driver.h"
#include <SD.h>
#include <lvgl.h>

void register_lvgl_sd_driver() {
    _lv_fs_init();  // Ensure LVGL's file system core is initialized

    static lv_fs_drv_t drv;  // Must be static to persist after registration
    lv_fs_drv_init(&drv);

    drv.letter = 'S';         // Use "S:" prefix in LVGL paths
    drv.cache_size = 0;       // No internal caching

    // Open file
    drv.open_cb = [](lv_fs_drv_t* drv, const char* path, lv_fs_mode_t mode) -> void* {
        File f = SD.open(path, mode == LV_FS_MODE_WR ? FILE_WRITE : FILE_READ);
        if (!f) return nullptr;
        return new File(f);  // Copy into heap
    };

    // Close file
    drv.close_cb = [](lv_fs_drv_t* drv, void* file_p) -> lv_fs_res_t {
        File* f = static_cast<File*>(file_p);
        if (f) {
            f->close();
            delete f;
        }
        return LV_FS_RES_OK;
    };

    // Read from file
    drv.read_cb = [](lv_fs_drv_t* drv, void* file_p, void* buf, uint32_t btr, uint32_t* br) -> lv_fs_res_t {
        File* f = static_cast<File*>(file_p);
        if (!f || !f->available()) {
            *br = 0;
            return LV_FS_RES_UNKNOWN;
        }
        *br = f->read((uint8_t*)buf, btr);
        return LV_FS_RES_OK;
    };

    // Write to file
    drv.write_cb = [](lv_fs_drv_t* drv, void* file_p, const void* buf, uint32_t btw, uint32_t* bw) -> lv_fs_res_t {
        File* f = static_cast<File*>(file_p);
        if (!f) {
            *bw = 0;
            return LV_FS_RES_UNKNOWN;
        }
        *bw = f->write((const uint8_t*)buf, btw);
        return LV_FS_RES_OK;
    };

    // Seek within file
    drv.seek_cb = [](lv_fs_drv_t* drv, void* file_p, uint32_t pos, lv_fs_whence_t whence) -> lv_fs_res_t {
        File* f = static_cast<File*>(file_p);
        if (!f) return LV_FS_RES_UNKNOWN;
        if (whence == LV_FS_SEEK_SET) f->seek(pos, SeekSet);
        else if (whence == LV_FS_SEEK_CUR) f->seek(f->position() + pos, SeekSet);
        else if (whence == LV_FS_SEEK_END) f->seek(f->size() - pos, SeekSet);
        return LV_FS_RES_OK;
    };

    // Tell current position
    drv.tell_cb = [](lv_fs_drv_t* drv, void* file_p, uint32_t* pos_p) -> lv_fs_res_t {
        File* f = static_cast<File*>(file_p);
        if (!f) {
            *pos_p = 0;
            return LV_FS_RES_UNKNOWN;
        }
        *pos_p = f->position();
        return LV_FS_RES_OK;
    };

    lv_fs_drv_register(&drv);  // Final step: register with LVGL
}
