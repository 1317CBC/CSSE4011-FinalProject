/**
 * This code demonstrates how to create a simple user interface with two buttons
 * on the M5Stack Core2 display. The buttons are used to control two devices
 * (e.g. a fan and an LED). When a button is pressed, the corresponding device
 */

#include <M5Core2.h>

// define the button properties
const int buttonWidth = 150;         // button width
const int buttonHeight = 150;        // button height
const int buttonY = 40;              // button y position
const int cornerRadius = 15;         // radius of the button corners
const uint16_t shadowColor = 0x39E7; // shadow color

void setup()
{
    M5.begin();
    M5.Lcd.setFont(&FreeMonoOblique24pt7b);
    Serial.begin(9600);

    // initialize the display
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setTextColor(WHITE, BLACK);
    M5.Lcd.setTextColor(0x00f0ff, WHITE);

    // draw buttons
    drawButton("FAN", 0, buttonY, WHITE);
    drawButton("LED", 160, buttonY, WHITE);
}

void loop()
{
    M5.update();

    // check if the screen is touched
    if (M5.Touch.ispressed())
    {
        TouchPoint_t pos = M5.Touch.getPressPoint(); // get the touch point

        if (pos.y >= buttonY && pos.y < buttonY + buttonHeight)
        {
            if (pos.x >= 0 && pos.x < buttonWidth)
            {                        // area of the FAN button
                Serial.println("0"); // send 0 to the serial port
                M5.Lcd.setTextColor(0x00f0ff, BLUE);
                drawButton("FAN", 0, buttonY, BLUE); // set button FAN to blue
                M5.Lcd.setTextColor(0x00f0ff, WHITE);
                drawButton("LED", 160, buttonY, WHITE); // reset button LED to white
                delay(100);
            }
            else if (pos.x >= 160 && pos.x < 320)
            {                        // area of the LED button
                Serial.println("1"); // send 1 to the serial port
                M5.Lcd.setTextColor(0x00f0ff, BLUE);
                drawButton("LED", 160, buttonY, BLUE); // set button LED to blue
                M5.Lcd.setTextColor(0x00f0ff, WHITE);
                drawButton("FAN", 0, buttonY, WHITE); // reset button FAN to white
                delay(100);
            }
        }
    }
}

/**
 * function to draw a button
 * @param label the text to display on the button
 * @param x the x position of the button
 * @param y the y position of the button
 * @param color the color of the button
 */
void drawButton(String label, int x, int y, uint16_t color)
{

    // draw shadow
    M5.Lcd.fillRoundRect(x + 3, y + 3, buttonWidth, buttonHeight, cornerRadius, shadowColor);
    // draw button
    M5.Lcd.fillRoundRect(x, y, buttonWidth, buttonHeight, cornerRadius, color);
    // draw border
    M5.Lcd.drawRoundRect(x, y, buttonWidth, buttonHeight, cornerRadius, WHITE);
    // set text color
    M5.Lcd.setCursor(x + 30, y + buttonHeight / 2); // set the cursor position
    M5.Lcd.print(label);                            // print the label
}
