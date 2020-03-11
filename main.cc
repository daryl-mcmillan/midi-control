#include <avr/io.h>
#include <util/delay.h>

void writeByte( char b ) {
  while ( !( UCSR0A & (1<<UDRE0) ) );
  UDR0 = b; 
}

int main(void) {

  // uart setup
  UBRR0 = 31; // 0 pause for bits - run full clock/16 rate
  UCSR0A = 0<<U2X0; // double rate - clock/8
  UCSR0B = 1<<TXEN0; // enable transmit
  UCSR0C = (0<<USBS0)|(3<<UCSZ00); // 1 stop bit, 8 data bits

  char count = 0;
  for( ;; ) {

    writeByte( 0x00 );
    writeByte( 0x01 );
  }

}
