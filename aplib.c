/**/

// C STDLIB INC'S
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

//#include "lean_math.h"
#include "./lean_string.h"
#include "./aplib.h"

#define APNUM AP
#define ULL unsigned long long int

/**
TABLE FOR OPTIMISING THE ARITHMETIC ALGORITHMS' REQUIREMENT TO DETERMINE A RESULT DIGIT AND A CARRYOVER DIGIT.
(A MSD IN A 2DIGIT INTERIM RESULT, e.g, 7 + 11 == 18, the carryover digit would be '1', the result digit would be '8'.)
The alternative would be to do:

r = (signed short) ( (a - b) + carry );
r = r - 48;
lsd = (r % 10);
carry = r - lsd;
carry = carry + 48;
r = lsd;
r = r + 48;

The reason for this is that the input digits are in ASCII offset, so range 48 to 57 (printable '0' to '9'),
so to do a * b or a - b with the carry digit as well, 48 has to be deducted from at least one or two of the
the interam operand digits regardless. Usefully too, later on, the overflow rows ("carry") might be made available
to an animation subroutine for visual rendering of the operation (subject to a per-interim calculation delay, so that
the sequence of interim operations can be seen happening sequentiallyn, any animation rendering the operation would need to slow the
frame-rate down for the sequence of steps to be visible to the viewer). Being in ASCII format without additional shifting & masking is kinda clean.
*/
signed char __VALUES__[ (89+19)*2 + 2 ] = {

	'1', '9', '1', '8', '1', '7', '1', '6', '1', '5', '1', '4', '1', '3', '1', '2', '1', '1', '1', '0',
	'0', '9', '0', '8', '0', '7', '0', '6', '0', '5', '0', '4', '0', '3', '0', '2', '0', '1',
	'0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
	'1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
	'2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
	'3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
	'4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
	'5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
	'6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
	'7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
	'8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9'
};

/**
This sets the pointer to the Lookup Table to the middle, as the first half of the table is for quick lookup of negative results.
For example 4 - 5 == -1. Typically, negative lookup results are in the range ( -19 <= r <= -1 ), as SUB is the only signed Operation.
Other operations have a simple method for establishing the sign without dynamic production.
*/
signed char* VALUES = __VALUES__+(19*2);

/**
Static descriptors for indexing the available public Operators.
The associated function "get_opcode" takes a string for the Op name (e.g. "add" for A+B),
and returns the descriptor for passing to the main APLIB switch-case block.
*/

#define FNC_ENTRY_SIG AP A, AP B, AP C, FLAGS extra
#define apfnc_t (AP)(*opfnc)( AP A, AP B, AP C, FLAGS extra )
#define Result result_t

const char* PI_math_h = "3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912";

static const char* Lp = "0.000000000000000000000000000000000001616255"; //( units => metres )
static const char* tp = "0.0000000000000000000000000000000000000000000539127"; //( units => secs )
static const char* Mp = "0.00000002176434"; //( units => Kg )
static const char* G  = "0.000000000066743015"; //( units => m_cubed * 1/kg * 1/(secs_squared) )
static const char* Tp = "0.00000000000000000000000000000001416784"; //( units => Kelvin )


struct aplib_t* aplib;
struct Planck* planck;
static int planck_struct_initialised = 0;
struct Planck* init_planck()    {

	if( planck_struct_initialised==1 )
		return planck;

	planck = (Planck*)calloc( 1,sizeof(struct Planck) );

	planck->Length = aplib->QuickAP( (char*)Lp );
	planck->Time   = aplib->QuickAP( (char*)tp );
    planck->Mass = aplib->QuickAP( (char*)Mp );
	planck_struct_initialised = 1;
	return planck;

}


AP e; // Euler's Constant.
AP GR; // The "Golden Ratio".
AP FGC; // The "Fine-Grain Constant." ( ~ 1/137 )
AP AP0;
AP AP1;
AP AP2;
AP AP3;
AP AP10;
AP AP16;
AP AP1_2;
AP AP1_3;
AP AP1_4;
AP AP1_5;
AP AP3_4;
AP AP4_5;
AP AP2_3;
AP AP1_7;
AP AP2_7;
AP AP22_7;
AP PI;

/* FNC DECL'S
*/

#define ASSIGNAPFROMAUTO(A, B) if( A != (AP)0 ) *A = B;

static AP QuickAP( char* val );
static AP NewAP( char* wp, char* fractpart, char* sign, int* base, signed long long int precision );
static AP SimpleAP( char* val );
static AP BlankAP();

static void FreeAllConstants();
static char* shift_ptr_past_leading_zeroes( char* );
static signed char get_opcode( char* op );

/* IEEE-754 (Native) Operator function signatures.
*/
static int EQ( double A, double B, double epsilon );
static Result benchmark(  AP(*f)( AP A, AP B, AP C, FLAGS extra ), AP A, AP B, AP C, FLAGS extra );

static Result benchmark(  AP(*f)(AP A, AP B, AP C, FLAGS extra), AP A, AP B, AP C, FLAGS extra ) {

	Result result;

	unsigned long long begin = 0;
	unsigned long long end = 0;

	begin = clock();
	f( A,B,C,extra );
	end = clock();

	result.r = begin - end;
	result.n_m.n = 1;
	result.n_m.m = 1;

	return result;
}

#ifdef COLOURLIB
static char* PrettyPrintAP( AP A )   {

    if( A==(AP)0 )
        return (char*)calloc( 1,sizeof(char*) );

	char* wp_f = "[brightgreen]";
	char* fp_f = "[brightyellow]";
	char* period_f = "[brightwhite]";
	char* reset_str = "[reset]";
	int len = lean_strlen( wp_f ) + lean_strlen( fp_f ) + lean_strlen( period_f ) + lean_strlen( reset_str );
	char* _ = (char*) malloc( lean_strlen(A->wholepart) + 1 + lean_strlen(A->fractpart) + len + 1 );

	lean_strcpy( _, wp_f );
	lean_strcat( _, A->wholepart );
	lean_strcat( _, period_f );
	lean_strcat( _, "." );
	lean_strcat( fp_f );
	lean_strcat( _, A->fractpart );
	lean_strcat( _, reset_str );

	return _;
}
#else
static char* PrettyPrintAP( AP A )    {

    if( A==(AP)0 )
        return (char*)calloc( 1,sizeof(char*) );

    char* _ = (char*)malloc( lean_strlen(A->wholepart)+1+lean_strlen(A->fractpart) + 1);    
	lean_strcat( _, A->wholepart );

	lean_strcat( _, "." );

	if( *A->fractpart ) 
	    lean_strcat( _, A->fractpart );
	else
	    lean_strcat( _, "0" );

    return _;
}
#endif

static const char* PrintAP( AP A )  {

	char* _ = (char*) malloc( lean_strlen(A->wholepart) + 1 + lean_strlen(A->fractpart) + 1 );

	while( *A->wholepart)
		*(_++) = *(A->wholepart++);

	*_ = '.';
	++_;

	while( *(A->fractpart) )
		*(_++) = *(A->fractpart++);

	*_ = '\0';

	return _;
}

static char* RandAPNumString( uint64_t maxlen );

static char* RandAPNumString( uint64_t maxlen )	{

	srand(time(NULL));

	uint64_t len = (rand() % maxlen);
	char* _ = (char*)malloc( len+1 );

	uint64_t i;
	for( i=0; i<len; i++ )
		_[i] = (rand() % 10) + '0';

	_[i] = 0;

	return _;
}

/* APLIB (Arbitrary-precision) Operator function signatures. These operators are complete for arbitrary length Integers (no fractpart).
*/
static void ADD( AP A, AP B, AP C, FLAGS extra );
static void SUB( AP A, AP B, AP C, FLAGS extra );
static void MUL( AP A, AP B, AP C, FLAGS extra );

// From below, the implementations for the operators are even more incomplete. Still testing iteratively.
static void DIV( AP A, AP B, AP C, FLAGS extra );
static void BXN( AP A, AP B, AP C, FLAGS extra );

static void Root( AP A, AP B, AP C, FLAGS extra );

static void FreeAP( AP A )  {

	if( A == (AP)0 )
		return;

	if( A->wholepart != (char*)0 )
		free( A->wholepart );

	if( A->fractpart != (char*)0 )
		free( A->fractpart );

	A->wholepart = (char*)0;
	A->fractpart = (char*)0;

	return;
}

/*
The Newton's Method root approximation functions. Only 'ap sqroot(ap)' is exposed via the aplib_t interface. The others are used by the algorithm internally (by the core 'sqroot' function), they are implementation details that do not need to be directly-accessed by the User.
*/
static AP sqrt_f( AP A )	{

	AP A_2 = (AP)0;
	aplib->mul( A, A, A_2, (int*)0 );
	AP A2x2 = (AP)0;
	aplib->sub( A_2, AP2, A2x2, (int*)0 );

	aplib->FreeAP( A_2 );
	return A2x2;
}
static AP sqrt_deriv_f( AP A )	{

	AP R;
	NULLAP(R);
	aplib->mul( A,AP2,R,0 );
	return R;
}

static int cmpap( AP A, AP B, AP C, FLAGS extra )   {

	int v = 0;
	v = cmpdigitstr( A->wholepart, B->wholepart );

	if( v>=0 )  {

		*C = *A;
	}
	else if(v<0) {

		*C = *B;
	}
	else    {

		v = cmpdigitstr( A->fractpart, B->fractpart );
		if( v>=0 )
			*C = *A;
		else
			*C = *B;
	}

	return v;
}

static uint64_t max( uint64_t a, uint64_t b );


static uint64_t max( uint64_t a, uint64_t b ) {
    
    if( a >= b )
        return a;
    
    return b;
}
static void Root( AP A, AP B, AP C, FLAGS extra )	{

	AP x0;
	NULLAP(x0);
	DIV( A,AP2, x0, extra );

	AP epsilon = aplib->QuickAP( "0.0000000001" );
	AP tolerance = aplib->QuickAP( "0.000001" );

	uint64_t max_iterations = 20;

	AP y;
	AP yprime;
	AP x1;
	AP x1_b;
	AP x1_c;

	while( max_iterations-- )	{

		y = sqrt_f( x0 );
		yprime = sqrt_deriv_f( x0 );
		AP R;
		R = aplib->BlankAP();
		if( aplib->cmpap( yprime, epsilon, R, (int*)0 ) < 0 )
			break;

		DIV( y,yprime, x1, extra );
		SUB( x0,x1, x1_b, extra );
		SUB( x1_b,x0, x1_c, extra );

		aplib->FreeAP( x0 );
		aplib->FreeAP( x1 );
		aplib->FreeAP( x1_b );
		aplib->FreeAP( y );
		aplib->FreeAP( yprime );

		AP r = (AP)0;
		if( ( aplib->cmpap( x1_c,tolerance, r, (int*)0 ) < 1 ) )  {

			*C = *x1_c;
			break;
		}


		*x0 = *x1_c;
		aplib->FreeAP( x1_c );
	}

	*C = *AP0;
	return;
}

/* Operator function implementations.
*/
static char* shift_ptr_past_leading_zeroes( char* _ )	{

	if( _==NULL )
		return NULL;

	if( *_=='\0' )
		return _;

	while( *_=='0' )
		++_;

	if( *_=='\0' )
		--_;

	return _;
}

static signed char get_opcode( char* op )	{

	if( lean_strcmp( op,"mul" ) )
		return OPMUL;

	if( lean_strcmp( op,"bxn" ) )
		return OPBXN;

	if( lean_strcmp( op,"add" ) )
		return OPADD;

	if( lean_strcmp( op,"sub" ) )
		return OPSUB;

	if( lean_strcmp( op,"div" ) )
		return OPDIV;

	if( lean_strcmp( op,"sqroot" ) )
		return OPSQROOT;

	if( lean_strcmp( op,"log" ) )
		return OPLOG;

	return -1;
}

static void ADD( AP A, AP B, AP C, FLAGS extra ) {

	char* Awp = A->wholepart;
	char* Bwp = B->wholepart;
	char* Cwp;

	uint64_t lenA = lean_strlen(A->wholepart);
	int64_t x = lenA-1;

	uint64_t lenB = lean_strlen(B->wholepart);
	int64_t y = lenB-1;

	uint64_t largest = 0;

	if( lenB>lenA )

		largest = lenB;
	else
		largest = lenA;


	uint64_t lenC = largest+1;
	if( C->wholepart == (char*)0 )  {

		C->wholepart = (char*)malloc( largest + 1 );
	}

	C->wholepart[ largest ] = '\0';
	C->wholepart[ largest-1 ] = '0';

	Cwp = C->wholepart;

	uint64_t z = lenC-1;

	unsigned char a = '0';
	unsigned char b = '0';

	unsigned char a0 = 0;
	unsigned char b0 = 0;

	unsigned char carry = '0';
	unsigned char r = '0';


ADDloop:

	// if fractpart, may need implicit trailing zeroes, so need to calc length of fractpart
	// for both A and B.
	if( a0 )
		a = '0';
	else
		a = Awp[x];

	if( b0 )
		b = '0';
	else
		b = Bwp[y];

	r = a + b + carry;
	r = r - (48 * 3);

	carry = VALUES[ r*2 ];
	Cwp[z] =  VALUES[ (r*2)+1 ];

	z--;
	x--;
	y--;

	if( x<0 )
		a0 = 1;

	if( y<0 )
		b0 = 1;


	if( a0 && b0 )	{

		Cwp[z] = carry;
	}
	else	{

		goto ADDloop;
	}


	/**
	END OF ADD BLOCK
	*/

	return;
}
static void SUB( AP A, AP B, AP C, FLAGS extra ) {

	char* Awp = A->wholepart;
	char* Bwp = B->wholepart;
	char* Cwp;

	uint64_t lenA = lean_strlen(Awp);
	int64_t x = lenA-1;

	uint64_t lenB = lean_strlen(Bwp);
	int64_t y = lenB-1;

	uint64_t lenC = max(lenA,lenB)+1;

	if( C==(AP)0 )  {

		C = (AP)calloc( 1, sizeof(struct ap) );
		C->wholepart = (char*)malloc( lenC+1 );
	}

	if( C->wholepart == (char*)0 )  {

		C->wholepart = (char*)malloc( lenC + 1 );
	}
    
    Cwp = C->wholepart;

	Cwp[ lenC ] = '\0';
	Cwp[ lenC-1 ] = '0';

	ULL z = lenC-1;

	signed char a = '0';
	signed char b = '0';

	unsigned char a0 = 0;
	unsigned char b0 = 0;

	signed char carry = '0';

	signed char r = '0';

SUBloop:

	if( a0 )
		a = '0';
	else
		a = Awp[x];

	if( b0 )
		b = '0';
	else
		b = Bwp[y];

	r = (signed short) ( (a - b) + carry );
	r = r - 48;

	if( r<0 )	{

		r = 10 + r;
		carry = '0' - 1;
	}
	else	{

		carry = VALUES[ ((r)*2) ];
	}

	Cwp[z] = VALUES[ ((r)*2)+1 ];

	--z;
	--x;
	--y;


	if( x<0 )
		a0 = 1;

	if( y<0 )
		b0 = 1;


	if( a0 && b0 )	{

		if( carry == ('0' - 1) )	{

			// result is negative.
			Cwp[z] = '-';
		}
		else
			Cwp[z] = carry;
	}
	else
		goto SUBloop;

	if( Cwp[z]=='-' )	{

		uint64_t _len = lean_strlen(Cwp);
		++Cwp;
		AP D;
		D = BlankAP();
		D->wholepart = (char*)malloc( _len+1 );

		D->wholepart[0] = '1';
		D->wholepart[1] = '\0';

		AP F; // = (AP)calloc( _len+1, sizeof(char) );
		F = aplib->BlankAP();
		F->wholepart = (char*)malloc( _len+1 );
		++F->wholepart;

		SUB( D,C,F,extra );
		--Cwp;
		--F->wholepart;
		F->wholepart[0] = '-';
		lean_strcpy( Cwp,F->wholepart );
		aplib->FreeAP( D );
		aplib->FreeAP( F );
	}
	/**
	END OF SUB BLOCK
	*/
	if( (*extra) & (OPDIV<<8) )
		;
	else
		shift_left_leading_zeroes( Cwp );

	return;
}

#define INITAP(A) AP A = (AP)calloc(sizeof(struct ap)) 

#define CHECKAP(A,lenwp,lenfp) if( A==(AP)0 ) A = BlankAP();\
if( A->wholepart==(char*)0 ) A->wholepart = (char*)calloc( lenwp+1, sizeof(char) );\
if( A->fractpart==(char*)0 ) A->fractpart = (char*)calloc( lenfp+1, sizeof(char) ); 

static void MUL( AP A, AP B, AP C, FLAGS extra ) {

	register char* Awp = A->wholepart;
	register char* Bwp = B->wholepart;
	register char* Cwp;

	int64_t lenA = lean_strlen( Awp );
	int64_t lenB = lean_strlen( Bwp );
	int64_t maxlenAB = max( lenA, lenB );
	register int64_t lenC = lenA+lenB;

    CHECKAP(C,lenC,0);

    Cwp = C->wholepart;
    Cwp[ lenC ] = '\0';

	signed char carry = 0;
	signed char a = 0;
	signed char b = 0;
	signed char c = 0;
    signed char ires_carry = 0;
    
	register signed long long int x = lenA-1;
	register signed long long int y = lenB-1;
	register signed long long int z = 0;
    signed long long int ires_msd_column = (lenC-lenB); // num cols in row 0 is 3, ||B|| 
    
	register signed long long cz = 0;
	signed long long tz = 0;

	z = cz = tz = lenA+lenB-1;

	// Loop
	while( x>=0 ) {

        a = Awp[x] & 15;
		b = Bwp[y] & 15;
		c = (a * b) + carry + Cwp[ cz ];
		
		Cwp[cz] = c % 10;
		carry = (signed char)(c - (c % 10))/10;

        
		if( carry > 8 ) fprintf( stderr, "Carry digit > 8 in MUL: %d\n", carry );

		if( y==0 )	{

            Cwp[ cz-1 ] += carry;

			/*
			Each loop of A (next digit in B), we no longer accum. the same C offset,
			it's now shunted to the left 1.
			*/

			--x;
			y = lenB-1;
			
			--z;
			cz = z;
			carry = 0;
		}
		else    {
		    
			--y;
			--cz;
		}
	}
	
	Cwp[ 0 ] = carry;
	
    while( lenC ) {
        
        Cwp[ tz-- ] |= 48;
        --lenC;
    }

	return;
}

static void DIV( AP A, AP B, AP C, FLAGS extra ) {

	char* Awp = A->wholepart;
	char* Bwp = B->wholepart;
	char* Cwp;



	uint64_t strlen_B = lean_strlen(Bwp);
	uint64_t strlen_A = lean_strlen(Awp);
	uint64_t strlen_C = strlen_A * 2;

	uint64_t strlen_REMAINDER = 0;

	char* REMAINDER = (char*)malloc( strlen_C +1 );
	char* RESULT = (char*)malloc( strlen_C+1 );
	char* NEW_REMAINDER = (char*)malloc( strlen_C +1 );
	char* _;
	char ch = '0';

	AP APREMAINDER = aplib->BlankAP();
	AP APRESULT = aplib->BlankAP();
	AP APNEWREMAINDER = aplib->BlankAP();

	APREMAINDER->wholepart = REMAINDER;
	APRESULT->wholepart = RESULT;
	APNEWREMAINDER->wholepart = NEW_REMAINDER;

	if( C==(AP)0 ) {

		C = (AP)calloc( 1, sizeof(struct ap) );
		C->wholepart = (char*)malloc( strlen_C+1 );

	}

    if( C->wholepart == (char*)0 )  {
        
        C->wholepart = (char*)malloc( strlen_C+1 );
    }

	Cwp = C->wholepart;

	Cwp[ strlen_C ] = '\0';
	*extra |= ( OPDIV << 8 );

	uint64_t x = 0;

	for( ; x<strlen_B; x++ )    {

		Cwp[x] = '0';
		REMAINDER[x] = Awp[x];
	}


	REMAINDER[x] = '\0';

	if( cmpdstr( REMAINDER, Bwp )<0 )  {

		Cwp[x] = '0';
		REMAINDER[x] = Awp[x];

		REMAINDER[x+1] = '\0';
	}
	else
		--x;

Loop:

	*extra = *extra & (65535-255);
	BXN( APREMAINDER, B, APRESULT, extra );

	SUB( APREMAINDER, APRESULT, APNEWREMAINDER, extra );

	signed diff = cmpdstr( NEW_REMAINDER, "0" );

//#include <assert.h>
	//  assert( diff>-1 );

	Cwp[x] = (char) (*extra & 255 ) + 48;
	Cwp[x+1] = '\0';
	++x;

	if( ((diff==0) && x>=strlen_A) || (x>=strlen_A*2) )  {

		free( REMAINDER );
		free( NEW_REMAINDER );
		free( RESULT );

		return;
	}

	_ = REMAINDER;
	REMAINDER = NEW_REMAINDER;
	NEW_REMAINDER = _;


	if( x >=strlen_A )
		ch = '0';
	else
		ch = Awp[x];

	if( strlen_REMAINDER == 0 )
		strlen_REMAINDER = lean_strlen(REMAINDER);
	else
		++strlen_REMAINDER;

	REMAINDER[ strlen_REMAINDER ] = ch;
	REMAINDER[ strlen_REMAINDER+1 ] = '\0';

	goto Loop;
}

static void BXN( AP A, AP B, AP C, FLAGS extra ) {

	char* Awp = A->wholepart;
	char* Bwp = B->wholepart;
	char* Cwp;

	ULL strlen_Awp = lean_strlen( A->wholepart );
	ULL strlen_Bwp = lean_strlen( B->wholepart );

	char* _;
	char* R1 = (char*)calloc( strlen_Awp+1,sizeof(char) );
	R1[0] = '0';
	char* R2 = (char*)calloc( strlen_Awp+1,sizeof(char) );
	int count = 0;

	AP APR1 = aplib->BlankAP();
	AP APR2 = aplib->BlankAP();

	APR1->wholepart = R1;
	APR1->sign = '+';
	APR2->wholepart = R2;

bxn_loop:

	ADD( APR1,B,APR2,extra );

	_ = R1;
	R1 = R2;
	R2 = _;


	if( cmpdigitstr( R1, Awp )<1 )	{

		count = count + 1;
		goto bxn_loop;

	}

	_ = R1;
	R1 = R2;
	R2 = _;

	ULL strlen_R1 = lean_strlen( R1 );

	if( C==(AP)0 )   {

		C = aplib->BlankAP();
		C->wholepart = (char*)malloc( strlen_R1 + 1 );
	}

	Cwp = C->wholepart;


	*extra = *extra | (count&255);
	strcpy( Cwp, R1 );
	aplib->FreeAP( APR1 );
	aplib->FreeAP( APR2 );

	return;
}


#ifndef EXPECT_STDLIB
/* nostdlib malloc( n ) */
void* (*malloc_fnc)( unsigned int size );
static void* (*aplib_malloc)( unsigned int size );
void* register_heap_allocator( void*(*f)(unsigned int) )   {

	aplib_malloc = f;
	return (void*) aplib_malloc;
}

/* nostdlib calloc( n,s ) */
void* (*calloc_fnc)( unsigned int numentries, unsigned int size );
static void* (*aplib_calloc)( unsigned int numentries, unsigned int size );
void* register_heap_callocator( void*(*f)(unsigned int numentries, unsigned int size) )  {

	aplib_calloc = f;
	return (void*) aplib_calloc;
}

/* nostdlib free( ptr ) */
void (*heap_free_alloc_fnc)( void* ep );
static void (*aplib_free)( void* ptr );
void* register_heap_alloc_free( void (*f)(void* ep) )  {

	aplib_free = f;
	return (void*) aplib_free;
}
#endif


static AP BlankAP() {

	return (AP)calloc( 1, sizeof(struct ap) );
}

static AP QuickAP( char* val )	{

	AP A = (AP)calloc( 1,sizeof(struct ap) );
	A->sign = '+';
	A->base = 10;

	A->wholepart = (char*)0;
	A->fractpart = (char*)0;

	unsigned long long x = 0;
	unsigned long long y = 0;
	unsigned long long z = 0;

	while( val[x] )	{

		if( val[x]=='.' )   {
		    
			A->wholepart = (char*)malloc(x + 1);
			z = x;
            break;
		}
		++x;
	}

    
	if( A->wholepart==(char*)0 )    {

		A->wholepart = strdup( val );
		A->fractpart = strdup( "0" );
	}
	else	{

		while( y<z )
			A->wholepart[y] = val[y++];

		A->wholepart[y] = '\0';

		A->fractpart = (char*)malloc( (lean_strlen(val)-y) + 1 );
		z = 0;

		while( val[y] )
			A->fractpart[z++] = val[y++];

		A->fractpart[z] = '\0';
	}

	return A;
}

AP SimpleAP( char* val )	{

	struct ap A;
	A.precision = aplib->precision;

	char* clippedval = lean_clipstring( val, A.precision );
	A.wholepart = clippedval;
	A.fractpart = strdup( "0" );
	A.sign = '+';
	A.base = 10;

	AP C = aplib->BlankAP();

	ASSIGNAPFROMAUTO(C, A);

	return C;
}


static AP CopyAP( AP A, FLAGS extra );

static AP CopyAP( AP A, FLAGS extra )   {

	AP _ = (AP)malloc( sizeof(struct ap) );

	memcpy( _, A, sizeof(struct ap) );

	return _;
}

static void LOGb( AP A, AP B, AP C, FLAGS extra );

static AP APNUMERATOR;
static AP APDENOMINATOR;
static void LOGb( AP A, AP B, AP C, FLAGS extra )	{

	AP root = B;

	AP V = aplib->CopyAP( AP1, extra );
	AP V2 = aplib->CopyAP( AP0, extra );

	AP N = aplib->CopyAP( AP1, extra );

	AP* tray = (AP*)calloc( 128, sizeof(AP) );
	AP* slider = tray;


	/*
	Stage 1 is to accumulate 2k-form n-roots of Base via multiplicative combination
	to as close to the scalar value of input X, a reasonable approximation.
	*/

	do	{

		aplib->root( root, AP2, root, extra );
		aplib->mul( N, AP2, N, extra );

		aplib->FreeAP( V2 );

		aplib->mul( V, root, V2, extra );

		if( aplib->cmpap( V2, V, (AP)0, extra ) >= 0 )
			break;

		aplib->FreeAP( V );
		*V = *V2;

		aplib->divide( AP1, N, *slider, extra );
		++slider;

	} while( 1 );


	/*
	The 2nd stage is to additivily accumulate all the n-roots 1/n radix values
	for the denominator 'n', and produce a decimal LOG(x) value.
	*/

	AP max_denom = aplib->CopyAP( N, extra );
	AP rational_numerator = aplib->CopyAP( AP0, extra );

	AP d;

	while( slider>=tray )	{

		if( (d = *slider) )	{

			AP t = (AP)0;
			aplib->divide( max_denom, d, t, extra );
			aplib->floor( t, t, extra );
			aplib->add( rational_numerator, t, rational_numerator, extra );
			aplib->FreeAP( t );
		}

		--slider;
	}

	free( tray );


	/*
	During the above 2nd stage, the code is also generating a rational n/m form
	structure for reference, to represent the actual rational for the radix (log).
	*/

	APNUMERATOR = rational_numerator;
	APDENOMINATOR = max_denom;

	if( C==(AP)0 )  {

		C = (AP)malloc( sizeof(struct ap) );
	}

	aplib->divide( APNUMERATOR, APDENOMINATOR, C, extra );
}

static void floor_ap( AP A, AP C, FLAGS extra );
static void floor_ap( AP A, AP C, FLAGS extra )   {


	AP* C_ref = (AP*)&C;
	C = *C_ref;

	C->wholepart = (char*)malloc( lean_strlen(A->wholepart) );
	lean_strcpy( C->wholepart, A->wholepart );
	C->fractpart = (char*)malloc( 2 );
	lean_strcpy( C->fractpart, "0" );
	C->sign = A->sign;
	C->base = A->base;
	C->precision = 0;
}

static char* default_desc = "AP Arbitrary-precision number.";

static AP NewAP( char* wp, char* fractpart, char* sign, int* base, signed long long int precision ) {

	AP a = (AP) malloc( sizeof(struct ap) );
    a->desc = default_desc;

    char* Awp = (char*)malloc( lean_strlen(wp)+1 );
    lean_strcpy( Awp, wp );
	a->wholepart = Awp;
	
	char* Afp = (char*)malloc( lean_strlen(fractpart)+1 );
	lean_strcpy( Afp, fractpart );
	a->fractpart = Afp;

	a->sign = *sign;
	a->base = *base;
	a->precision = precision;

	return a;
};
static void divby2( AP A, AP C, FLAGS extra ) {

	DIV( A, AP2, C, extra );
}
static unsigned char aplib_initialised = 0;
void InitAPLIB()    {


	if( aplib_initialised==0 )  {

		aplib = (struct aplib_t*)calloc( 1, sizeof(struct aplib_t) );
		aplib_initialised = 1;

		
	}

	aplib->get_opcode = get_opcode;

	aplib->FreeAP = FreeAP;
	aplib->QuickAP = QuickAP;
	aplib->NewAP = NewAP;
	aplib->QuickAP = QuickAP;
	aplib->SimpleAP = SimpleAP;
	aplib->BlankAP = BlankAP;
	aplib->CopyAP = CopyAP;

	aplib->floor = floor_ap;
	aplib->mul = MUL;
	aplib->divide = DIV;
	aplib->bxn = BXN;
	aplib->add = ADD;
	aplib->sub = SUB;
	//aplib->LOG = LOG;
	aplib->logb = LOGb;
	aplib->root = Root;
	aplib->divby2 = divby2;

    init_planck();
	aplib_initialised = 1;
	return;
}
void DeInitAPLIB()  {

	aplib_initialised = 0;

}
static void FreeAllConstants( ) {

	FreeAP( AP0 );
	FreeAP( AP1 );
	FreeAP( AP2 );
	FreeAP( AP10 );
	FreeAP( e );
	FreeAP( PI );
	FreeAP( FGC );
	FreeAP( GR );
	FreeAP( AP3 );
	FreeAP( AP16 );
	FreeAP( AP1_2 );
	FreeAP( AP1_3 );
	FreeAP( AP1_4 );
	FreeAP( AP1_5 );
	FreeAP( AP3_4 );
	FreeAP( AP4_5 );
	FreeAP( AP2_3 );
	FreeAP( AP1_7 );
	FreeAP( AP2_7 );
	FreeAP( AP22_7 );
	FreeAP( PI );

	return;
}
