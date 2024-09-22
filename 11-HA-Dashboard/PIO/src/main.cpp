#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <Bounce2.h>

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

// Define button pins
#define BUTTON1_PIN 0 // Change this to your Button 1 GPIO pin
#define BUTTON2_PIN 14 // Change this to your Button 2 GPIO pin

/* States and mode selection */
enum Mode {
    AC_MODE,
    LIGHT_MODE,
    BLIND_MODE
};

Mode currentMode = AC_MODE; // Start in AC_MODE
Mode previousMode = AC_MODE; // To track the previous mode for animations

/* Slider values */
int acSetting = 22; // Range 19-25 for AC
int lightLevel = 50; // Range 0-100 for light
int blindPosition = 10; // Range 0-100 for blind


/* Bounce instances for button debouncing */
Bounce button1;
Bounce button2;

/* Timing constants */
const unsigned long longPressDuration = 1000; // 1 second for long press
unsigned long button2PressStart = 0; // Track press start time

// Function to animate the x position
void animateXPosition(lv_obj_t* obj, int32_t start, int32_t end, uint32_t duration) {
    lv_anim_t anim; // Declare an animation object
    lv_anim_init(&anim); // Initialize the animation object
    lv_anim_set_var(&anim, obj); // Set the object to animate

    // Animation from 'start' to 'end' over 'duration' milliseconds
    lv_anim_set_values(&anim, start, end); // Set start and end values
    lv_anim_set_time(&anim, duration); // Set the duration of the animation

    // Set the function that changes the 'x' position
    lv_anim_set_exec_cb(&anim, (lv_anim_exec_xcb_t) lv_obj_set_x); // Execute by setting 'x'

    // Start the animation
    lv_anim_start(&anim); // Start the animation
}

// Example usage for switching modes
void switchModeAnimation(Mode newMode) {
    int32_t newX = 0; // Default x position

    // Determine the target x position based on the new mode
    switch (newMode) {
        case AC_MODE:
            newX = 0; // x position for AC mode
            break;
        case LIGHT_MODE:
            newX = 38; // x position for light mode
            break;
        case BLIND_MODE:
            newX = 76; // x position for blind mode
            break;
        default:
            break;
    }

    // Animate the 'x' position to the new target
    animateXPosition(ui_Image_iconactive, lv_obj_get_x(ui_Image_iconactive), newX, 500); // 500ms duration
}

// Function to map a value to a y-position based on mode range
int mapToYPosition(int value, int minValue, int maxValue) {
    int yMin = 110; // Bottom position
    int yMax = 30;  // Top position
    // Linear interpolation from [minValue, maxValue] to [yMin, yMax]
    return yMin + (yMax - yMin) * (value - minValue) / (maxValue - minValue);
}

// Function to map a value to a specific range
int mapToRange(int value, int minValue, int maxValue, int rangeMin, int rangeMax) {
    // Linear interpolation from [minValue, maxValue] to [rangeMin, rangeMax]
    return rangeMin + (rangeMax - rangeMin) * (value - minValue) / (maxValue - minValue);
}

// Function to update additional elements based on the current mode
void updateAdditionalElements() {
    switch (currentMode) {
        case AC_MODE:
            // Set the y-position of ui_Image_wind based on acSetting (19-25)
            lv_obj_set_y(ui_Image_wind, mapToRange(acSetting, 19, 25, -5, 33));
            break;
        case LIGHT_MODE:
            // Set the opacity of ui_Image_lux based on lightLevel (0-100)
            lv_obj_set_style_opa(ui_Image_lux, mapToRange(lightLevel, 0, 100, 0, 255), LV_PART_MAIN);
            break;
        case BLIND_MODE:
            // Set the y-position of ui_Image_blindpos based on blindPosition (0-100)
            lv_obj_set_y(ui_Image_blindpos, mapToRange(blindPosition, 0, 100, -10, -140));
            blink_Animation(ui_Image_ea, 0);
            break;
        default:
            break;
    }
}

// Function to update slider knob position and associated label
void updateSliderKnobPosition() {
    int yPos = 110; // Default to bottom
    int sliderValue = 0; // The value to set on the label

    switch (currentMode) {
        case AC_MODE:
            sliderValue = acSetting; // AC setting is in range 19-25
            yPos = mapToYPosition(acSetting, 19, 25); // Map to y-position
            lv_label_set_text(ui_label_set_unit, "°C");
            break;
        case LIGHT_MODE:
            sliderValue = lightLevel; // Light level is in range 0-100
            yPos = mapToYPosition(lightLevel, 0, 100); // Map to y-position
            lv_label_set_text(ui_label_set_unit, "%");
            break;
        case BLIND_MODE:
            sliderValue = blindPosition; // Blind position is in range 0-100
            yPos = mapToYPosition(blindPosition, 0, 100); // Map to y-position
            lv_label_set_text(ui_label_set_unit, "%");
            break;
        default:
            break;
    }

    // Update the y-position of the slider knob
    lv_obj_set_y(ui_Image_sliderknob, yPos);

    // Update the label with the current slider value
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%d", sliderValue);
    lv_label_set_text(ui_label_set_value, buffer); // Update the displayed value

    // Update additional elements based on mode
    updateAdditionalElements(); // Update wind, lux, and blind position based on current mode
}


/* Function to play "out" animation for the previous container */
void playOutAnimation(Mode mode) {
    switch (mode) {
        case AC_MODE:
            out_Animation(ui_Container_ac, 0); // Play "out" animation for AC container
            break;
        case LIGHT_MODE:
            out_Animation(ui_Container_lamp, 0); // Play "out" animation for light container
            break;
        case BLIND_MODE:
            out_Animation(ui_Container_blind, 0); // Play "out" animation for blind container
            break;
        default:
            break;
    }
}

/* Function to play "in" animation for the current container and set icon properties */
void playInAnimation(Mode mode) {
    switch (mode) {
        case AC_MODE:
            in_Animation(ui_Container_ac, 500); // Play "in" animation for AC container
            lv_obj_set_x(ui_Image_iconactive, 0); // Move icon to x=0
            updateSliderKnobPosition(); // Update the slider knob position
            // Set the active icon's opacity to 255, others to 0
            lv_obj_set_style_img_recolor_opa(ui_icon_ac, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_img_recolor_opa(ui_icon_lamp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_img_recolor_opa(ui_icon_blind, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            break;
        case LIGHT_MODE:
            in_Animation(ui_Container_lamp, 500); // Play "in" animation for light container
            lv_obj_set_x(ui_Image_iconactive, 38); // Move icon to x=38
            updateSliderKnobPosition(); // Update the slider knob position
            // Set the active icon's opacity to 255, others to 0
            lv_obj_set_style_img_recolor_opa(ui_icon_lamp, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_img_recolor_opa(ui_icon_ac, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_img_recolor_opa(ui_icon_blind, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            break;
        case BLIND_MODE:
            in_Animation(ui_Container_blind, 500); // Play "in" animation for blind container
            lv_obj_set_x(ui_Image_iconactive, 76); // Move icon to x=76
            updateSliderKnobPosition(); // Update the slider knob position
            // Set the active icon's opacity to 255, others to 0
            lv_obj_set_style_img_recolor_opa(ui_icon_blind, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_img_recolor_opa(ui_icon_lamp, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_img_recolor_opa(ui_icon_ac, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            break;
        default:
            break;
    }
}

// Handle button 2 for decrementing slider values and mode switching
void checkButton2() {
    button2.update(); // Update button state
    
    if (button2.fell()) { // Button press detected
        button2PressStart = millis(); // Record the press start time
        
        // Decrement logic with boundary checks
        switch (currentMode) {
            case AC_MODE:
                if (acSetting > 19) { // Check minimum limit
                    acSetting -= 1; // Decrement by 1
                    Serial.print("AC Setting: ");
                    Serial.println(acSetting);
                }
                break;
            case LIGHT_MODE:
                if (lightLevel > 0) { // Check minimum limit
                    lightLevel -= 10; // Decrement by 5
                    Serial.print("Light Level: ");
                    Serial.println(lightLevel);
                }
                break;
            case BLIND_MODE:
                if (blindPosition > 0) { // Check minimum limit
                    blindPosition -= 10;
                    Serial.print("Blind Position: ");
                    Serial.println(blindPosition);
                }
                break;
            default:
                break;
        }

        updateSliderKnobPosition(); // Update the slider knob position
    }

    if (button2.rose()) { // Button released
        unsigned long pressDuration = millis() - button2PressStart;

        if (pressDuration >= longPressDuration) { // Long press detected
            previousMode = currentMode; // Remember previous mode
            currentMode = (Mode)((currentMode + 1) % 3); // Cycle to the next mode
            
            // Play the "out" animation for the previous mode
            playOutAnimation(previousMode);

            // Play the "in" animation for the new mode
            playInAnimation(currentMode);

            // Apply the correct x position with animation
            switchModeAnimation(currentMode);

            // Update the slider knob position for the new mode
            updateSliderKnobPosition(); // Recalculate the y-position
        }
    }
}

// Handle button 1 for incrementing slider values based on the current mode
void checkButton1() {
    button1.update(); // Update the button state
    
    if (button1.fell()) { // Button press detected
        Serial.println("Button 1 pressed. Incrementing in current mode.");

        // Increment logic with boundary checks
        switch (currentMode) {
            case AC_MODE:
                if (acSetting < 25) { // Check maximum limit
                    acSetting += 1; // Increment by 1
                    Serial.print("AC Setting: ");
                    Serial.println(acSetting);
                }
                break;
            case LIGHT_MODE:
                if (lightLevel < 100) { // Check maximum limit
                    lightLevel += 10; // Increment by 10
                    Serial.print("Light Level: ");
                    Serial.println(lightLevel);
                }
                break;
            case BLIND_MODE:
                if (blindPosition < 100) { // Check maximum limit
                    blindPosition += 10; 
                    Serial.print("Blind Position: ");
                    Serial.println(blindPosition);
                }
                break;
            default:
                break;
        }

        updateSliderKnobPosition(); // Update the slider knob position
    }
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

    /* Initialize buttons with bounce */
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    button1.attach(BUTTON1_PIN);
    button1.interval(25); // Debounce interval
    button2.attach(BUTTON2_PIN);
    button2.interval(25); // Debounce interval

    // Play in animation for the initial mode (AC_MODE)
    playInAnimation(AC_MODE); // Ensure AC mode is active initially
    lv_obj_set_style_opa(ui_Image_ea, 0, LV_PART_MAIN);
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    checkButton1(); // Handle Button 1 press
    checkButton2(); // Handle Button 2 press
}
