#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>
#include <ui.h>

#define BUTTON_PIN_1 0 // boot button
#define BUTTON_PIN_2 14

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 170;
static const uint16_t screenHeight = 320;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(); /* TFT instance */

lv_obj_t* ui_btn_matrix;  // Declare ui_btn_matrix in the global scope

// Forward declaration of buttonMatrixEventHandler
lv_res_t buttonMatrixEventHandler(lv_obj_t* btnm, const char* txt);

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

int lastButtonState = HIGH;

String selectedButtonName = "";  // Global variable to store the selected button name for calculation
uint8_t selectedButtonIndex = 0; // Global variable for the selected button index

volatile bool button1Pressed = false; // Declare button1Pressed as a global variable
volatile bool button2Pressed = false; // Declare button2Pressed as a global variable

String selectedOperator = "";
String value1 = "";
String value2 = "";

// Forward declarations
void clearCalculation();
bool isOperator(const String &buttonName);
void performCalculation(); 

// Global variable for the selected panel
lv_obj_t* ui_pnl_selected;

// Array to store button information
const int numButtons = 18;
const char* buttonInfo[numButtons][3] = {
    {"C", "7", "95"},
    {"=", "47", "95"},
    {"/", "87", "95"},
    {"*", "127", "95"},
    {"7", "7", "138"},
    {"8", "47", "138"},
    {"9", "87", "138"},
    {"-", "127", "138"},
    {"4", "7", "182"},
    {"5", "47", "182"},
    {"6", "87", "182"},
    {"+", "127", "182"},
    {"1", "7", "226"},
    {"2", "47", "226"},
    {"3", "87", "226"},
    {"0", "8", "266"},
    {".", "87", "266"},
    {"equals_alt", "127", "226"}
};


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
    tft.setRotation( 4 ); 

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

    // Set up button pins
   /* pinMode(BUTTON_PIN_1, INPUT_PULLUP);
    pinMode(BUTTON_PIN_2, INPUT_PULLUP); */

    // Set up button pins
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_1), []() { button1Pressed = true; }, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN_2), []() { button2Pressed = true; }, FALLING);

}

void handleButton1Press()
{
    // Read the state of the button
    int buttonState = digitalRead(BUTTON_PIN_1);

    // Check if the button is pressed
    if (buttonState == LOW)
    {
        // Move to the next button in a cyclic manner
        selectedButtonIndex = (selectedButtonIndex + 1) % numButtons;

        // Get the position and name of the selected button
        int x = atoi(buttonInfo[selectedButtonIndex][1]);
        int y = atoi(buttonInfo[selectedButtonIndex][2]);
        const char *buttonName = buttonInfo[selectedButtonIndex][0];

        // Update the position of ui_pnl_selected
        lv_obj_set_pos(ui_pnl_selected, x, y);

        // Print debug information
        Serial.print("Selected Button Index: ");
        Serial.println(selectedButtonIndex);
        Serial.print("Current Button: ");
        Serial.println(buttonName);
        Serial.print("Current Button Position: (");
        Serial.print(x);
        Serial.print(", ");
        Serial.print(y);
        Serial.println(")");
        Serial.println("------------------");

        // Wait until the button is released
        while (digitalRead(BUTTON_PIN_1) == LOW)
        {
            delay(10);
        }
    }
}

void handleButton2Press()
{
    // Read the state of the button
    int buttonState = digitalRead(BUTTON_PIN_2);

    // Check for button press
    if (buttonState == LOW)
    {
        // Get the position of the selected button
        int x = atoi(buttonInfo[selectedButtonIndex][1]);
        int y = atoi(buttonInfo[selectedButtonIndex][2]);

        // Serial print the selected button name and position
        String buttonName = buttonInfo[selectedButtonIndex][0];
        Serial.print("Selected Button Name: ");
        Serial.println(buttonName);
        Serial.print("Current Button Position: (");
        Serial.print(x);
        Serial.print(", ");
        Serial.print(y);
        Serial.println(")");

        // Perform actions based on the selected button
        if (buttonName.equals("C"))
        {
            // Clear variable and display
            clearCalculation();
        }
        else if (isOperator(buttonName))
        {
            // Selected button is an operator, perform calculation if there is a previous operation
            if (!selectedOperator.isEmpty() && !value1.isEmpty() && !value2.isEmpty())
            {
                performCalculation();
                value1 = lv_label_get_text(ui_lbl_answer);
                value2 = "";
            }

            // Save the operator
            selectedOperator = buttonName;

            // Update the label or perform other actions as needed
            lv_label_set_text(ui_lbl_answer, selectedOperator.c_str());
        }
        else if (buttonName.equals("=") || buttonName.equals("equals_alt"))
        {
            // Selected button is "=", perform calculation and display
            if (!selectedOperator.isEmpty() && !value1.isEmpty() && !value2.isEmpty())
            {
                performCalculation();
                value1 = lv_label_get_text(ui_lbl_answer);
                value2 = "";
                selectedOperator = "";
            }
        }
        else
        {
            // Selected button is a number, update the variable for calculation
            if (selectedOperator == "")
            {
                // If no operator selected, update value1
                value1 += buttonName;
                lv_label_set_text(ui_lbl_answer, value1.c_str());
            }
            else
            {
                // If operator selected, update value2
                value2 += buttonName;
                lv_label_set_text(ui_lbl_answer, value2.c_str());
            }

            // Update the label or perform other actions as needed
            // ...
        }

        // Reset the selected button name after processing
        selectedButtonName = "";

        // Wait until the button is released
        while (digitalRead(BUTTON_PIN_2) == LOW)
        {
            delay(10);
        }
    }
}

void clearCalculation() {
    // Implement the logic to clear calculation variables and display
    selectedButtonName = "";
    value1 = "";
    value2 = "";
    selectedOperator = "";
    lv_label_set_text(ui_lbl_answer, "");
}

bool isOperator(const String &buttonName) {
    // Implement the logic to check if the button name is an operator
    return buttonName.equals("+") || buttonName.equals("-") || buttonName.equals("*") || buttonName.equals("/");
}

void performCalculation()
{
    // Perform calculation based on the selected button and display the result
    // Update the label or perform other actions as needed
    // ...

    // Example: Display a placeholder result for demonstration purposes
    if (selectedOperator != "" && value1 != "" && value2 != "")
    {
        // Convert string values to float for calculation
        float operand1 = value1.toFloat();
        float operand2 = value2.toFloat();
        float result;

        // Perform calculation based on the operator
        if (selectedOperator == "+")
        {
            result = operand1 + operand2;
        }
        else if (selectedOperator == "-")
        {
            result = operand1 - operand2;
        }
        else if (selectedOperator == "*")
        {
            result = operand1 * operand2;
        }
        else if (selectedOperator == "/")
        {
            // Check for division by zero
            if (operand2 != 0)
            {
                result = operand1 / operand2;
            }
            else
            {
                // Handle division by zero error
                result = 0; // You may set an appropriate value or display an error message
            }
        }
        else
        {
            // Handle other operators if needed
            result = 0;
        }

        // Display the result without decimal part if it's a whole number
        if (result == static_cast<int>(result))
        {
            lv_label_set_text(ui_lbl_answer, String(static_cast<int>(result)).c_str());
        }
        else
        {
            lv_label_set_text(ui_lbl_answer, String(result).c_str());
        }

        // Print result to Serial Monitor
        Serial.print("Result: ");
        Serial.println(result);
    }
    else
    {
        // Display an error message if operands or operator are missing
        lv_label_set_text(ui_lbl_answer, "Error !!!");
        Serial.println("Error: Incomplete Expression");
    }
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5);

    handleButton1Press();
    handleButton2Press();
}
