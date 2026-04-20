#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "./lean_string.h"
#include "./aplib.h"

int main( int argc, char** argv )   {

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
	AP B = aplib->NewAP( arg3,  strdup("0"), "+", &base, 128 );
	AP C = (AP)0;
	FLAGS extra = (FLAGS) calloc( 1, sizeof(int) );

	char opcode = aplib->get_opcode( arg1 );
	unsigned char emit_result = 1;

	// OPERATOR SWITCH-BLOCK
	switch( opcode )	{

    	case -1:
    		printf( "Opcode not recognized. You entered '%s'. Exiting.\n", argv[1] );
    		emit_result = 0;
    
    		break;
    
    
    	case 0:
    		printf( "Opcode not yet implemented. Exiting.\n" );
    		emit_result = 0;
    
    		break;

    
    	case OPADD:
    	    C = (AP)calloc( 1,sizeof(struct ap) );
    		aplib->add( A, B, C, extra );
    
    		break;
    
    
    	case OPSUB:
            C = aplib->BlankAP();
    		aplib->sub( A, B, C, extra );
    
    		break;

    
    	case OPMUL:
    	    C = (AP)calloc( 1,sizeof(struct ap) );
    		aplib->mul( A,B,C,extra );
    
    		break;
    
    	case OPDIV:
            C = aplib->BlankAP();
    		aplib->divide( A,B,C, extra );
    		emit_result = 1;
    
    		uint64_t strlen_A = lean_strlen(A->wholepart);
    		uint64_t strlen_C = lean_strlen(C->wholepart);
    
    		if( strlen_C > strlen_A )   {
    
    			AP D = (AP)malloc( sizeof(struct ap) );
    			D->wholepart = (char*)malloc( strlen_C+2+1 +1 );
    			D->fractpart = (char*)malloc( strlen_C+2+1 +1 );
    
    			char t = C->wholepart[ strlen_A ];
    			C->wholepart[ strlen_A ] = '\0';
    			lean_strcpy( D->wholepart, C->wholepart );
    
    			C->wholepart[ strlen_A ] = t;
    
    			lean_strcpy( D->fractpart, C->wholepart+strlen_A );
    
    			aplib->FreeAP( C );
    			C = D;
    		}
    
    		break;
    
    	case OPBXN:
    		if( cmpdstr( A->wholepart,B->wholepart )<0 )	{
    
    			fprintf( stdout, "A is smaller than B. Swap? (y/n) :" );
    			fflush( stdout );
    			char ch = getc( stdin );
    			if( !(ch=='y' || ch==' ') )	{
    
    				fflush( stdout );
    				printf( "Exiting.\n" );
    
    				//C = (AP)calloc( 1,1 );
    				C = aplib->QuickAP( "" );
    
    				emit_result = 0;
    				break;
    			}
    
    			AP _;
    			_ = A;
    			A = B;
    			B = _;
    		}
        
        		C = (AP)0;
        		aplib->bxn( A, B, C, extra );
    
    		AP REMAINDER = aplib->QuickAP( "" ); // = (AP)calloc( strlen(A)+1, sizeof(char) );
    		free( REMAINDER->wholepart );
    		REMAINDER->wholepart = (char*)malloc( lean_strlen(A->wholepart + 1) );
    
    		aplib->sub( A,C,REMAINDER,extra );
    
    		printf( "BxN specfic: REMAINDER ( A %% B ) == '%s'.\n", aplib->PrintAP(REMAINDER) );
    		aplib->FreeAP( REMAINDER );
    
    		break;


    	default:
    		//C = (AP)calloc( 2,sizeof(char) );
    		*C = *AP0;
    		emit_result = 0;
    		break;
	}


	// EMIT OPERATION RESULT.
	if( emit_result )	{

		shift_left_leading_zeroes( C->wholepart );
		shift_left_leading_zeroes( C->fractpart );
		printf( "Result of '%s' '%s' '%s' == '%s'.\n", \
		A->wholepart, arg1, B->wholepart, C->wholepart );

	}


	aplib->FreeAP( A );
	aplib->FreeAP( B );
	aplib->FreeAP( C );

	free( arg1 );
	free( arg2 );
	free( arg3 );

	// EXIT.
	return 0;
}
