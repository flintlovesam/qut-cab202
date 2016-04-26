#-----| Super Awesome Makefile! -----#

CAB202_LIB_DIR="../Teensy Resources/CAB202_Teensy"
FLAGS=-mmcu=atmega32u4 -Os -DF_CPU=8000000UL -std=gnu99
LIBS=-Wl,-u,vfprintf -lprintf_flt -lcab202_teensy -lm

all:
	avr-gcc falling_faces.c usb_serial.c $(FLAGS) -I$(CAB202_LIB_DIR) -L$(CAB202_LIB_DIR) $(LIBS) -o falling_faces.o
	avr-objcopy -O ihex falling_faces.o falling_faces.hex