/*
 * Author: Nathan Krause
 * Last Major Update December 2017
 */

//variables setup
unsigned long elapsedTime;
unsigned long onTime;

int button = A8;

byte incomingByte;
byte note;
byte velocity;
int noteDown = LOW;
int count = 0;
int state = 0; // state machine variable 0 = command waiting : 1 = note waitin : 2 = velocity waiting
int baseNote = 50;  // lowest note
// use different values of baseNote to select the MIDI octiave
// 24 for octiave 1 -- 36 for octiave 2 -- 48 for octiave 3 -- 60 for octiave 4 -- 72 for octiave 5
// 84 for octiave 6 -- 96 for octiave 7
#define NCH 32
// play only notes in the key of C (that is no sharps or flats) define pin numbers:-
byte playArray[] =    { 25, 25, 24, 24, 26, 27, 27, 29, 29, 31, 31, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53};
// corrisponding to note 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48 - for base note = 36 or C2
int strobe = 13;   // select the pin for the monostable
int relayCount = 12; //number of relays we'd like to use
int channel = 0; // MIDI channel to respond to (in this case channel 2) change this to change the channel number
// MIDI channel = the value in 'channel' + 1
boolean timerFlag = false;


//setup: declaring iputs and outputs and begin serial
void setup() {
  pinMode(strobe, OUTPUT);  // declare the strobe pin as output
  onTime = millis();
  onTime = elapsedTime;
  pinMode(button, INPUT);
  for (int i = 0; i < NCH; i++) pinMode ( playArray[i], OUTPUT );

  state = 0;  // initilise state machine variable
  //start serial with MIDI baudrate 31250 or 38400 for debugging
  Serial.begin(31250);
  digitalWrite(strobe, LOW);
  digitalWrite(45, HIGH);
  digitalWrite(button, LOW);
  for ( int i = 0; i < relayCount; i++) {
    digitalWrite(playArray[i], HIGH);
  }
}

//loop: wait for serial data, and interpret the message
void loop () {
  digitalWrite( 43, LOW );
  digitalWrite( 44, LOW );
  if ( timerFlag == false ) { //time limit up and button has not been pressed set all the lights to on
    for (int i = 0; i < relayCount; i++) digitalWrite( playArray[i], HIGH );
  }
  if (digitalRead(button) == HIGH || noteCheck()) {
    onTime = millis();
    timerFlag = true;
    for ( int i = 0; i < relayCount; i++) {
      digitalWrite( playArray[i], LOW);
    }
    digitalWrite( 43, HIGH);
    while ( timerFlag ) {
      digitalWrite( 44, HIGH );
      if (Serial.available() > 0 ) {
        // read the incoming byte:

        incomingByte = Serial.read();
        digitalWrite(strobe, LOW);

        count = 0;// clear any previous strobe
        switch (state) {
          case 0:
            // look for as status-byte, our channel, note on
            if (incomingByte == (144 | channel)) {
              noteDown = HIGH;
              state = 1;
            }
            // look for as status-byte, our channel, note off
            if (incomingByte == (128 | channel)) {
              noteDown = LOW;
              state = 1;
            }
          case 1:
            // get the note to play or stop
            if (incomingByte < (128)) {
              note = incomingByte;
              state = 2;
            }
            else {
              state = 0;// reset state machine as this should be a note number
            }
            break;
          case 2:
            // get the velocity
            if (incomingByte < 128) {
              playNote(note, incomingByte, noteDown); // fire off the solenoid
            }
            state = 0; // reset state machine to start
        }//end of switch
      } //end of serial/available
      if (onTime > 0 && millis() - onTime >= 5000 && checkLights() == false ) {
        timerFlag = false;
      }
    }
  }//end of digitalRead on button
} //end of main loop

void playNote(byte note, byte velocity, int down) {
  // if velocity = 0 on a 'Note ON' command, treat it as a note off
  if ((down == HIGH) && (velocity == 0)) {
    down = LOW;
  }
  note += 1;
  note = note % relayCount;
  if ( note == 0 ) {
    note = relayCount;
  }
  note -= 1;
  byte myPin = playArray[ note ];

  digitalWrite(myPin, down); // play it if it is one of our notes
  if (down == HIGH) digitalWrite(strobe, HIGH); // strobe high to monostable for note on
  onTime = millis();


}

boolean checkLights() {
  boolean found = false;
  int val = 0;
  for ( int i = 0; i < relayCount && found == false; i++ ) {
    val = digitalRead( playArray[i] );
    if ( val == HIGH ) {
      found = true;
    }
  }
  return found;
}

boolean noteCheck() {
  boolean keyPlayed = false;
  if( Serial.read() == (144 | channel) ) {
    keyPlayed = true;
  }
  return keyPlayed;
}

