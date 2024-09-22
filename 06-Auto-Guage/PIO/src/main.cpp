#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>

#define BUTTON_PIN_1 0

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

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_driver, lv_indev_data_t * data )
{
    uint16_t touchX = 0, touchY = 0;

    bool touched = false;//tft.getTouch( &touchX, &touchY, 600 );

    if( !touched )
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
        data->state = LV_INDEV_STATE_PR;

        /*Set the coordinates*/
        data->point.x = touchX;
        data->point.y = touchY;

        Serial.print( "Data x " );
        Serial.println( touchX );

        Serial.print( "Data y " );
        Serial.println( touchY );
    }
}

// Button interrupt and slider range update functions

bool buttonPressed = false;

void buttonInterrupt()
{
    if (digitalRead(BUTTON_PIN_1) == HIGH)
    {
        buttonPressed = true;
    }
    else
    {
        buttonPressed = false;
    }
}

extern void ui_Screen1_screen_init(void); // Assuming this initializes your screen including the slider
extern lv_obj_t* ui_slider_speed; // Make sure this is declared in a header file or as an extern

unsigned long previousMillis = 0; // Declare previousMillis as a global variable


void setup()
{
    Serial.begin( 115200 ); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    tft.begin();          /* TFT init */
    tft.setRotation( 3 ); /* Landscape orientation, flipped */

    lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init( &disp_drv );
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register( &disp_drv );

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init( &indev_drv );
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register( &indev_drv );

    ui_init();

    Serial.println( "Setup done" );

    // Initialize button interrupt
    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_1), buttonInterrupt, CHANGE);

}

void loop()
{
    lv_timer_handler(); // let the GUI do its work
    delay(5);

    static int i = 100; // Declare i as a static variable

    static int currentValue = 0; // Keep track of the current slider value
    int targetValue = buttonPressed ? 0 : 200; // Target value based on button state

    // Gradually increase or decrease the slider value
    if (currentValue < targetValue) { 
        currentValue += 5; // Increment value by 5
        if (currentValue > targetValue) {
            currentValue = targetValue; // Ensure currentValue does not exceed targetValue
        }
        lv_slider_set_value(ui_slider_speed, currentValue, LV_ANIM_OFF);
       compass1_Animation(ui_compass, 0);
    } else if (currentValue > targetValue) { 
        currentValue -= 3; // Decrement value by 5
        if (currentValue < targetValue) {
            currentValue = targetValue; // Ensure currentValue does not drop below targetValue
        }
        lv_slider_set_value(ui_slider_speed, currentValue, LV_ANIM_OFF);  // LV_ANIM_OFF
        compass2_Animation(ui_compass, 0);
    }

    // Manually update ui_lbl_speed based on the slider's current value
    // Convert the current value to a string for display
    char speedText[10]; // Ensure this buffer is large enough for your number
    snprintf(speedText, sizeof(speedText), "%d", currentValue);
    lv_label_set_text(ui_lbl_speed, speedText);

    // Switch between options of ui_roller_mode based on the value of the slider
    if (currentValue >= 0 && currentValue < 60) {
       lv_roller_set_selected(ui_roller_mode, 0, LV_ANIM_OFF);
    } else if (currentValue >= 60 && currentValue < 80) {
       lv_roller_set_selected(ui_roller_mode, 1, LV_ANIM_OFF);
    } else if (currentValue >= 80 && currentValue < 140) {
       lv_roller_set_selected(ui_roller_mode, 2, LV_ANIM_OFF);
    } else if (currentValue >= 140 && currentValue <= 200) {
       lv_roller_set_selected(ui_roller_mode, 3, LV_ANIM_OFF);
    }

    // Assuming the speed is in km/h
    float speedKmPerHour = currentValue; // currentValue is the speed from the slider
    float timeHours = 1.0 / 60.0; // 1 minute in hours
    float distanceKm = speedKmPerHour * timeHours; // Distance traveled in 1 minute

    // Add the distance traveled in this loop iteration to the total trip distance
    static float totalDistanceKm = 0;
    totalDistanceKm += distanceKm;

    // Update the label with the total distance in kilometers
    char tripText[20]; // Ensure this buffer is large enough for your number
    snprintf(tripText, sizeof(tripText), "%.0f km", totalDistanceKm);
    lv_label_set_text(ui_lbl_trip_value, tripText);

    // Decrease the battery level
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 10000) {
        previousMillis = currentMillis;
        if (i > 0) {
            i -= i / 10; // Decrease the battery level by 10% of the current level
        }
        lv_bar_set_value(ui_bar_battery, i, LV_ANIM_ON);
        char batteryText[10]; // Ensure this buffer is large enough for your number
        snprintf(batteryText, sizeof(batteryText), "%d%%", i);
        lv_label_set_text(ui_lbl_battery_value, batteryText);
        lv_bar_set_value(ui_bar_battery, i, LV_ANIM_ON); 

    }

    // Calculate the range based on the battery percentage
    float rangeKm = i * 4.82; // 482 km for 100% battery
    char rangeText[10]; // Ensure this buffer is large enough for your number
    snprintf(rangeText, sizeof(rangeText), "%.0f", rangeKm);
    lv_label_set_text(ui_lbl_range, rangeText);

    // Map the battery level to the range
    int rangeLevel = map(i, 0, 100, 0, 70);
    lv_slider_set_value(ui_slider_range, rangeLevel, LV_ANIM_ON); 
}
