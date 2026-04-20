/* APLIB_H */

#ifndef APLIBI_H
#define APLIBI_H

#include <stdint.h>

#define OPMUL 1
#define OPADD 2
#define OPSUB 3
#define OPBXN 4
#define OPDIV 5
#define OPSQROOT 0
#define OPLOG 0

/* 'nostdlib' (0-dependency) heap allocate/free function signatures. */
extern void* (*malloc_fnc)( unsigned size );
extern void* (*calloc_fnc)( unsigned numentries, unsigned size );
extern void (*heap_free_alloc_fnc)( void* ep );

extern void* register_heap_allocator( void*(*f)(unsigned) );
extern void* register_heap_callocator( void*(f)(unsigned, unsigned) );
extern void* register_heap_alloc_free( void(*_free)(void*) );


#ifndef real
#ifdef aplib_native_double_real
#define real double
#else
#define real float
#endif
#endif

typedef unsigned long long PRECISION;

typedef struct ap	{

	char* wholepart;
	char* fractpart;
	char* desc;
	PRECISION precision;

	uint32_t type; // Integer, Real, Radians, Degrees, etc...
	uint32_t base;

	char sign;

} ap;

#define ap_bool unsigned char

typedef struct ap ap_obj;
typedef ap_obj* AP;

typedef struct aplib_rational_t {

	int n;
	int m;

} aplib_rational_t;
#define rational_t aplib_rational_t

typedef struct aplib_result_t {

	double r;
	struct aplib_rational_t n_m;

} aplib_result_t;
#define result_t aplib_result_t

#define FLAGS int*
typedef struct aplib_t {

	PRECISION precision;
	int default_base;

	AP (*SimpleAP)( char* val );
	AP (*QuickAP)( char* str );
	AP (*NewAP)( char* wholepart, char* fractpart, char* sign, int* Base, signed long long int precision );
    AP (*CopyAP)( AP A, FLAGS extra );
	void (*FreeAP)( AP A );
	AP (*BlankAP)();
	char*(*PrintAP)( AP A );

	int (*getBase)(  AP A );
	void (*setBase)(  AP A, int Base );
	char (*getSign)(  AP A );
	void (*setSign)(  AP A, char* s );

	PRECISION (*getPrecision)(  AP A );
	void (*setPrecision)(  AP A, PRECISION P );

	signed char (*get_opcode)( char* );

	void (*add)( AP A, AP B, AP C, FLAGS extra );
	void (*sub)( AP A, AP B, AP C, FLAGS extra );
	void (*mul)( AP A, AP B, AP C, FLAGS extra );
	void (*divide)( AP A, AP B, AP C, FLAGS extra );
	void (*bxn)( AP A, AP B, AP C, FLAGS extra );
	void (*divby2)( AP A, AP C, FLAGS extra );
	void (*reciprocal)( AP A, AP B, AP C, FLAGS extra );

	void (*power)( AP A, AP B, AP C, FLAGS extra );
	void (*root)( AP A, AP B, AP C, FLAGS extra );

	void (*log10)( AP A, AP C, FLAGS extra );
	void (*log2)( AP A, AP C, FLAGS extra );
	void (*log16)( AP A, AP C, FLAGS extra );
	void (*nlog)( AP A, AP C, FLAGS extra );
	void (*logb)( AP A, AP B, AP C, FLAGS extra );

	signed short (*cmpdigitstr)( char* dstr1, char* dstr2 );
	signed short (*cmpap)( AP A, AP B, AP C, FLAGS extra );
	void (*diff)( AP A, AP B, AP C, FLAGS extra );

    void (*floor)( AP A, AP C, FLAGS extra );

	void (*cos)( AP R, AP C, FLAGS extra );
	void (*sine)( AP R, AP C, FLAGS extra );
	void (*tan)( AP R, AP C, FLAGS extra );
	void (*PI)( PRECISION P, FLAGS extra ); // if precision P==0, the precision is based on the current setting for aplib->precision

} aplib_t;

typedef struct Planck   {

	AP Time;
	AP Length;
	AP Mass;
	AP Energy;

	AP Constant;
	AP ReducedConstant;

} Planck;


extern struct aplib_t* aplib;
extern struct Planck* planck;

extern void InitAPLIB();
extern void DeInitAPLIB();
#define NULLAP(A) A = (AP)0


// Extra helper objects.
#include "./aplib_extra_typedefs.h"


#ifndef PIE
#define PIE PI
#endif

extern char* desc;
/* "DAVE'S APLIB. Arbitrary-precision mathematical tools, from Arithmetic to Trig functions (such as SINE and COS), working with PI, LOGARITHMS & ROOTS etc. Facilities for working within IEEE754 mode, and arbitrary-precision emulation of IEEE754 floats and doubles." */


/*
Some helpful symbols for calling the arithemtic operator functions.
*/

#ifndef RECIPROCAL
#define RECIPROCAL(a) (a=DIVD( AP1,a ))
#endif

#ifndef GETDIGIT
#define GETDIGIT (a,x) ( x<strlen(a->wholepart)?(a->wholepart[x]):(a->fractpart[x-strlen(a->wholepart)]) )
#endif

/**/

#endif
