         /////////////////////////////////////////////  
        //   Arduino-Based Mouse and Keyboard      //
       //               Controller                //
      //             ---------------             //
     //           (Arduino Pro Micro)           //           
    //             by Kutluhan Aktar           // 
   //                                         //
  /////////////////////////////////////////////

//
// This Raspberry Pi-compatible device lets you use joysticks as a mouse and enter keyboard and modifier keys with two different keypad settings.
//
// For more information:
// https://www.theamplituhedron.com/projects/Arduino-Based-ATmega32U4-Mouse-and-Keyboard-Controller
//
//
// Connections
// Arduino Pro Micro :           
//                                JoyStick (Left)
// A0  --------------------------- VRX
// A1  --------------------------- VRY
// D14 --------------------------- SW
// 5V  --------------------------- 5V
// GND --------------------------- GND
//                                JoyStick (Right)
// A2  --------------------------- VRX
// A3  --------------------------- VRY
// D16 --------------------------- SW
// 5V  --------------------------- 5V
// GND --------------------------- GND
//                                KeyPad
// D2  --------------------------- C1
// D3  --------------------------- C2
// D4  --------------------------- C3
// D5  --------------------------- C4
// D6  --------------------------- R1
// D7  --------------------------- R2
// D8  --------------------------- R3
// D9  --------------------------- R4
//                                Letter Keypad LED
// D15 --------------------------- +
//                                Number Keypad LED
// D10 --------------------------- +


// Include the required libraries.
#include <Keypad.h>
#include "Keyboard.h"
#include "Mouse.h"

const byte ROWS = 4; // four rows
const byte COLS = 4; // four columns

// Define the symbols on the buttons of the keypads.
char letterKeys[ROWS][COLS] = {
  {'e','a','r','i'},
  {'o','t','n','s'},
  {'p','m','h','w'},
  {'l','c','u','d'}
};

char numberKeys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'#','0','*','%'},
  {'7','8','9','/'}

};

byte rowPins[ROWS] = {6, 7, 8, 9}; // Connect to the row pinouts of the keypad.
byte colPins[COLS] = {2, 3, 4, 5}; // Connect to the column pinouts of the keypad.

// Initialize an instance of class NewKeypad for each keypad setting - letter and number.
Keypad letterKeypad = Keypad( makeKeymap(letterKeys), rowPins, colPins, ROWS, COLS);
Keypad numberKeypad = Keypad( makeKeymap(numberKeys), rowPins, colPins, ROWS, COLS);

// Define pinouts of joysticks.
#define VRX_L A0
#define VRY_L A1
#define SW_L 14
#define VRX_R A2
#define VRY_R A3
#define SW_R 16

// Define keypad LED pins:
#define let_LED 15
#define num_LED 10

// Define dynamic keypad options - letter and number.
volatile boolean letter = true;
volatile boolean number = false; 

// Define the data holders:
int joystick_left_x, joystick_left_y, joystick_left_button, joystick_right_x, joystick_right_y, joystick_right_button;

void setup() {
  Serial.begin(9600);

  // Initialize mouse and keyboard control:
  Mouse.begin();
  Keyboard.begin();

  // Joystick button settings:
  pinMode(SW_L, INPUT);
  digitalWrite(SW_L, HIGH);
  pinMode(SW_R, INPUT);
  digitalWrite(SW_R, HIGH);

  // Keypad LEDs:
  pinMode(let_LED, OUTPUT);
  digitalWrite(let_LED, HIGH);
  pinMode(num_LED, OUTPUT);
  digitalWrite(num_LED, LOW);
}

void loop() {
  // Read joystick positions:
  read_joysticks();

  // Mouse:
  mouse_controls();

  // Keyboard:
  keyboard_controls();

}

void read_joysticks(){
  joystick_left_x = analogRead(VRX_L); 
  joystick_left_y = analogRead(VRY_L);
  joystick_left_button = digitalRead(SW_L);
  
  joystick_right_x = analogRead(VRX_R); 
  joystick_right_y = analogRead(VRY_R);  
  joystick_right_button = digitalRead(SW_R);

  /*
  Serial.print(joystick_right_x);
  Serial.print("\t");
  Serial.print(joystick_right_y);
  Serial.print("\t\t");
  Serial.print(joystick_right_button);
  Serial.print("\n\n");
  */
}

void mouse_controls(){
  // Move mouse according to the left joystick movements.
  if(joystick_left_y > 700)  Mouse.move(0, -15); // UP
  if(joystick_left_y < 300)  Mouse.move(0, 15);  // DOWN
  if(joystick_left_x > 700)  Mouse.move(-15, 0); // LEFT
  if(joystick_left_x < 300)  Mouse.move(15, 0);  // RIGHT
  if(joystick_left_button == 0) Mouse.click(MOUSE_LEFT); // LEFT CLICK
  if(joystick_right_button == 0) Mouse.click(MOUSE_RIGHT); // RIGHT CLICK
  delay(100);
}

void keyboard_controls(){
  // Change keypad settings (letter or number) and press modifier keys according to the right joystick movements.
  if(joystick_right_y > 700){ Keyboard.press(KEY_RETURN); delay(100); Keyboard.releaseAll(); }    // RETURN
  if(joystick_right_y < 300){ Keyboard.press(KEY_BACKSPACE); delay(100); Keyboard.releaseAll(); } // BACKSPACE
  if(joystick_right_x > 700){ letter = true; number = false; digitalWrite(let_LED, HIGH); digitalWrite(num_LED, LOW); } // Letter Keypad
  if(joystick_right_x < 300){ letter = false; number = true; digitalWrite(let_LED, LOW); digitalWrite(num_LED, HIGH); } // Number Keypad
  
  // Get the custom key depending on the selected keypad type - letter or number.
  char customKey;
  if(letter == true) customKey = letterKeypad.getKey();
  if(number == true) customKey = numberKeypad.getKey();
  
  // Write the custom key.
  if (customKey){
    Keyboard.write(customKey);
  }
}
