#include <stdio.h>


unsigned char read_bit( unsigned char bitmask ) {
    unsigned char switchValue = (bitmask>>3) & 1;
    return switchValue;
}

void print_bits(unsigned char value ) {
	char bits[] = { '0', '1' };

	for ( int bit = 7; bit >= 0; bit-- ) {
		unsigned char bit_val = value;
		bit_val &= (1 << bit);
		bit_val >>= bit;
		printf( "%c", bits[bit_val] );
	}
}

int main( void ) {
	unsigned char register_val[] = {
		0x00,
		0xff,
		(1 << 3) | (1 << (3 + 1) ) | ( 1 << ( 3 - 1) ),
		(1 << (3 + 1) ) | ( 1 << ( 3 - 1) ),
	};

	for ( int i = 0; i < (sizeof(register_val) / sizeof(unsigned char)); i++ ) {
		unsigned char result = read_bit( register_val[i] );
		printf( "When register is 0x%02x, the result is ", register_val[i] );
		print_bits( result );
		printf( "\n" );
	}

	return 0;
}
