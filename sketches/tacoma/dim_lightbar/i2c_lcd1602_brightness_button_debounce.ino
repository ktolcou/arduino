
// C:\Users\ktolc\OneDrive\Documents\Arduino\Sunfounder\I2C LCD1602 Module\Sketches\i2c_lcd1602_brightness_button_debounce

/*
  The circuit:
  - Arduino UNO is Default board.
  - Arduino Mega 2560 is alternate.
  - Smaller boards are possible if they have sufficient storeage.
  
  - LCD From www.sunfounder.com.
  - Alternate LCD is possible. Sunfounder 16 x 4 should plug in the same.
  - Other boards would use built in libraries instead of LiquidCrystal_I2C.h.
  
    LCD attached to 5v, ground, A4 and A5
  - LED attached from pin 13 to ground
  - Pushbutton attached from pin 7 to +5V
  - 10 kilohm resistor attached from pin 2 to ground
*/

/********************************
  I2C LCD1602 display from SunFounder.
  DEVICE PINOUTS:
  These are for Uno, See additional notes for Mega 2560.
  LCD      ARDUINO   NOTE                                       Sunfounder I2C LCD1602
  +++      +++++++   +++++++++++++++++++++++++++++++++++++++++  ++++++++++++++++++++++
  VSS      GND       Ground                                     GND* on I2C Board
  VDD      +5V       3 - 5V                                     VCC* on I2C Board
  SCL      A4        Clock Line - Analog input (Mega also 21)
  SDA      A5        Data Line - Analog input (Mega also 20) 
  VO       +5V       Contrast Via Potentiometer
  RS       D12       Register Select - 0=Command / 1=Character
  RW       GND       Read/Write - 0=Write or 1=Read
  E        D11       Enable - Enable data transmit
  D0       N/A       Data Bit 0
  D1       N/A       Data Bit 1
  D2       N/A       Data Bit 2
  D3       N/A       Data Bit 3
  D4       D5        Data Bit 4 - used in 4 bit operation
  D5       D4        Data Bit 5 - used in 4 bit operation
  D6       D3        Data Bit 6 - used in 4 bit operation
  D7       D2        Data Bit 7 - used in 4 bit operation
  A        +5V       3 - 5V
  K        GND       Ground
  
  BL1 - Backlight +   Emitter of 2N3904, Collector to VCC, Base to D10 via 10K resistor
  BL2 - Backlight -   GND

  #SYNOPSIS
    Sets up LCD display, increments brightness while displaying the brightness value, then loops
    n times while scrolling the bottom line only.
  
  #DESCRIPTION
    Not yet.
  
  #NOTES
    Additionally there is commented code that scrolls the entire display.
    Also contains 2 custom functions for scrolling 1 line onlyinstead of using the built in librayy
    that scrolls the entire display.
  
  #LINK
    http://www.arduino.cc/en/Tutorial/Debounce
  
  #LATEST CHANGES
  20200407  Create Date
  20200409  Add Debounce for switch.
            Add cycle back to starting brightness on button push if already at max brightness.
            Add brightness output to serial monitor if hooked up to computer,  
  
  #TO DO
    PRIORITY 1
    
/*********************************************************/
// include the library code
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display
/*********************************************************/

// Constants won't change. They're used here to set pin numbers:
// Adding keyword const to be verbose, but not required.
const int buttonPin = 7;    // the number of the pushbutton pin
const int ledPin = 13;      // the number of the LED pin

// Define the pin the controls the brightness of the screen on the arduino board.
// Pin 10 attaches to Pin A on the Sunfounder LCD.
// Pin is PWM so you can analogWrite to it and have it output a variable value
const int bri = 10;   //Set pin 10 to OUTPUT, LCD_Backlight pin 10 on SunFounder LCD.
//Alternate syntax.
//#define bri 10

// Set constant sting value for start of line 1
String lcd_line1_start_text = "16x2 LCD Screen";
// Set constant sting value for start of line 2
String lcd_line2_start_text = "Brightness  ";

// Set the starting and full brightness values.
int lcd_bri_start = 32; //32 is roughly 1/8 brightness.
int lcd_bri_full = 255;  //255 is full.
int lcd_fade_amount = 32;           //fade increment for when you push the LCD brightness cycle button.
int brightness = 32;                //initial LCD brightness value.

// Declare strings that are set later in file.
String serial_bri_text;
String serial_BL_UP_BUTTON_STATE_text;

// Variables will change:
int ledState = LOW;                 // the current state of the output pin, LOW means LED is initially off. 
int buttonState;                    // the current reading from the input pin
int lastButtonState = LOW;          // the previous reading from the input pin
int count_of_max_lcd_bright = 0;    // initial count of how many times we have been at max brightness after hitting the LCD brightness cycle button.
int BL_UP_BUTTON_STATE = 0;         // used to store the initial state of the backlight up button. 
bool lcd_bri_full_reached = false;  //LCD display at maximum brightness is currently set to false.

String lcd_bri_text = lcd_line2_start_text + lcd_bri_start; //Concatenate 1 LCD starting line text and current starting value for output to 1st LCD line.

// The following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

/*
//Not currently used, for scroll right and scroll left for a single line on the LCD.
// ++++++++++++++++++++ Scroll single line function start ++++++++++++++++++++
int i = 0;
int j = 0;
int k = 0;
int delayTime2 = 350; // Delay between shifts.

//Not currently used.
void scrollInFromRight (int line, char str1[])
{
  i = strlen(str1);
  for (j = 16; j >= 0; j--)
    {
    lcd.setCursor(0, line);
    for (k = 0; k <= 15; k++)
          {
            lcd.print(" "); // Clear line.
          } //End for k
    lcd.setCursor(j, line);
    lcd.print(str1);
    delay(delayTime2);
    } //End for j
} //End void scrollInFromRight

//Not currently used.
void scrollInFromLeft (int line, char str1[])
{
  i = 40 - strlen(str1);
  line = line - 1;
  for (j = i; j <= i + 16; j++)
    {
      for (k = 0; k <= 15; k++)
        {
          lcd.print(" "); // Clear line
        } //End for k
        lcd.setCursor(j, line);
        lcd.print(str1);
        delay(delayTime2);
    } //End for j
} //End void scrollInFromLeft

// ++++++++++++++++++++ Scroll single line function end ++++++++++++++++++++
*/

void setup() // The setup routine runs once when you power the Arduino, upload new code, or press reset.
  {
    // initialize serial communication at 9600 bits per second:
    Serial.begin(9600);
    
    // initialize 16 x 2 LCD
    lcd.init();  //initialize the lcd
    lcd.backlight();  //open the backlight

    pinMode(buttonPin, INPUT);  // tell Arduino buttonPin is an input
    pinMode(ledPin, OUTPUT);  // tell Arduino ledPin is an output
    pinMode(bri, OUTPUT); //Set pin 10 to OUTPUT

    /*
      //Contrast stuff.
      //pinMode(contra, OUTPUT); //set pin 9 to OUTPUT
      //digitalWrite(contra, LOW); outputs no power to the contrast pin. This lets you see the words.
    */

    analogWrite(bri, lcd_bri_start); //Outputs power to the LCD brightness pin at whatever the start variable is set to.
    // clear the screen
    lcd.clear();
    lcd.setCursor(0, 0); // set the cursor to column 0, line 0
    //lcd.print("16x2 LCD Screen");  // Print a message to the LCD
    lcd.print(lcd_line1_start_text);  // Print a message to the LCD
    lcd.setCursor(0, 1); // set the cursor to column 0, line 1
    //Reset the text.
    lcd_bri_text = lcd_line2_start_text + brightness;
    lcd.print(lcd_bri_text);  // Print the concatenated variable to the LCD (Brightness + initial brightness value).
    
    // Send the initial LCD screen display to the serial monitor if plugged into the computer.
    if (Serial) // wait for serial port to connect. Needed for native USB
    //Alternate syntax.
    //if (Serial.available()) // wait for serial port to connect. Needed for native USB.
      {
        Serial.println(lcd_line1_start_text); // Print the above value to the serial monitor.
        delay(1);        // delay in between reads for stability.
        Serial.println(lcd_bri_text); // Print the above value to the serial monitor.
        delay(1);        // delay in between reads for stability.
      }       
      // set initial LED state
      digitalWrite(ledPin, ledState);
  } // end setup

void loop() // The loop routine runs over and over again forever.
  {
    // read the state of the switch into a local variable:
    int reading = digitalRead(buttonPin);

    // check to see if you just pressed the button
    // (i.e. the input went from LOW to HIGH), and you've waited long enough
    // since the last press to ignore any noise:

    // If the switch changed, due to noise or pressing:
    if (reading != lastButtonState)
      {
        // reset the debouncing timer
        lastDebounceTime = millis();
      }

    if ((millis() - lastDebounceTime) > debounceDelay)
      {
        // Whatever the reading is at, it's been there for longer than the debounce
        // delay, so take it as the actual current state:
        
        // Compares the variable on the left with the value or variable on the right of the operator.
        // Returns true when the two operands are not equal.
        if (reading != buttonState) //If button is pressed or let go. (There was a voltage change.)
          {
            buttonState = reading; //Don't remove this section otherwise we won't increment the brightness.
            if (BL_UP_BUTTON_STATE == 0)
              {
                BL_UP_BUTTON_STATE = 1;
              } //End Pushed button.
            else
              {
                BL_UP_BUTTON_STATE = 0;
              } //End Let go of button.
            // only toggle the LED if the new button state is HIGH
            if (buttonState == HIGH)
              {
                ledState = !ledState;
                brightness += lcd_fade_amount;
                if (brightness > lcd_bri_full) //255
                  {
                    brightness = lcd_bri_full; //255
                    lcd_bri_full_reached = true;
                  } // End if (brightness > 255)
                else
                  {
                    lcd_bri_full_reached = false;
                  } //End else (brightness > lcd_bri_full)
                // If we reached full brightness on last button push set brightness variable to 0 so we can add the fade amount and start over.
                if (lcd_bri_full_reached == true)
                  {
                    count_of_max_lcd_bright = count_of_max_lcd_bright + 1;
                    if (count_of_max_lcd_bright == 2) //If were are at max brightness a second time set the brightness to starting value.
                      {
                        //brightness = 32;
                        brightness = 32;
                      }
                    // clear the screen
                    lcd.clear();
                    lcd.setCursor(0, 0); // set the cursor to column 0, line 0
                    lcd.print(lcd_line1_start_text);  // Print a message to the LCD                  
                  } //End if (lcd_bri_full_reached == true)  
                else //lcd_bri_full_reached is `false
                  {
                    count_of_max_lcd_bright = 0;
                  } //End else (lcd_bri_full_reached == true)               
                analogWrite(bri, brightness);
                lcd.setCursor(0, 1); // set the cursor to column 0, line 1
                //Reset the text.
                lcd_bri_text = lcd_line2_start_text + brightness;
                lcd.print(lcd_bri_text);  // Print the concatenated variable to the LCD (Brightness + initial brightness value).
                
                // Send the initial LCD screen display to the serial monitor if plugged into the computer.
                if (Serial) // wait for serial port to connect. Needed for native USB
                //Alternate syntax.
                //if (Serial.available()) // wait for serial port to connect. Needed for native USB.
                  {
                    serial_bri_text = lcd_line2_start_text + brightness;
                    Serial.println(serial_bri_text); // Print the above value to the serial monitor.
                    delay(1);        // delay in between reads for stability.
                  }
              } //End buttonState HIGH
            else // Turn off the LED because current is not HIGH.
              {
                ledState = !ledState;
              } //End buttonState not HIGH
          } //End if (reading != buttonState)
      } //End if ((millis() - lastDebounceTime) > debounceDelay)

    // set the LED:
    digitalWrite(ledPin, ledState);
    
    // save the reading. Next time through the loop, it'll be the lastButtonState:
    lastButtonState = reading;
  } //end main loop

void post_Loop() // Do more stuff here after the loop is started.
  {
     //digitalWrite(latchPin, LOW);
  }
