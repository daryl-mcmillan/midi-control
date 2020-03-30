#include <avr/io.h>
#include <util/delay.h>

void writeByte( char b ) {
  while ( !( UCSR0A & (1<<UDRE0) ) );
  UDR0 = b; 
}

void start( int channel, int key, int velocity ) {
  writeByte( 0b10010000 | channel );
  writeByte( key );
  writeByte( velocity );
}

void stop( int channel, int key ) {
  writeByte( 0b10000000 | channel );
  writeByte( key );
  writeByte( 0 );
}

void programChange( int channel, int program ) {
  writeByte( 0b11000000 | channel );
  writeByte( program );
}

void controlChange( int channel, int control, int value ) {
  writeByte( 0b10110000 | channel );
  writeByte( control );
  writeByte( value );
}

void bankSelect( int channel, int msb, int lsb ) {
  controlChange( channel, 0x00, msb );
  controlChange( channel, 0x20, lsb );
}

int main(void) {

  // uart setup
  UBRR0 = 31; // 0 pause for bits - run full clock/16 rate
  UCSR0A = 0<<U2X0; // double rate - clock/8
  UCSR0B = 1<<TXEN0; // enable transmit
  UCSR0C = (0<<USBS0)|(3<<UCSZ00); // 1 stop bit, 8 data bits


  int prog = 29; // slap bass
  int bank_msb = 0x00;
  int bank_lsb = 0x70;
  int channel = 0;

  bankSelect( channel, bank_msb, bank_lsb );
  programChange( channel, prog );

  int key = 0;
  int velocity = 127;
  for( ;; ) {

    start( channel, key, velocity );
    _delay_ms(250);
    stop( channel, key );

    key = ( key + 1 ) % 128;
  }

}
