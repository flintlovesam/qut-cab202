#-----| Super Awesome Makefile! -----#

CAB202_LIB_DIR="../Teensy Resources/CAB202_Teensy"
FLAGS=-mmcu=atmega32u4 -Os -DF_CPU=8000000UL -std=gnu99
LIBS=-Wl,-u,vfprintf -lprintf_flt -lcab202_teensy -lm

all:
	avr-gcc question_1.c usb_serial.c $(FLAGS) -I$(CAB202_LIB_DIR) -L$(CAB202_LIB_DIR) $(LIBS) -o question_1.o
	avr-objcopy -O ihex question_1.o question_1.hex

	avr-gcc question_2.c usb_serial.c $(FLAGS) -I$(CAB202_LIB_DIR) -L$(CAB202_LIB_DIR) $(LIBS) -o question_2.o
	avr-objcopy -O ihex question_2.o question_2.hex

	avr-gcc question_3.c usb_serial.c $(FLAGS) -I$(CAB202_LIB_DIR) -L$(CAB202_LIB_DIR) $(LIBS) -o question_3.o
	avr-objcopy -O ihex question_3.o question_3.hex

	avr-gcc question_4.c usb_serial.c $(FLAGS) -I$(CAB202_LIB_DIR) -L$(CAB202_LIB_DIR) $(LIBS) -o question_4.o
	avr-objcopy -O ihex question_4.o question_4.hex