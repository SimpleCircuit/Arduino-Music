
/* ArduinoMusic

    Program to enter and play "Arduino MIDI" music from the serial monitor
    Program includes 3 octaves of notes (C3-C6), dotted notes, and rests
    pinout:
      output to speaker = analog pin 2 (A2)
      pins for piano keys = digital pins 3 through 13

    Last revision: 6/23/2016  Arduino Version: 1.6.8
    Author: Terry Dunlap aka "Simple Circuit"
    This code is in the public domain.
*/

#include <avr/pgmspace.h>
#define notePin A2    // Output to speaker
#define keyC 3
#define keyD 4
#define keyDShrp 5
#define keyE 6
#define keyF 7
#define keyFShrp 8
#define keyG 9
#define keyA 10
#define keyAShrp 11
#define keyB 12
#define keyC5 13
#define bufferSize 20
#define beat 400           // Default note length (noteValue) = 400 milliseconds

String cmdString;
int index = 0;
String noteString[120];   // Melodies stored in this string
int noteString_Size = 0;  // Size of melody up to 120 characters
char noteName[20];

// Store the note frquencies
const float frequency[] PROGMEM = {130.81, 138.59, 146.83, 155.56, 164.81, 174.61,
                                   185.00, 196.00, 207.65, 220.00, 233.08, 246.94,
                                   261.63, 277.18, 293.66, 311.13, 329.63, 349.23,
                                   369.99, 392.00, 415.30, 440.00, 466.16, 493.88,
                                   523.25, 554.37, 587.33, 622.25, 659.25, 698.46,
                                   739.99, 783.99, 830.61, 880.00, 932.33, 987.77, 1046.50
                                  };

//Store the music composition characters and notes in a 2D array
const char c3[] PROGMEM = "c3";
const char cShrp3[] PROGMEM = "c#3";
const char d3[] PROGMEM = "d3";
const char dShrp3[] PROGMEM = "d#3";
const char e3[] PROGMEM = "e3";
const char f3[] PROGMEM = "f3";
const char fShrp3[] PROGMEM = "f#3";
const char g3[] PROGMEM = "g3";
const char gShrp3[] PROGMEM = "g#3";
const char a3[] PROGMEM = "a3";
const char aShrp3[] PROGMEM = "a#3";
const char b3[] PROGMEM = "b3";
const char c[] PROGMEM = "c";
const char cShrp[] PROGMEM = "c#";
const char d[] PROGMEM = "d";
const char dShrp[] PROGMEM = "d#";
const char e[] PROGMEM = "e";
const char f[] PROGMEM = "f";
const char fShrp[] PROGMEM = "f#";
const char g[] PROGMEM = "g";
const char gShrp[] PROGMEM = "g#";
const char a[] PROGMEM = "a";
const char aShrp[] PROGMEM = "a#";
const char b[] PROGMEM = "b";
const char c5[] PROGMEM = "c5";
const char cShrp5[] PROGMEM = "c#5";
const char d5[] PROGMEM = "d5";
const char dShrp5[] PROGMEM = "d#5";
const char e5[] PROGMEM = "e5";
const char f5[] PROGMEM = "f5";
const char fShrp5[] PROGMEM = "f#5";
const char g5[] PROGMEM = "g5";
const char gShrp5[] PROGMEM = "g#5";
const char a5[] PROGMEM = "a5";
const char aShrp5[] PROGMEM = "a#5";
const char b5[] PROGMEM = "b5";
const char c6[] PROGMEM = "c6";
const char W[] PROGMEM = "1";
const char Wdot[] PROGMEM = "1.";
const char H[] PROGMEM = "2";
const char Hdot[] PROGMEM = "2.";
const char Q[] PROGMEM = "4";
const char Qdot[] PROGMEM = "4.";
const char E[] PROGMEM = "8";
const char Edot[] PROGMEM = "8.";
const char S[] PROGMEM = "16";
const char R1[] PROGMEM = "r1";
const char R2[] PROGMEM = "r2";
const char R4[] PROGMEM = "r4";
const char R4dot[] PROGMEM = "r4.";
const char R8[] PROGMEM = "r8";
const char R16[] PROGMEM = "r16";

const char* const noteTable[] PROGMEM = {c3, cShrp3, d3, dShrp3, e3, f3, fShrp3, g3, gShrp3, a3, aShrp3, b3,
                                         c, cShrp, d, dShrp, e, f, fShrp, g, gShrp, a, aShrp, b,
                                         c5, cShrp5, d5, dShrp5, e5, f5, fShrp5, g5, gShrp5, a5, aShrp5, b5,
                                         c6, W, Wdot, H, Hdot, Q, Qdot, E, Edot, S, R1, R2, R4, R4dot, R8, R16
                                        };


void setup() {
  pinMode (notePin, OUTPUT);
  for (int i = 3; i < 14; i++) {
    pinMode(i, INPUT_PULLUP);
  }
  Serial.begin(9600);
  Serial.println(F("Set serial monitor to [Newline]"
                   " and enter musical composition characters, one at a time."));
  Serial.println();
}

void loop() {
  if (serialInput()) music();
  /* else piano(); */    // Uncomment if using piano function
}

// Get input from serial monitor
boolean serialInput()
{
  char charBuffer[bufferSize];
  char charInput;
  boolean flag1 = false;

  while (Serial.available() > 0)
  {
    charInput = Serial.read();
    if (charInput == '\n')
    {
      charBuffer[index] = '\0';
      flag1 = (index > 0);
      index = 0;
      cmdString = charBuffer;
    }
    else if (index < bufferSize)
    {
      charBuffer[index++] = charInput;
    }
  }
  return flag1;
}

// Enter and play music
void music() {

  float noteFrequency;
  int noteValue = beat;

  if (cmdString == F("play")) {
    for (int i = 0; i < noteString_Size; i++) {
      for (int j = 0; j < 52; j++) {
        strcpy_P(noteName, (char*)pgm_read_word(&(noteTable[j])));
        if (j < 37) {
          noteFrequency = pgm_read_float_near(frequency + j);
        }
        if (noteString[i] == noteName) {
          if (noteString[i] == F("1")) {
            noteValue = beat * 4;     // whole note
          }
          else if (noteString[i] == F("1.")) {
            noteValue = beat * 6;     // dotted whole note
          }
          else if (noteString[i] == F("2")) {
            noteValue = beat * 2;     // half note
          } 
          else if (noteString[i] == F("2.")) {
            noteValue =  beat * 3;    // dotted half note
          }
          else if (noteString[i] == F("4")) {
            noteValue = beat;         // quarter note
          }
          else if (noteString[i] == F("4.")) {
            noteValue =  beat * 1.5;  // dotted quarter note
          }
          else if (noteString[i] == F("8")) {
            noteValue =  beat / 2;    // eighth note
          }
          else if (noteString[i] == F("8.")) {
            noteValue =  beat * 0.75; // dotted eighth note
          }
          else if (noteString[i] == F("16")) {
            noteValue = beat / 4;     // sixteenth note
          }
          else if (noteString[i] == F("r1")) {
            delay(beat * 4);          // whole rest
          }
          else if (noteString[i] == F("r2")) {
            delay(beat * 2);          // half rest
          }
          else if (noteString[i] == F("r4")) {
            delay(beat);              // quarter rest
          }
           else if (noteString[i] == F("r4.")) {
            delay(beat * 1.5);        // dotted quarter rest
          }
          else if (noteString[i] == F("r8")) {
            delay(beat / 2);          // eighth rest
          }
          else if (noteString[i] == F("r16")) {
            delay(beat / 4);          // sixteenth rest
          }
          else {
            tone(notePin, noteFrequency);
            delay(noteValue);
            noTone(notePin);
            delay(50);
          }
        }
      }
    }
  }

  else if (cmdString != F("play")) {
    // Check to see if noteString buffer is full
    if (noteString_Size >= 120) {
      Serial.println(F("Note buffer full"));
      return;
    }
    // Add notes to noteString array
    for (int i = 0; i < 52; i++) {
      strcpy_P(noteName, (char*)pgm_read_word(&(noteTable[i])));
      if (cmdString == noteName) {
        noteString[noteString_Size] = cmdString;
        noteString_Size = noteString_Size + 1;
      }
    }
    // Print notes in noteString array
    for (int i = 0; i < noteString_Size; i++) {
      Serial.print(noteString[i] + ",");
    }
    Serial.println();
  }
}

void piano() {
  if (! digitalRead(keyC)) {
    tone(notePin, 261.63);      // C4
  }
  else if (!digitalRead(keyD)) {
    tone(notePin, 293.66);      // D4
  }
  else if (!digitalRead(keyDShrp)) {
    tone(notePin, 311.13);      // D#4
  }
  else if (!digitalRead(keyE)) {
    tone(notePin, 329.63);      // E4
  }
  else if (!digitalRead(keyF)) {
    tone(notePin, 349.23);      // F4
  }
  else if (!digitalRead(keyFShrp)) {
    tone(notePin, 369.99);      // F#4
  }
  else if (!digitalRead(keyG)) {
    tone(notePin, 392.00);      // G4
  }
  else if (!digitalRead(keyA)) {
    tone(notePin, 440.00);      // A4
  }
  else if (!digitalRead(keyAShrp)) {
    tone(notePin, 466.16);      // A#4
  }
  else if (!digitalRead(keyB)) {
    tone(notePin, 493.88);      // B4
  }
  else if (!digitalRead(keyC5)) {
    tone(notePin, 523.25);      // C5
  }
  else {
    noTone(notePin);
  }
}
