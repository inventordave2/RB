// LOGb( X )

/*
A Lean approximator for:

Y = LOGb( X );

When tested on onlinegdb.com, it can be argued to be technically more
accurate than the C stdlib implememted there, as my function doesn't
round the l.s.d.
*/
	
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "./log_native.h"


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef struct rational {

	int n;
	int m;

} rational;

typedef struct result {

	double r;
	struct rational n_m;

} result;


int test_log_native_impl( double X ) {


    #include <time.h>
    clock_t begin = clock();
    //
    struct result Y = LOGb( X, 10, 0.00001 );
    //
    clock_t end = clock();
    double time_spent_lean = (double)(end - begin) / CLOCKS_PER_SEC;
    
    begin = clock();
    //
    double Z = log10( X );
    //
    end = clock();
    double time_spent_stdlib = (double)(end - begin) / CLOCKS_PER_SEC;

	
	printf( "C stdlib log(%f) == %f\nThe lean approximator for log(%f) gives %f\nThe rational approximation gives %d/%d\n", X, Z, X, Y.r, Y.n_m.n, Y.n_m.m );

	printf( "\n" );
	printf( "My lean function took %f time.\nThe stdlib log10() fnc took %f time.\n", time_spent_lean, time_spent_stdlib );
	
	exit(0);

}

int EQ( double A, double B, double epsilon )	{

	if( A == B )
		return TRUE;

	if( A>B )
		if( A-B < epsilon )
			return TRUE;

	if( B>A )
		if( B-A < epsilon )
			return TRUE;

	return FALSE;
}

struct result LOGb( double X, int Base, double epsilon )	{
	
	double root = (double)Base;
	double V = 1.0;
	double V2 = 0.0;
	
	int N = 1;

	int* tray = (int*)calloc( 128, sizeof(int) );
	int* slider = tray;


	/*
	Stage 1 is to accumulate 2k-form n-roots of Base via multiplicative combination
	to as close to the scalar value of input X, a reasonable approximation.
	*/

	do	{
		
		root = sqrt( root );
		N *= 2;
		
		V2 = V * root;			

		if( V2 > X )
			continue;
	
		V = V2;

		*slider = N;
		++slider;
		
	} while( ! EQ(V, X, epsilon) );


	/*
	The 2nd stage is to additivily accumulate all the n-roots 1/n radix vales
	for the denominator 'n', and produce a decimal LOG(x) valuw.
	*/

	int max_denom = N;
	int rational_numerator = 0;
	double R = 0.0;
	int d = 0;
	
	while( slider>=tray )	{
		
		if( (d = *slider) )	{
	
			R += ( 1.0/(double)d );
			rational_numerator += floor( max_denom / d );
		}

		--slider;
	}

	free( tray );

	
	/*
	During the above 2nd stage, the code is also generating a rational n/m form
	structure for reference (to represent the actual rational for the radix (log).
	*/
	struct result y_envelope;
	struct rational n_m;

	n_m.n = rational_numerator;
	n_m.m = max_denom;

	y_envelope.r = R;
	y_envelope.n_m = n_m;

	
	return y_envelope;
}



/* Newton's Method for SQROOT approximation. */

/* Internal (to algorithm) derivative functions. Tjese are for internal use. */

static double sqrt_f( double a ) {

    double a2 = 0.0;
    a2 = a*a;
    
   double a2x2 = 0.0;
   a2x2 = a2 - 2.0;

   return a2x2;
}

static double sqrt_deriv_f( double a )	{

	double r = 0.0;
	r = a * 2;
	return r;
}


/* The core Newtons Method loop. */

double Root_NewtonsMethod( double a )   {
    
    
    double x = a / 2.0;
    double prev_x = 0.0;
    double diff = 0.0;
    
    int max_iterations = 20;
    double tolerance = 0.000000001;
    
    // Iteration loop
    while( max_iterations-- )   {

        prev_x = x;
        x = 0.5 * (x + (a/x) );  // Newton iteration formula
        
        diff = x - prev_x;
        if( diff < 0.0 )
            diff = 0.0 - diff;
        
        if( diff < tolerance )  {

            break;
        }
        
    }

    return x;
}

/*

    
	while( max_iterations-- )	{

		y = sqrt_f( x0 );
		yprime = sqrt_deriv_f( x0 );

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


	return;
}

*/

