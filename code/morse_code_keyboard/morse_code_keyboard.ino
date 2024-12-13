/*
  HID Keyboard example


  created 2020
  by Deqing Sun for use with CH55xduino

  This example code is in the public domain.

  cli board options: usb_settings=user148

*/

//For windows user, if you ever played with other HID device with the same PID C55D
//You may need to uninstall the previous driver completely        


#ifndef USER_USB_RAM
#error "This example needs to be compiled with a USER USB setting"
#endif

#include "src/userUsbHidKeyboard/USBHIDKeyboard.h"

#define BUTTON_PIN 33


bool buttonPressPrev = false;


void setup() {
  USBInit();
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  //button 1 is mapped to letter 'a'
  bool buttonPress = !digitalRead(BUTTON_PIN);
  if (buttonPressPrev != buttonPress) {
    buttonPressPrev = buttonPress;
    if (buttonPress) {
      Keyboard_press('x');
      delay(10);
    } else {
      Keyboard_release('x');
      delay(10);
    }
  }
}
