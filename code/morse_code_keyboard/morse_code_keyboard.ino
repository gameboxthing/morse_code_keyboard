/*
 * Morse Code Keyboard
 * 
 * Created 2024
 * Quentin lovett for use with CH55xduino for the CH552G
 * 
 * This code is in the public domain.
 */


#ifndef USER_USB_RAM
#error "Compilation needs to be done with USER USB setting"
#endif

#include <string.h>
#include "src/userUsbHidKeyboard/USBHIDKeyboard.h"

#define BUTTON_PIN 33
#define DEBOUNCE 10  // debounce duration ms
#define DOT_LENGTH 50 // dot length ms
#define SYMBOL_LENGTH 250  // standard to measure timing intervals
#define DASH_LENGTH (SYMBOL_LENGTH * 1)
#define LETTER_GAP (SYMBOL_LENGTH * 3)
#define WORD_GAP (SYMBOL_LENGTH * 7)
#define CHARACTER_BUFFER_LENGTH 5  // the most morse symbols that can make up a character

enum InputState {
    IDLE,
    PRESSED
};

enum WordComputed {
    UNPROCESSED,
    PROCESSED
};

struct MorseSequence {
    const char *code;
    const char character;
};

void parseBuffer();
void interpretMorse();
void clearBuffer();

struct MorseSequence morse_table[] = {
    {".-", 'a'}, {"-...", 'b'}, {"-.-.", 'c'}, {"-..", 'd'}, {".", 'e'}, {"..-.", 'f'},
    {"--.", 'g'}, {"....", 'h'}, {"..", 'i'}, {".---", 'j'}, {"-.-", 'k'}, {".-..", 'l'},
    {"--", 'm'}, {"-.", 'n'}, {"---", 'o'}, {".--.", 'p'}, {"--.-", 'q'}, {".-.", 'r'},
    {"...", 's'}, {"-", 't'}, {"..-", 'u'}, {"...-", 'v'}, {".--", 'w'}, {"-..-", 'x'},
    {"-.--", 'y'}, {"--..", 'z'}, {".----", '1'}, {"..---", '2'}, {"...--", '3'}, {"....-", '4'},
    {".....", '5'}, {"-....", '6'}, {"--...", '7'}, {"---..", '8'}, {"----.", '9'}, {"-----", '0'},
    {"   ", ' '}, {NULL, 0} // End of table marker, space
};

enum InputState state = IDLE;
enum WordComputed word_computed = PROCESSED;
unsigned long start_time;
unsigned long current_time;
unsigned long released_time;
unsigned long duration;
char morse_buffer[CHARACTER_BUFFER_LENGTH + 1] = "";
int button_position;

void setup() {
    // initialise usb
    USBInit();
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // using internal pulls-up resistor
}

void loop() {
    // check button state
    button_position = digitalRead(BUTTON_PIN);
    current_time = millis();

    switch (state) {
        case IDLE:
            if (button_position == LOW) {  // Button pressed 
                state = PRESSED;
                start_time = current_time;
                word_computed = UNPROCESSED;
            } else {
                // Parse all the characters that have been added to the buffer if any
                parseBuffer();
            }
            break;

        case PRESSED:
            if (button_position == HIGH) {  // Button released
                state = IDLE;
                released_time = current_time;
                duration = current_time - start_time;

                // Parse the individual symbols
                if (duration >= DASH_LENGTH) {
                    strcat(morse_buffer, "-");
                } else if (duration >= DOT_LENGTH) {
                    strcat(morse_buffer, ".");
                }
            }
            break;

        default:
            // Do nothing
            break;
    }

    delay(DEBOUNCE);  // debounce
}

void parseBuffer() {
    // compute space
    if ((current_time - released_time) >= WORD_GAP && word_computed == UNPROCESSED) {
        word_computed = PROCESSED;
        Keyboard_write(' ');
    // compute character
    } else if ((current_time - released_time) >= LETTER_GAP && strlen(morse_buffer) > 0) {
        interpretMorse();
        clearBuffer();
    }
}

void interpretMorse() {
    for (int i = 0; morse_table[i].code != NULL; ++i) {
        if (strncmp(morse_buffer, morse_table[i].code, sizeof(morse_buffer) - 1) == 0) {
            Keyboard_write(morse_table[i].character);
        }
    }
}

void clearBuffer() {
    memset(morse_buffer, 0, sizeof(morse_buffer));
}