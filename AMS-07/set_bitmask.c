#include <stdio.h>


unsigned char set_bitmask( void ) {
    return 0b01001001;
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
	unsigned char result = set_bitmask();
	printf( "The result is " );
	print_bits( result );
	printf( "\n" );
	return 0;
}
