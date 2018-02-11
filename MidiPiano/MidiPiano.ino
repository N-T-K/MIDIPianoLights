igned long elapsedTime;
unsigned long onTime;

int button = A8;

byte midiByte;
byte note;
byte velocity;
byte modRelay;
int noteStatus = LOW;
int count = 0;
int state = 0;
int CHANNEL = 0;
boolean notPlayed = true;


int strobe = 13;
byte relays[] = { 22, 23, 24, 26, 27, 28, 29 };

void setup() {
    pinMode( strobe, OUTPUT );
      pinMode( button, INPUT );
        onTime = millis();
          onTime = elapsedTime;
            pinMode(45, OUTPUT);
              pinMode(44, OUTPUT);
                for ( int i = 0; i < sizeof(relays); i++ ) { //declare each
                  relay as an output
                        pinMode( relays[i], OUTPUT );
                    }
                  Serial.begin(31250); //start serial with MIDI baudrate 31250 or 38400 for debugging
                      digitalWrite( strobe, LOW );

                    digitalWrite( 45, HIGH );
                      digitalWrite( 44, HIGH);
}


  

void loop() {
    
    if( Serial.available() > 0) { //returns the # of bytes in the buffer 
        
          midiByte = Serial.read();
              
              if( state == 0 ) { //expecting a note on/off command
                      if( midiByte == (144 | CHANNEL ) ) { //bitwiseOR modifies
                        our on byte to the correct expected channel
                                  noteStatus = HIGH;
                                state = 1; //we're expecting a channel byte next
                                      } else if( midiByte == ( 128 | CHANNEL ) )
                                        { //same as before but with the off byte
                                                  noteStatus = LOW;
                                                          state = 1; //expecting
                                                          a channel byte next
                                                                  } 
                                          }

                  if( state == 1 ) { //looking for a channel byte
                          if( midiByte < 128 ) { //byte should be 0-127 (min/max
                            channel)
                                      note = midiByte; //assign the byte to our note value
                                      state = 2; //looking for a velocity value now 
                                    } else {
                                              state = 0; //wrong value startover
                                                    }
                              }

                      if( state == 2 ) { //looking for a velocity value
                              if ( midiByte < 128 ) {
                                        velocity = midiByte; //assign byte to velocity
                                                  play( noteStatus, note,velocity ); //method call  (I'm from java as you can see) 
                              }
                                    state = 0;
                                        }
                      } //end of if
} //end of loop

void play( int noteStatus, byte note, byte velocity ) {
    modRelay = note % 7;
      if( modRelay == 0 ) {
            modRelay = 7;
              }
        if(  (noteStatus == HIGH) && (velocity == 0) ) {
              noteStatus = LOW;
                }
          digitalWrite( relays[modRelay - 1], noteStatus );
            digitalWrite( strobe, noteStatus );
}




