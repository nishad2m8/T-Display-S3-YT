#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>

// https://www.youtube.com/nishad2m8
// https://buymeacoffee.com/nishad2m8

/* Change to your screen resolution */
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 170;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

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
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)color_p, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void update_pitch(lv_obj_t *ui_img_pitch_scale, lv_obj_t *ui_Label_pitch, int pitch_value, int pitch_rotation)
{
    // Calculate the rotation angle in degrees for pitch
    int rotation_angle = pitch_rotation / 10; // Each 100 units corresponds to 10 degrees
    lv_img_set_angle(ui_img_pitch_scale, rotation_angle * 10); // LVGL uses 0.1 degree units

    // Calculate the Y position
    int y_position = pitch_value * 2; // Each 10 pixels corresponds to 5 units
    lv_obj_set_y(ui_img_pitch_scale, y_position);

    // Update the pitch label with the current pitch value
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", pitch_value);
    lv_label_set_text(ui_Label_pitch, buf);
}

void update_roll(lv_obj_t *ui_img_roll_scale, lv_obj_t *ui_Label_roll, int roll_value)
{
    // Calculate the rotation angle in degrees
    int rotation_angle = roll_value; // Each unit corresponds to 1 degree
    lv_img_set_angle(ui_img_roll_scale, rotation_angle * 10); // LVGL uses 0.1 degree units

    // Display positive degree on the right and negative degree on the left
    int display_angle = -roll_value; // Invert the roll value for display

    // Update the roll label with the current rotation angle in degrees
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", display_angle);
    lv_label_set_text(ui_Label_roll, buf);
}

void update_pitch_and_roll(lv_obj_t *ui_img_pitch_scale, lv_obj_t *ui_img_roll_scale, lv_obj_t *ui_Label_pitch, lv_obj_t *ui_Label_roll, int pitch_value, int pitch_rotation, int roll_value)
{
    update_pitch(ui_img_pitch_scale, ui_Label_pitch, pitch_value, pitch_rotation);
    update_roll(ui_img_roll_scale, ui_Label_roll, roll_value);
}

void mock_data(int &pitch_value, int &pitch_rotation, int &roll_value, int speed_factor)
{
    static bool increasing_pitch = true;
    static bool increasing_pitch_rotation = true;
    static bool increasing_roll = true;

    // Update pitch
    if (increasing_pitch)
    {
        pitch_value += speed_factor;
        if (pitch_value >= 20) { // 20 units is the max pitch value
            increasing_pitch = false;
        }
    }
    else
    {
        pitch_value -= speed_factor;
        if (pitch_value <= -20) { // -20 units is the min pitch value
            increasing_pitch = true;
        }
    }

    // Update pitch rotation
    if (increasing_pitch_rotation)
    {
        pitch_rotation += 10 * speed_factor;
        if (pitch_rotation >= 900) { // 900 units is the max pitch rotation
            increasing_pitch_rotation = false;
        }
    }
    else
    {
        pitch_rotation -= 10 * speed_factor;
        if (pitch_rotation <= -900) { // -900 units is the min pitch rotation
            increasing_pitch_rotation = true;
        }
    }

    // Update roll
    if (increasing_roll)
    {
        roll_value -= speed_factor; // Decrease roll value for right rotation
        if (roll_value <= -90) { // -90 degrees is the max right roll value
            increasing_roll = false;
        }
    }
    else
    {
        roll_value += speed_factor; // Increase roll value for left rotation
        if (roll_value >= 90) { // 90 degrees is the max left roll value
            increasing_roll = true;
        }
    }
}

void setup()
{
    Serial.begin(115200);

    lv_init();

    #if LV_USE_LOG != 0
        lv_log_register_print_cb(my_print);
    #endif

    tft.begin();
    tft.setRotation(3); // Set the display to default counterclockwise to landscape mode

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

    // Initialize with level pitch, pitch rotation, and no roll
    int pitch_value = 0;
    int pitch_rotation = 0;
    int roll_value = 0;

    update_pitch_and_roll(ui_img_pitch_scale, ui_img_roll_scale, ui_Label__pitch, ui_Label_roll, pitch_value, pitch_rotation, roll_value);
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    // Example updates for testing
    static int pitch_value = 0;
    static int pitch_rotation = 0;
    static int roll_value = 0;

    // Simulate pitch and roll changes
    int speed_factor = 1; // Adjust this value to control speed of mock data generation
    mock_data(pitch_value, pitch_rotation, roll_value, speed_factor);

    update_pitch_and_roll(ui_img_pitch_scale, ui_img_roll_scale, ui_Label__pitch, ui_Label_roll, pitch_value, pitch_rotation, roll_value);
}
