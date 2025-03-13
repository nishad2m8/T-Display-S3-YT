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

extern const lv_img_dsc_t *ui_imgset_1862877112[13];

// Global variables
int data1 = 0; // 0-120 range
int data2 = 0; // 0-100 range
bool data1_up = true; // Direction flag for data1
bool data2_up = true; // Direction flag for data2

// Blinking variables
int blinkCounter = 0;  // Counter for blinking
lv_timer_t *blinkTimer = NULL;  // Timer handle

// Timing variables
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;
const unsigned long interval1 = 10; // Update interval for data1 (in ms)
const unsigned long interval2 = 150; // Update interval for data2 (in ms)

// Function to update data1 (0-120)
void updateData1() {
    if (data1_up) {
        data1++;
        if (data1 >= 120) {
            data1_up = false; // Change direction
        }
    } else {
        data1--;
        if (data1 <= 0) {
            data1_up = true; // Change direction
        }
    }
}

// Function to update data2 (0-100)
void updateData2() {
    if (data2_up) {
        data2++;
        if (data2 >= 100) {
            data2_up = false; // Change direction
        }
    } else {
        data2--;
        if (data2 <= 0) {
            data2_up = true; // Change direction
        }
    }
}

// Function to toggle opacity
void blinkPanels(lv_timer_t *timer) {
    static bool toggle = false;
    
    // Toggle opacity
    int opacity = toggle ? 210 : 0;
    lv_obj_set_style_bg_opa(ui_Panel_SL, opacity, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui_Panel_SR, opacity, LV_PART_MAIN | LV_STATE_DEFAULT);

    toggle = !toggle;
    blinkCounter++;

    // Stop blinking after 5 times
    if (blinkCounter >= 8) {  // 5 blinks (on/off = 10 toggles)
        lv_timer_del(timer);  // Delete timer
        blinkTimer = NULL;
        blinkCounter = 0;
    }
}

// Function to update the UI
void updateUI() {
    // Map data1 (0-120) to angle (-800 to 400)
    int angle1 = map(data1, 0, 120, -800, 400);
    lv_img_set_angle(ui_Image_needle_1, angle1);

    // Map value1 (0-120) to image index (0-12)
    int data1index = map(data1, 0, 120, 0, 12);
    lv_img_set_src(ui_Image_Speed, (const void *)ui_imgset_1862877112[data1index]);

    // Update LVGL label
    lv_label_set_text(ui_Label_speed, String(data1).c_str());

    // Map data2 (0-100) to angle (-500 to 500)
    int angle2 = map(data2, 0, 100, -500, 500);
    lv_img_set_angle(ui_Image_needle_2, angle2);

    // If data1 is 120, start blinking effect
    if (data1 == 120 && blinkTimer == NULL) {
        blinkCounter = 0;  // Reset counter
        blinkTimer = lv_timer_create(blinkPanels, 100, NULL); // 500ms interval
    }

    Serial.print("UI Updated: Needle1 Angle = ");
    Serial.print(angle1);
    Serial.print(" | Needle2 Angle = ");
    Serial.println(angle2);
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
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    unsigned long currentMillis = millis();

    // Update data1 at the defined interval
    if (currentMillis - previousMillis1 >= interval1) {
        previousMillis1 = currentMillis;
        updateData1();
    }

    // Update data2 at the defined interval
    if (currentMillis - previousMillis2 >= interval2) {
        previousMillis2 = currentMillis;
        updateData2();
    }

    // Update UI elements
    updateUI();


}
