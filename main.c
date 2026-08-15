#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "./lean_string.h"
#include "./aplib.h"
#include "./native.h"
#include "./ieee754.h"

int main( int argc, char** argv )   {

    if( !strcmp( argv[1], "cosine" ) )    {
        
        double a = 1.0;
        
        if( argc > 2 )
            a = atof( argv[2] );
        
        printf( "My implementation calculates cosine(%.20f) = %.20f.\n\
        The cstdlib version calculates %.20f.\n", a, quick_cosine(a), cos(a) );
        
        exit(0);
    }
    if( !strcmp( argv[1], "newtons_root") )  {
        
        char* input = "100.0";
        
        if( argc>2 )
            input = argv[2];
            
        double x = (double)atof( input );
        
        SPEEDTESTINIT();
        
        double custom_method = 0.0;
        double stdlib_method = 0.0;

        SPEEDTESTBEGIN();
        double result = Root_NewtonsMethod( x );
        SPEEDTESTEND();
        custom_method = (double) (end - begin) / CLOCKS_PER_SEC;
        
        SPEEDTESTBEGIN();
        double stdlib_result = sqrt( x );
        SPEEDTESTEND();
        stdlib_method = (double) (end - begin) / CLOCKS_PER_SEC;
        
        printf( "stdlib sqrt(%f) = %.10f\nMy method sqrt(%f) = %.10f\n", x, stdlib_result, x, result );
        printf( "stdlib version took: %f\nmy custom method took: %f\n", stdlib_method, custom_method );
        exit(0);
    }
    
    
    if( !strcmp( argv[1], "stringify_float" ) )    {
        
        // char* IEEE_readDouble( double f )
        char* input = "65535.0";
        
        if( argc>2 )
            input = argv[2];
            
        float x = (float)atof( input );

        char* result = IEEE_readFloat( x );
        
        printf( "Inputted '%f10'. Binary string is '%s'\n", x, result );
        
        float y = 0.0;
        
        //void IEEE_writeDouble(double* dest, char* str)
        IEEE_writeFloat( &y, result );

        printf( "writing back to float silo indicates value %f .\n", y );
        
        
        return 0;
    }

    if( !strcmp(argv[1], "native_log") )    {
        
        char* input = "5.0";
        if( argc>2 )
            input = argv[2];
            
        double A = (double) atof( input );
        
        test_log_native_impl( A );

        return 0;

    }


	InitAPLIB();

	char* arg1;
	char* arg2;
	char* arg3;

	if( argc < 4 )	{
		printf( "Please invoke the test app with 3 args: OPERATION LHS(A) RHS(B). Generating a default input.\n" );

		arg1 = strdup( "add" );
		arg2 = strdup( "1211" );
		arg3 = strdup( "123" );
	}
	else	{

		arg1 = strdup( argv[1] );
		arg2 = strdup( argv[2] );
		arg3 = strdup( argv[3] );
	}

	int base = 10;
	AP A = aplib->NewAP( arg2, strdup("0"), "+", &base, 128 );
	AP B = aplib->NewAP( arg3, strdup("0"), "+", &base, 128 );
	AP C = aplib->BlankAP();
	Flags extra = (Flags) calloc( 1, sizeof(int) );

	char opcode = aplib->get_opcode( arg1 );
	int emit_result = 1;

    operator = ops[ opcode ];
    
    operator( A, B, C, extra );
    
    printf( "Result Calculated.\n" );
 
	// EMIT OPERATION RESULT.
	if( emit_result )	{

		shift_left_leading_zeroes( C->wholepart );
		shift_left_leading_zeroes( C->fractpart );
		
		char* A_WP;
		char* A_FP;
		char* B_WP;
		char* B_FP;

        char* Z	= "0";	
		char* C_WP;
		char* C_FP; 
		
		if( cmpdstr(A->wholepart, "0")>0 )
		    A_WP = A->wholepart;
		else
		    A_WP = Z;

        if( cmpdstr(A->fractpart, "0")>0 )
            A_FP = A->fractpart;
        else
            A_FP = Z;
            
        if( cmpdstr(B->wholepart, "0")>0 )
            B_WP = B->wholepart;
        else
            B_WP = Z;
            
        if( cmpdstr(B->fractpart, "0")>0 )
            B_FP = B->wholepart;
        else
            B_FP = Z;
        
		if( cmpdstr(C->wholepart, "0")>0 )
		    C_WP = C->wholepart;
		else
		    C_WP = Z;
		    
		if( cmpdstr(C->fractpart, "0")>0 )
		    C_FP = C->fractpart;
		else
		    C_FP = Z;
		
		printf( "Result of '%s.%s' '%s' '%s.%s' = '%s.%s'.\n", \
		A_WP, A_FP, arg1, B_WP, B_FP, C_WP, C_FP );

	}


	aplib->FreeAP( A );
	aplib->FreeAP( B );
	aplib->FreeAP( C );

    DeInitAPLIB();
    
	free( arg1 );
	free( arg2 );
	free( arg3 );

	// EXIT.
	return 0;
}
