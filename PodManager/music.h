typedef uint16_t (*TuneGen)(uint16_t t);

TuneGen tuneGen;

uint8_t buffer, bitsLeft;
uint16_t tuneTime;

#ifndef AB_DEVKIT
  // Arduboy speaker pin 1 = Arduino pin 5 = ATmega32u4 PC6
  #define TONE_PIN_PORT PORTC
  #define TONE_PIN_DDR DDRC
  #define TONE_PIN PORTC6
  #define TONE_PIN_MASK _BV(TONE_PIN)
  // Arduboy speaker pin 2 = Arduino pin 13 = ATmega32u4 PC7
  #define TONE_PIN2_PORT PORTC
  #define TONE_PIN2_DDR DDRC
  #define TONE_PIN2 PORTC7
  #define TONE_PIN2_MASK _BV(TONE_PIN2)
#else
  // DevKit speaker pin 1 = Arduino pin A2 = ATmega32u4 PF5
  #define TONE_PIN_PORT PORTF
  #define TONE_PIN_DDR DDRF
  #define TONE_PIN PORTF5
  #define TONE_PIN_MASK _BV(TONE_PIN)
#endif

void stopChiptune(){
    TIMSK4 = 0;
}

void playChiptune( TuneGen tg ){
    tuneGen = tg;
    tuneTime = 0;
    
    bitSet(TONE_PIN_DDR, TONE_PIN);
    TCCR4A = 0b00000000;
    TCCR4B = 0b00000100;
    OCR4C  = 0xFF;
    OCR4A  = 0x80;
    TIMSK4 = 0b00000100;
    bitsLeft = 0;    
}



//Timer
ISR(TIMER4_OVF_vect){

    if( !tuneGen || !Arduboy2Audio::enabled() )
	return;

    if( !bitsLeft ){
	buffer = tuneGen( tuneTime++ );
	bitsLeft = 8;
    }

    uint8_t bit = buffer & 1;

    if( bit ) bitSet( TONE_PIN_PORT, TONE_PIN );
    else bitClear( TONE_PIN_PORT, TONE_PIN );
    
    buffer >>= 1;
    bitsLeft--;    

} 
