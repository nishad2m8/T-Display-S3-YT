#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>
#include <Arduino.h>
#include <Ticker.h>
#include <Bounce2.h>

/*Change to your screen resolution*/
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 170;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

#if LV_USE_LOG != 0
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

// Timing and cycle configurations
#define FOCUS_DURATION 25 * 60 * 1000      // 25 minutes
#define BREAK_DURATION_SHORT 5 * 60 * 1000 // 5 minutes
#define BREAK_DURATION_LONG 15 * 60 * 1000 // 15 minutes
#define CYCLES_BEFORE_LONG_BREAK 4         // 4 cycles before long break

// Button setup
Bounce button1 = Bounce(0, 5);  // Start/Pause button
Bounce button2 = Bounce(14, 5); // Reset button

// Timer states
unsigned long endTime = 0;
unsigned long lastUpdateTime = 0; // Track last update time for serial debug
unsigned long remainingTime = 0;  // Track remaining time when paused
bool isFocusTime = false;         // true if focus time, false if break time
bool isPaused = true;             // true if timer is paused false if running
int cycleCount = 0;

// Function declarations
void startFocus();
void startBreak();
void updateTimer();
void updateUILabels();
void handleButtons();

lv_anim_t focusAnimation; // Global animation object
lv_anim_t breakAnimation; // Global animation object for break

void startFocusAnimation(long remainingTime)
{
  int seconds = (remainingTime % 60000) / 1000;
  int startDegrees = seconds * 6;

  lv_anim_init(&focusAnimation);
  lv_anim_set_var(&focusAnimation, ui_img_focus_sec);
  lv_anim_set_exec_cb(&focusAnimation, (lv_anim_exec_xcb_t)lv_img_set_angle);
  lv_anim_set_values(&focusAnimation, startDegrees * 10, 0);
  lv_anim_set_time(&focusAnimation, remainingTime);
  lv_anim_set_playback_time(&focusAnimation, 0);
  lv_anim_set_repeat_count(&focusAnimation, 1);

  lv_anim_start(&focusAnimation);
}

void startBreakAnimation(long remainingTime)
{
  int seconds = (remainingTime % 60000) / 1000;
  int startDegrees = seconds * 6;

  lv_anim_init(&breakAnimation);
  lv_anim_set_var(&breakAnimation, ui_img_break_sec);
  lv_anim_set_exec_cb(&breakAnimation, (lv_anim_exec_xcb_t)lv_img_set_angle);
  lv_anim_set_values(&breakAnimation, startDegrees * 10, 0);
  lv_anim_set_time(&breakAnimation, remainingTime);
  lv_anim_set_playback_time(&breakAnimation, 0);
  lv_anim_set_repeat_count(&breakAnimation, 1);

  lv_anim_start(&breakAnimation);
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
  Serial.println("Setup done");

  button1.attach(0, INPUT_PULLUP); // Setup correct button pin
  button2.attach(14, INPUT_PULLUP);

  // Start the first focus session UI update
  char focusLabel[4];
  sprintf(focusLabel, "%d", FOCUS_DURATION / 60000); // Convert milliseconds to minutes and format as string
  lv_label_set_text(ui_label_focus, focusLabel);

  char breakLabel[4];
  sprintf(breakLabel, "%d", BREAK_DURATION_SHORT / 60000); // Convert milliseconds to minutes and format as string
  lv_label_set_text(ui_label_break, breakLabel);

  // Setup for lv_label
  char cycleString[10];                                   // Buffer to hold the cycle count string
  sprintf(cycleString, "0/%d", CYCLES_BEFORE_LONG_BREAK); // Format cycle count as 0/2
  lv_label_set_text(ui_label_cycle, cycleString);         // Update the cycle count label in the setup function
}

void loop()
{
  lv_timer_handler(); // Let LVGL handle its work
  delay(5);           // Delay to control the loop frequency

  handleButtons();

  if (!isPaused)
  {
    updateTimer();

    // Serial debug update every second
    if (millis() - lastUpdateTime >= 1000)
    {
      lastUpdateTime = millis();
      long remainingTime = endTime - millis();
      if (remainingTime < 0)
        remainingTime = 0;

      int remainingMinutes = remainingTime / 60000;
      int remainingSeconds = (remainingTime % 60000) / 1000;

      // Corrected function calls with the required parameter
      if (isFocusTime)
      {
        startFocusAnimation(remainingTime); // Pass remaining time to animation function
        lv_obj_add_flag(ui_img_break_sec, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(ui_img_focus_sec, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
      }
      else
      {
        startBreakAnimation(remainingTime); // Pass remaining time to animation function
        lv_obj_add_flag(ui_img_focus_sec, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
        lv_obj_clear_flag(ui_img_break_sec, LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_ADV_HITTEST);
      }

      String timeStatus = isFocusTime ? "Focus Time" : "Break Time";
      Serial.printf("Remaining time: %02d:%02d [%s]\n", remainingMinutes, remainingSeconds, timeStatus.c_str());
    }
  }
}

void startFocus()
{
  Serial.println("Focus started");
  isFocusTime = true;
  isPaused = false;
  endTime = millis() + FOCUS_DURATION;
  remainingTime = FOCUS_DURATION;
}

void startBreak()
{
  Serial.print("Break started after cycle: ");
  Serial.println(cycleCount);
  isFocusTime = false;
  isPaused = false;
  endTime = millis() + (cycleCount == CYCLES_BEFORE_LONG_BREAK - 1 ? BREAK_DURATION_LONG : BREAK_DURATION_SHORT);
  cycleCount = (cycleCount + 1) % CYCLES_BEFORE_LONG_BREAK;
}

void updateTimer()
{
  if (!isPaused && millis() > endTime)
  {
    if (isFocusTime)
    {
      startBreak();
    }
    else
    {
      startFocus();
    }
    updateUILabels(); // Update UI labels whenever the timer state changes
  }

  if (!isPaused)
  {
    updateUILabels(); // Also ensure the UI labels are updated continuously as the time counts down
  }
}

void updateUILabels()
{
  long remainingMs = endTime - millis(); // Calculate remaining milliseconds
  if (remainingMs < 0)
    remainingMs = 0;

  int remainingMinutes = remainingMs / 60000; // Convert milliseconds to minutes

  char timeString[4];                          // Buffer to hold the time string
  sprintf(timeString, "%d", remainingMinutes); // Format the minutes without leading zero

  char cycleString[10]; // Buffer to accommodate the cycle count format
  sprintf(cycleString, "%d/%d", cycleCount + 1, CYCLES_BEFORE_LONG_BREAK);

  if (isFocusTime)
  {
    lv_label_set_text(ui_label_focus, timeString); // Update the focus time label
  }
  else
  {
    lv_label_set_text(ui_label_break, timeString); // Update the break time label
  }

  lv_label_set_text(ui_label_cycle, cycleString); // Displays "1/4", "2/4", etc.
}

void handleButtons()
{
  button1.update();
  button2.update();

  if (button1.fell())
  { // Detect falling edge (button press)
    if (isPaused)
    {
      // Resume the timer from the paused state
      endTime = millis() + remainingTime;
      isPaused = false;
      Serial.println("Timer resumed");

      if (isFocusTime)
      {
        startFocusAnimation(remainingTime); // Pass remaining time to resume focus animation
      }
      else
      {
        startBreakAnimation(remainingTime); // Pass remaining time to resume break animation
      }

      // Change the image to pause
      lv_img_set_src(ui_img_start_pause, &ui_img_elements_pause_png);
    }
    else
    {
      // Pause the timer and save remaining time
      remainingTime = endTime - millis();
      isPaused = true;
      Serial.println("Timer paused");

      // Stop the appropriate animation
      if (isFocusTime)
      {
        lv_anim_del(ui_img_focus_sec, NULL);
      }
      else
      {
        lv_anim_del(ui_img_break_sec, NULL);
      }

      // Change the image to start
      lv_img_set_src(ui_img_start_pause, &ui_img_elements_start_png);
    }
  }

  if (button2.fell())
  {
    Serial.println("Timer reset");
    cycleCount = 0;                     // Reset cycle count
    startFocus();                       // Start focus regardless of current state
    remainingTime = FOCUS_DURATION;     // Ensure remaining time is set correctly for the new session
    startFocusAnimation(remainingTime); // Start the focus animation with the full duration
  }
}
