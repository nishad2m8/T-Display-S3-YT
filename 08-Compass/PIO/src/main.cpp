#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 170;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

#if LV_USE_LOG != 1 // 0 is false, 1 is true
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

lv_obj_t *ui_TextArea1;

// Global variables to hold the compass reading string and direction
int compassReading;
char compassReadingStr[6]; // Need extra space for the degree symbol and space
const char* direction;

// Global variable for the random ID timer
lv_timer_t *randomIDTimer;

// Function to generate a random ID number
int generateRandomID() {
    return rand() % 1000; // Generate a random number between 0 and 999
}


// Function to update compass position based on the compass reading
void updateCompassPosition(lv_timer_t *timer) {

    // Generate a random compass reading (0 to 359 degrees)
    compassReading = random(0, 360);

    // Increment the compass reading by a small amount
    // compassReading += 1; 

    // Ensure that compassReading stays within the range of 0 to 359 degrees
    if (compassReading >= 360) {
        compassReading -= 360;
    }

    // Calculate the new X position based on the compass reading
    int newPos = -70 - (compassReading / 5) * 10; // Adjusted based on new zero location and width
    int nextPos = -70 - ((compassReading + 5) / 5) * 10; // Adjusted based on new zero location and width
    int newXPos = newPos + ((nextPos - newPos) * (compassReading % 5) / 5);

    // Convert the compass reading to a string
    char compassReadingStr[6]; // Need extra space for the degree symbol and space
    sprintf(compassReadingStr, "%d°", compassReading); // Add degree symbol and space

    // Update the label to display the compass reading
    lv_label_set_text(ui_lbl_degree, compassReadingStr);

    // Determine the direction based on the compass reading
    const char* directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    int sector = (compassReading + 22) / 45;
    const char* direction = directions[sector % 8];

    // Update the label to display the direction
    lv_label_set_text(ui_lbl_direction, direction);

    Serial.print("Compass reading: ");
    Serial.println(compassReading);
    Serial.print("New X position: ");
    Serial.println(newXPos);

    // Smoothly animate the transition to the new X position
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, ui_compass);
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t)lv_obj_set_x);
    lv_anim_set_values(&anim, lv_obj_get_x(ui_compass), newXPos);
    lv_anim_set_time(&anim, 1000); // Animation duration: 1000 milliseconds
    lv_anim_set_path_cb(&anim, lv_anim_path_ease_in_out); //  lv_anim_path_linear // lv_anim_path_overshoot // lv_anim_path_bounce // lv_anim_path_ease_in_out
    lv_anim_start(&anim);
}

// Function to handle the random ID timer
void randomIDTimerHandler(lv_timer_t *timer) {
    // Generate a random ID number
    int randomID = generateRandomID();

    // Convert the compass reading to a string
    char compassReadingStr[6]; // Need extra space for the degree symbol and space
    sprintf(compassReadingStr, "%d°", compassReading); // Add degree symbol and space

    // Determine the direction based on the compass reading
    const char* directions[] = {"N", "NE", "E", "SE", "S", "SW", "W", "NW"};
    int sector = (compassReading + 22) / 45;
    const char* direction = directions[sector % 8];

    // Concatenate compass reading, degree symbol, and direction
    char mockData[100]; // Adjust size as needed
    sprintf(mockData, "ID:%d - %s %s\n", randomID, compassReadingStr, direction);

    // Update the TextArea to display the mock data
    lv_textarea_add_text(ui_TextArea1, mockData);
    
    Serial.print("Mock data: ");
    Serial.println(mockData);
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
    lv_indev_drv_register( &indev_drv );


    ui_init();

    Serial.println( "Setup done" );

    // Set up a timer to update the compass position every 5 seconds
    lv_timer_create(updateCompassPosition, 5000, NULL);

    // Create the random ID timer to trigger every 15 seconds
    randomIDTimer = lv_timer_create(randomIDTimerHandler, 15000, NULL);
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);
}
