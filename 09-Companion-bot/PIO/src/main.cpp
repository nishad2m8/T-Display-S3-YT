#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>

const int BUTTON_PIN_1 = 0;              // Button 1 pin
const int BUTTON_PIN_2 = 14;             // Button 2 pin
const unsigned long debounceDelay = 100; // Debounce delay

/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 170;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

#if LV_USE_LOG != 1 // 0 is false, 1 is true
/* Serial debugging */
void my_print(const char *buf)
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
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

enum AnimationState
{
    ANGRY,
    LOVE,
    SAD,
    SLEEPY,
    AWE,
    BLINK // Removed IDLE, using BLINK as default
};

AnimationState currentState = BLINK; // Default to BLINK
volatile bool buttonPressed = false;

void updateUIForCurrentState()
{
    lv_obj_add_flag(ui_blink, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_angry, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_love, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_sad, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_sleepy, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(ui_awe, LV_OBJ_FLAG_HIDDEN);

    switch (currentState)
    {
    case ANGRY:
        lv_obj_clear_flag(ui_angry, LV_OBJ_FLAG_HIDDEN);
        break;
    case LOVE:
        lv_obj_clear_flag(ui_love, LV_OBJ_FLAG_HIDDEN);
        break;
    case SAD:
        lv_obj_clear_flag(ui_sad, LV_OBJ_FLAG_HIDDEN);
        break;
    case SLEEPY:
        lv_obj_clear_flag(ui_sleepy, LV_OBJ_FLAG_HIDDEN);
        break;
    case AWE:
        lv_obj_clear_flag(ui_awe, LV_OBJ_FLAG_HIDDEN);
        break;
    case BLINK:
    default:
        lv_obj_clear_flag(ui_blink, LV_OBJ_FLAG_HIDDEN);
        break;
    }
}

// The previously defined function to schedule the return to BLINK
static void returnToBlinkAnimation(lv_timer_t *timer)
{
    currentState = BLINK;
    updateUIForCurrentState();
}

void scheduleReturnToBlink()
{
    // Ensure any previous timer is cancelled before creating a new one
    static lv_timer_t *blinkTimer = nullptr;
    if (blinkTimer)
    {
        lv_timer_del(blinkTimer);
        blinkTimer = nullptr;
    }
    blinkTimer = lv_timer_create(returnToBlinkAnimation, 2000, NULL); // 5000 ms delay
}

void handleAnimation()
{
    // Ensures UI elements are updated based on the current state
    updateUIForCurrentState();

    Serial.println("Animation triggered.");
    switch (currentState)
    {
    case ANGRY:
        Serial.println("Angry animation triggered.");
        angry_Animation(ui_angry, 0);
        break;
    case LOVE:
        Serial.println("Love animation triggered.");
        love_Animation(ui_love, 0);
        break;
    case SAD:
        Serial.println("Sad animation triggered.");
        sad_Animation(ui_sad, 0);
        break;
    case SLEEPY:
        Serial.println("Sleepy animation triggered.");
        sleepy_Animation(ui_sleepy, 0);
        break;
    case AWE:
        Serial.println("Awe animation triggered.");
        awe_Animation(ui_awe, 0);
        break;
    case BLINK:
    default:
        Serial.println("Blink animation triggered.");
        blink_Animation(ui_blink, 0);
        // No need to schedule return to BLINK, as it's the default state
        return; // Exit early to avoid scheduling return to BLINK
    }

    // Schedule return to BLINK after showing any animation other than BLINK
    scheduleReturnToBlink();
}

// New global variable to track which animations have been shown
bool animationShown[] = {false, false, false, false, false, false}; // Corresponds to ANGRY, LOVE, SAD, SLEEPY, AWE, BLINK

void resetAnimationShown()
{
    for (int i = 0; i < sizeof(animationShown) / sizeof(animationShown[0]); ++i)
    {
        animationShown[i] = false;
    }
    // Ensure BLINK is always available
    animationShown[BLINK] = true;
}

// Update currentState cycling in buttonInterrupt
void buttonInterrupt()
{
    static unsigned long lastInterruptTime = 0;
    unsigned long interruptTime = millis();
    if (interruptTime - lastInterruptTime > debounceDelay)
    {
        lastInterruptTime = interruptTime;

        int nextAnimation = (currentState + 1) % (BLINK + 1);
        // Find the next unshown animation
        while (animationShown[nextAnimation])
        {
            nextAnimation = (nextAnimation + 1) % (BLINK + 1);
        }

        currentState = static_cast<AnimationState>(nextAnimation);
        animationShown[currentState] = true; // Mark this animation as shown

        // Check if all animations have been shown
        bool allShown = true;
        for (int i = 0; i < BLINK; ++i)
        { // Only check until the last animation before BLINK
            if (!animationShown[i])
            {
                allShown = false;
                break;
            }
        }

        // If all animations have been shown, reset for the next cycle
        if (allShown)
        {
            resetAnimationShown();
        }

        buttonPressed = true;
    }
}

void triggerSpecificAnimation(AnimationState animation)
{
    currentState = animation;
    animationShown[currentState] = true; // Optionally mark this animation as shown if using tracking logic

    // Directly update UI for the current state and handle the animation
    updateUIForCurrentState();
    handleAnimation();
}

void setup()
{
    Serial.begin(115200); /* prepare for possible serial debug */

    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println(LVGL_Arduino);
    Serial.println("I am LVGL_Arduino");

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print); /* register print function for debugging */
#endif

    tft.begin();        /* TFT init */
    tft.setRotation(3); /* Landscape orientation, flipped */

    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    /*Initialize the display*/
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    /*Change the following line to your display resolution*/
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /*Initialize the (dummy) input device driver*/
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    lv_indev_drv_register(&indev_drv);

    ui_init();

    pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP);

    // Attach interrupt to the button pin
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_1), buttonInterrupt, FALLING);

    currentState = BLINK;
    handleAnimation(); // Initialize with the blink animation
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    static unsigned long lastDebounceTime = 0; // Last time the button pin was toggled
    static int buttonState = HIGH;             // Current state of the button
    int reading = digitalRead(BUTTON_PIN_1);   // Read the state of the button pin

    // Check if the button state has changed
    if (reading != buttonState)
    {
        lastDebounceTime = millis(); // Reset the debounce timer
    }

    if (buttonPressed)
    {
        buttonPressed = false; // Reset the button press flag
        handleAnimation();     // Handle the animation based on the updated state
    }

    // New logic to check BUTTON_PIN_2 state and trigger SLEEPY animation
    static unsigned long lastDebounceTimeButton2 = 0; // Last time the button pin was toggled
    int readingButton2 = digitalRead(BUTTON_PIN_2);   // Read the state of the button pin

    // Checking specific animation
    if (readingButton2 == LOW && (millis() - lastDebounceTimeButton2) > debounceDelay)
    {
        lastDebounceTimeButton2 = millis(); // Update the debounce time

        // Call specific animation
        triggerSpecificAnimation(SLEEPY);
    }
}