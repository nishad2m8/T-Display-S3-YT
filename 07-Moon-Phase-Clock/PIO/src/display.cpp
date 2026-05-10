#include "display.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

namespace Display
{
    static TFT_eSPI tft;
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf[Width * Height / 10];

#if LV_USE_LOG != 0
    static void lvgl_print(const char *msg)
    {
        Serial.printf(msg);
        Serial.flush();
    }
#endif

    static void flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
    {
        uint32_t w = (area->x2 - area->x1 + 1);
        uint32_t h = (area->y2 - area->y1 + 1);

        tft.startWrite();
        tft.setAddrWindow(area->x1, area->y1, w, h);
        tft.pushColors((uint16_t *)&color_p->full, w * h, true);
        tft.endWrite();

        lv_disp_flush_ready(disp);
    }

    void begin()
    {
        lv_init();

#if LV_USE_LOG != 0
        lv_log_register_print_cb(lvgl_print);
#endif

        tft.begin();
        tft.setRotation(3); // Landscape, flipped

        lv_disp_draw_buf_init(&draw_buf, buf, NULL, Width * Height / 10);

        // Display driver
        static lv_disp_drv_t disp_drv;
        lv_disp_drv_init(&disp_drv);
        disp_drv.hor_res  = Width;
        disp_drv.ver_res  = Height;
        disp_drv.flush_cb = flush;
        disp_drv.draw_buf = &draw_buf;
        lv_disp_drv_register(&disp_drv);

        // Dummy input device
        static lv_indev_drv_t indev_drv;
        lv_indev_drv_init(&indev_drv);
        indev_drv.type = LV_INDEV_TYPE_POINTER;
        lv_indev_drv_register(&indev_drv);
    }
}
