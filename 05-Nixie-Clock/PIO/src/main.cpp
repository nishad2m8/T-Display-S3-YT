#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include "credential.h"


// Define a constant for the update interval in milliseconds
const unsigned long UPDATE_INTERVAL = 10000;  // 10000 milliseconds = 10 seconds

unsigned long lastUpdateTime = 0;

extern lv_obj_t *ui_img_bg;
extern const lv_img_dsc_t *ui_imgset_[10];

void connectWiFi();
void updateWorldTime();
void displayTime(int& hours, int& minutes);

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

// Function to create and display a single digit at a specific position
lv_obj_t* create_digit(const lv_img_dsc_t* img, lv_coord_t x, lv_coord_t y, lv_obj_t* parent) {
    lv_obj_t* digit = lv_img_create(parent);  // Create the digit with the specified parent
    lv_img_set_src(digit, img);
    lv_obj_set_pos(digit, x, y);

    return digit;
}

// Function to display hours at specific positions
void display_hours(int hours) {
    // Extract individual digits
    int digit1 = (hours / 10) % 10;
    int digit0 = hours % 10;

    // Display hours at the specified positions
    lv_obj_del(ui_num_3);  // Delete the existing digit
    ui_num_3 = create_digit(ui_imgset_[digit1], 14, 28, ui_Screen1);

    lv_obj_del(ui_num_2);  // Delete the existing digit
    ui_num_2 = create_digit(ui_imgset_[digit0], 88, 28, ui_Screen1);
}

// Function to display minutes at specific positions
void display_minutes(int minutes) {
    // Extract individual digits
    int digit1 = (minutes / 10) % 10;
    int digit0 = minutes % 10;

    // Display minutes at the specified positions
    lv_obj_del(ui_num_1);  // Delete the existing digit
    ui_num_1 = create_digit(ui_imgset_[digit1], 180, 28, ui_Screen1);

    lv_obj_del(ui_num_0);  // Delete the existing digit
    ui_num_0 = create_digit(ui_imgset_[digit0], 253, 28, ui_Screen1);

}

// Function to create the background image
void create_background() {
    ui_img_bg = lv_img_create(ui_Screen1);
    lv_img_set_src(ui_img_bg, &ui_img_bg); // Replace with the actual image
    lv_obj_set_pos(ui_img_bg, 0, 0);

    // Set a higher z-index for the background
    lv_obj_move_to_index(ui_img_bg, 1);

    // Bring the background to the top
    lv_obj_move_foreground(ui_img_bg);
}

void clearDisplay() {
    tft.fillScreen(TFT_BLACK); // Assuming TFT_BLACK is the background color you want
}

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

    // Clear the display before showing the UI
    clearDisplay();

    // Create background after digits, so it will be drawn on top of the digits
    create_background();

    Serial.println( "Setup done" );

    connectWiFi();

    lastUpdateTime = millis();
}

void connectWiFi() {
    Serial.println("Connecting to WiFi");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    unsigned long currentTime = millis();

    // Check if the update interval has elapsed
    if (currentTime - lastUpdateTime >= UPDATE_INTERVAL) {
        // Update the time
        updateWorldTime();

        // Update the last update time
        lastUpdateTime = currentTime;
    }
}

void updateWorldTime() {
    HTTPClient http;

    // Make a GET request to the worldtimeapi.org API
    if (http.begin("http://worldtimeapi.org/api/ip")) {
        int httpCode = http.GET();

        if (httpCode > 0) {
            // Parse the JSON response
            DynamicJsonDocument jsonDoc(1024);  // Adjust the size as needed
            deserializeJson(jsonDoc, http.getString());

            // Extract hour and minute values
            String datetime = jsonDoc["datetime"];
            int hours = datetime.substring(11, 13).toInt();
            int minutes = datetime.substring(14, 16).toInt();

            // Convert to 12-hour format
            hours = (hours % 12 == 0) ? 12 : hours % 12;

            // Display the time
            displayTime(hours, minutes);
        }

        http.end();
    }
}


void displayTime(int &hours, int &minutes)
{
    // Delete existing digits
    lv_obj_del(ui_num_0);
    lv_obj_del(ui_num_1);
    lv_obj_del(ui_num_2);
    lv_obj_del(ui_num_3);

    lv_obj_add_flag(ui_num_0, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_add_flag(ui_num_00, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_add_flag(ui_num_000, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
    lv_obj_add_flag(ui_num_0000, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);

    // Display hours at the specified positions
    int digit1 = (hours / 10) % 10;
    int digit0 = hours % 10;
    ui_num_3 = create_digit(ui_imgset_[digit1], 14, 28, ui_Screen1);
    ui_num_2 = create_digit(ui_imgset_[digit0], 88, 28, ui_Screen1);

    lv_obj_move_to_index(ui_num_3, 0);
    lv_obj_move_to_index(ui_num_2, 0);

    // Display minutes at the specified positions
    int digit1_m = (minutes / 10) % 10;
    int digit0_m = minutes % 10;
    ui_num_1 = create_digit(ui_imgset_[digit1_m], 180, 28, ui_Screen1);
    ui_num_0 = create_digit(ui_imgset_[digit0_m], 253, 28, ui_Screen1);

    lv_obj_move_to_index(ui_num_1, 0);
    lv_obj_move_to_index(ui_num_0, 0);
}
