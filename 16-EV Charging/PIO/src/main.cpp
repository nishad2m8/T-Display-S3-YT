#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>

// https://www.youtube.com/nishad2m8
// https://buymeacoffee.com/nishad2m8

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 170;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp );
}

// Battery bar progress counter
static int battery_progress = 0;

// Update the battery bar
void update_battery_bar(lv_obj_t *bar) {
    lv_bar_set_value(bar, battery_progress, LV_ANIM_ON); // Update bar with animation

    // Increment the progress and reset if it exceeds 100
    battery_progress = (battery_progress + 1) % 101;
}

// Initialize and start periodic updates for the battery bar
void start_battery_bar_update(lv_obj_t *bar) {
    // Create a periodic task to update the bar
    lv_timer_create(
        [](lv_timer_t *timer) {
            lv_obj_t *bar = (lv_obj_t *)timer->user_data;
            update_battery_bar(bar);
        },
        100, // Update every 100ms for smooth progress
        bar   // Pass the bar object as user data
    );
}

void setup() {
    Serial.begin(115200);

    lv_init();

    #if LV_USE_LOG != 0
        lv_log_register_print_cb(my_print);
    #endif

    tft.begin();
    // Set the orientation of the TFT display
    //   0: Home button down (portrait mode) - 170x320 resolution
    //   1: Rotate clockwise to landscape mode - 320x170 resolution
    //   2: Home button up (upside-down portrait mode)
    //   3: Rotate counterclockwise to landscape mode
    tft.setRotation(1); // Set the display to default counterclockwise to landscape mode

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);

    ui_init();
    Serial.println("Setup done");

    // Start the gradual battery bar updates
    start_battery_bar_update(ui_Bar_Battery);
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
}
