
#include <stdlib.h>
#include <string.h>


char* IEEE_readFloat( float f )	{
	
	float * tc = malloc( sizeof(float) ) ;
	
	*tc = f;
	
	char * offset = (char *) tc;
	char * str = malloc(sizeof(float)*8 + 1); // 3 for the in-between spaces, 1 for the null-terminator of the string.
	// float := 64 + 7 + 1
	
	int ptr = 0;

	for(int i=0; i<sizeof(float); i+=1 )	{ // iterate through each of the 4 bytes of the float. (change to 8 when parsing a float.)
		
		for(int k=7; k>=0; k-=1 )	{ // iterate from the MSB (bit 7), to the LSB (bit 0) of an individual byte of the memory.
			
			int r = *(offset+i) & (1 << k); // is bit 'k' of offset pointer set to 1, or not? In other words, is the &-exp TRUE, or FALSE?

			if( r==0 )
				str[ptr++] = '0';
			else
				str[ptr++] = '1';
		}
		
	}
		
	str[ptr] = '\0';
	
	return str;

}


void IEEE_writeFloat(float* dest, char* str)	{
	
#include <assert.h>

	assert( strlen(str)==(sizeof(float)*8) );
	
	char* mem = (char *)dest;
	unsigned char bitmask;
	int offset = 0;
	
	for( int i=0; i<sizeof(float); i+=1 )	{

		bitmask = 0;

		for( int k=0; k<8; k++ )
			bitmask += (str[offset+k] - '0') << (7 - k);

		(mem[i]) = bitmask;
        offset += 8;

	}
}


char* IEEE_readDouble( double f )	{
	
	double * tc = malloc( sizeof(double) ) ;
	
	*tc = f;
	
	char * offset = (char *) tc;
	char * str = malloc((sizeof(double)*8) + 1); // 3 for the in-between spaces, 1 for the null-terminator of the string.
	// float := 64 + 7 + 1
	
	int ptr = 0;

	for(int i=0; i<sizeof(double); i+=1 )	{ // iterate through each of the 4 bytes of the float. (change to 8 when parsing a float.)
		
		for(int k=7; k>=0; k-=1 )	{ // iterate from the MSB (bit 7), to the LSB (bit 0) of an individual byte of the memory.
			
			int r = *(offset+i) & (1 << k); // is bit 'k' of offset pointer set to 1, or not? In other words, is the &-exp TRUE, or FALSE?

			if( r==0 )
				str[ptr++] = '0';
			else
				str[ptr++] = '1';
		}
		
	}
		
	str[ptr] = '\0';
	
	return str;
}

void IEEE_writeDouble(double* dest, char* str)	{
	
#include <assert.h>

	assert( strlen(str)==(sizeof(double)*8) );
	
	char* mem = (char *)dest;
	unsigned char bitmask;
	int offset = 0;
	
	for( int i=0; i<sizeof(double); i+=1 )	{

		bitmask = 0;

		for( int k=0; k<8; k++ )
			bitmask += (str[offset+k] - '0') << (7 - k);

		(mem[i]) = bitmask;
        offset += 8;

	}
}

