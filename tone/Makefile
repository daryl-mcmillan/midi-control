CXXFLAGS=-Os -DF_CPU=16000000UL -mmcu=atmega328p
ASFLAGS=-mmcu=atmega328p
PROCESSOR=atmega328p
LDFLAGS=-mmcu=$(PROCESSOR)
CXX=avr-gcc
CC=avr-gcc
BAUD=115200
AS=avr-as

PROGNAME=main
COMPORT=/dev/ttyACM0

$(PROGNAME).hex: $(PROGNAME)
	avr-objcopy -O ihex -R .eeprom $(PROGNAME) $(PROGNAME).hex

upload: $(PROGNAME).hex
	avrdude -v -carduino -p$(PROCESSOR) -P$(COMPORT) -b$(BAUD) -D -Uflash:w:$(PROGNAME).hex:i

main.o: main.cc

capture.o: capture.S

$(PROGNAME): main.o

clean:
	$(RM) *.hex
	$(RM) *.o
	$(RM) $(PROGNAME)
