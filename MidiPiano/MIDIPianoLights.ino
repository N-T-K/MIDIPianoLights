/*
 * Author: Nathan Krause
 * Project: Christmas MIDI Piano Light Show 
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
byte playArray[] =    { 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53};
// corrisponding to note 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48 - for base note = 36 or C2
int strobe = 13;   // select the pin for the monostable

int channel = 0; // MIDI channel to respond to (in this case channel 2) change this to change the channel number
// MIDI channel = the value in 'channel' + 1


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
  
}

//loop: wait for serial data, and interpret the message
void loop () {
/*if(digitalRead(button) == HIGH) {
  onTime = millis(); */
  
if(digitalRead(22) == HIGH && digitalRead(23) == HIGH && digitalRead(24) == HIGH
    && digitalRead(25) == HIGH && digitalRead(26) == HIGH && digitalRead(27) == HIGH
    && digitalRead(28) == HIGH && digitalRead(29) == HIGH && digitalRead(30) == HIGH
    && digitalRead(31) == HIGH && digitalRead(32) == HIGH && digitalRead(33) == HIGH
    && digitalRead(34) == HIGH && digitalRead(35) == HIGH && digitalRead(36) == HIGH
    && digitalRead(37) == HIGH && digitalRead(38) == HIGH && digitalRead(39) == HIGH
    && digitalRead(40) == HIGH && digitalRead(41) == HIGH && digitalRead(42) == HIGH
    && digitalRead(43) == HIGH && digitalRead(44) == HIGH && digitalRead(45) == HIGH
    && digitalRead(46) == HIGH && digitalRead(47) == HIGH && digitalRead(48) == HIGH
    && digitalRead(49) == HIGH && digitalRead(50) == HIGH && digitalRead(51) == HIGH
    && digitalRead(52) == HIGH && digitalRead(53) == HIGH && noteDown == HIGH)     { 

for (int i = 0; i < NCH; i++) digitalWrite( playArray[i], LOW );
}
digitalWrite( 34, HIGH );
while ( true ) {
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
        if (incomingByte < 128) {
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
    }
  }
  }
//inside main loop here
  if(onTime > 0 && millis() - onTime > 20000) {
    for (int i = 0; i < NCH; i++) digitalWrite( playArray[i], HIGH );
  }
}

void playNote(byte note, byte velocity, int down) {
  // if velocity = 0 on a 'Note ON' command, treat it as a note off
  if ((down == HIGH) && (velocity == 0)) {
    down = LOW;
  }
  //since we can't play all notes we only action notes between 36 & 49
  if (note >= baseNote && note < (baseNote + NCH)) {
    byte myPin = playArray[note - baseNote]; // to get a pin number between 2 and 9
    if (myPin != 0) {
      digitalWrite(myPin, down); // play it if it is one of our notes
      if (down == HIGH) digitalWrite(strobe, HIGH); // strobe high to monostable for note on
    }
  }

}

