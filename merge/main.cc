#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static volatile uint8_t next_index;
static volatile uint8_t buffer[256];

void setup() {

  // input and pull up on D2 to D7
  DDRD &= 0b00000011;
  PORTD |= 0b11111100;

  // timer 1 at 16 MHz, CTC
  TCCR1A = 0;
  TCCR1B = (1<<CS10) | (1<<WGM12);
  TCCR1C = 0;

  // timer 1 interrupt at 171 ( 3 * 31250 Hz )
  OCR1A = 171;
  TIMSK1 |= 1 << OCIE1A;

  sei();
}

ISR(TIMER1_COMPA_vect) {
  buffer[ next_index++ ] = PIND;
}

#define BIT_0 0
#define BIT_1 0x80

#define CAPTURE_BIT_MASK  0b01000000
#define CAPTURE_BYTE_MASK 0b00100000
#define ANY_ACTION_MASK   0b01100000

#define CAPTURE_BIT_0 ( CAPTURE_BIT_MASK | 0 )
#define CAPTURE_BIT_1 ( CAPTURE_BIT_MASK | 1 )
#define CAPTURE_BIT_2 ( CAPTURE_BIT_MASK | 2 )
#define CAPTURE_BIT_3 ( CAPTURE_BIT_MASK | 3 )
#define CAPTURE_BIT_4 ( CAPTURE_BIT_MASK | 4 )
#define CAPTURE_BIT_5 ( CAPTURE_BIT_MASK | 5 )
#define CAPTURE_BIT_6 ( CAPTURE_BIT_MASK | 6 )
#define CAPTURE_BIT_7 ( CAPTURE_BIT_MASK | 7 )

#define INITIAL 0
#define IDLE_1 1
#define IDLE_11 2
#define START_0 3
#define START_00 4

#define GAP_0a 5
#define GAP_0b 6
#define GAP_1a 7
#define GAP_1b 8
#define GAP_2a 9
#define GAP_2b 10
#define GAP_3a 11
#define GAP_3b 12
#define GAP_4a 13
#define GAP_4b 14
#define GAP_5a 15
#define GAP_5b 16
#define GAP_6a 17
#define GAP_6b 18
#define GAP_7a 19
#define GAP_7b 20

#define STOP_GAP_0 21
#define STOP_GAP_00 22
#define STOP_1 23
#define STOP_11 ( CAPTURE_BYTE_MASK | 0 )

uint8_t stateMachine[256];

void setupStateMachine() {
  // default to resetting state
  for( int i=0;i<128; i++ ) {
    stateMachine[ i | BIT_0 ] = INITIAL;
    stateMachine[ i | BIT_1 ] = IDLE_1;
  }

  stateMachine[ INITIAL | BIT_1 ] = IDLE_1;
  stateMachine[ IDLE_1 | BIT_1 ] = IDLE_11;
  stateMachine[ IDLE_11 | BIT_1 ] = IDLE_11;
  stateMachine[ IDLE_11 | BIT_0 ] = START_0;
  stateMachine[ START_0 | BIT_0 ] = START_00;

  stateMachine[ START_00 | BIT_0 ] = GAP_0a;
  stateMachine[ START_00 | BIT_1 ] = GAP_0a;
  stateMachine[ GAP_0a | BIT_0 ] = GAP_0b;
  stateMachine[ GAP_0a | BIT_1 ] = GAP_0b;
  stateMachine[ GAP_0b | BIT_0 ] = CAPTURE_BIT_0;
  stateMachine[ GAP_0b | BIT_1 ] = CAPTURE_BIT_0;

  stateMachine[ CAPTURE_BIT_0 | BIT_0 ] = GAP_1a;
  stateMachine[ CAPTURE_BIT_0 | BIT_1 ] = GAP_1a;
  stateMachine[ GAP_1a | BIT_0 ] = GAP_1b;
  stateMachine[ GAP_1a | BIT_1 ] = GAP_1b;
  stateMachine[ GAP_1b | BIT_0 ] = CAPTURE_BIT_1;
  stateMachine[ GAP_1b | BIT_1 ] = CAPTURE_BIT_1;

  stateMachine[ CAPTURE_BIT_1 | BIT_0 ] = GAP_2a;
  stateMachine[ CAPTURE_BIT_1 | BIT_1 ] = GAP_2a;
  stateMachine[ GAP_2a | BIT_0 ] = GAP_2b;
  stateMachine[ GAP_2a | BIT_1 ] = GAP_2b;
  stateMachine[ GAP_2b | BIT_0 ] = CAPTURE_BIT_2;
  stateMachine[ GAP_2b | BIT_1 ] = CAPTURE_BIT_2;

  stateMachine[ CAPTURE_BIT_2 | BIT_0 ] = GAP_3a;
  stateMachine[ CAPTURE_BIT_2 | BIT_1 ] = GAP_3a;
  stateMachine[ GAP_3a | BIT_0 ] = GAP_3b;
  stateMachine[ GAP_3a | BIT_1 ] = GAP_3b;
  stateMachine[ GAP_3b | BIT_0 ] = CAPTURE_BIT_3;
  stateMachine[ GAP_3b | BIT_1 ] = CAPTURE_BIT_3;

  stateMachine[ CAPTURE_BIT_3 | BIT_0 ] = GAP_4a;
  stateMachine[ CAPTURE_BIT_3 | BIT_1 ] = GAP_4a;
  stateMachine[ GAP_4a | BIT_0 ] = GAP_4b;
  stateMachine[ GAP_4a | BIT_1 ] = GAP_4b;
  stateMachine[ GAP_4b | BIT_0 ] = CAPTURE_BIT_4;
  stateMachine[ GAP_4b | BIT_1 ] = CAPTURE_BIT_4;

  stateMachine[ CAPTURE_BIT_4 | BIT_0 ] = GAP_5a;
  stateMachine[ CAPTURE_BIT_4 | BIT_1 ] = GAP_5a;
  stateMachine[ GAP_5a | BIT_0 ] = GAP_5b;
  stateMachine[ GAP_5a | BIT_1 ] = GAP_5b;
  stateMachine[ GAP_5b | BIT_0 ] = CAPTURE_BIT_5;
  stateMachine[ GAP_5b | BIT_1 ] = CAPTURE_BIT_5;

  stateMachine[ CAPTURE_BIT_5 | BIT_0 ] = GAP_6a;
  stateMachine[ CAPTURE_BIT_5 | BIT_1 ] = GAP_6a;
  stateMachine[ GAP_6a | BIT_0 ] = GAP_6b;
  stateMachine[ GAP_6a | BIT_1 ] = GAP_6b;
  stateMachine[ GAP_6b | BIT_0 ] = CAPTURE_BIT_6;
  stateMachine[ GAP_6b | BIT_1 ] = CAPTURE_BIT_6;

  stateMachine[ CAPTURE_BIT_6 | BIT_0 ] = GAP_7a;
  stateMachine[ CAPTURE_BIT_6 | BIT_1 ] = GAP_7a;
  stateMachine[ GAP_7a | BIT_0 ] = GAP_7b;
  stateMachine[ GAP_7a | BIT_1 ] = GAP_7b;
  stateMachine[ GAP_7b | BIT_0 ] = CAPTURE_BIT_7;
  stateMachine[ GAP_7b | BIT_1 ] = CAPTURE_BIT_7;

  stateMachine[ CAPTURE_BIT_7 | BIT_0 ] = STOP_GAP_0;
  stateMachine[ CAPTURE_BIT_7 | BIT_1 ] = STOP_GAP_0;
  stateMachine[ STOP_GAP_0 | BIT_0 ] = STOP_GAP_00;
  stateMachine[ STOP_GAP_0 | BIT_1 ] = STOP_1;
  stateMachine[ STOP_GAP_00 | BIT_1 ] = STOP_1;
  stateMachine[ STOP_1 | BIT_1 ] = STOP_11;
  stateMachine[ STOP_11 | BIT_0 ] = START_0;
  stateMachine[ STOP_11 | BIT_1 ] = IDLE_11;
}

class Serial {
  uint8_t state;
  uint8_t byte;
  public:
    void initialize() {
      state = INITIAL;
    }
    void addBit( uint8_t bit ) {
      state = stateMachine[ state | bit ];
      if( (state & ANY_ACTION_MASK) == 0 ) {
        return;
      }
      if( state & CAPTURE_BIT_MASK ) {
        byte = ( byte >> 1 ) | bit;
        return;
      }
      if( state & CAPTURE_BYTE_MASK ) {
        switch( byte ) {
          case 0b10010000:
            PORTC = 0xFF;
            return;
          case 0b10000000:
            PORTC = 0x00;
            return;
          default:
            return;
        }
      }
    }
};

int main(void) {

  DDRB = 0xFF;
  PORTB = 0x00;

  setup();
  setupStateMachine();


  uint8_t index = 0;
  Serial serial1, serial2, serial3, serial4;
  serial1.initialize();
  serial2.initialize();
  serial3.initialize();
  serial4.initialize();
  for( ;; ) {
    PORTB = 0xFF;
    while( index == next_index ) {}
    PORTB = 0x00;
    uint8_t sample = buffer[index++];

    uint8_t bit = sample & 0x80;
    serial1.addBit( bit );

    sample = sample << 1;
    bit = sample & 0x80;
    serial2.addBit( bit );

    sample = sample << 1;
    bit = sample & 0x80;
    serial3.addBit( bit );

    sample = sample << 1;
    bit = sample & 0x80;
    serial4.addBit( bit );

  }
}
