
#ifndef LOG_NATIVE_H
#define LOG_NATIVE_H

#include <time.h>

#define SPEEDTESTINIT() clock_t begin; clock_t end;

#define SPEEDTESTBEGIN() begin = clock();
#define SPEEDTESTEND() end = clock();
#define CALCSPEED() timespent = (double)(end - begin) \/ CLOCKS_PER_SEC;

struct result LOGb( double X, int Base, double epsilon );
int EQ( double A, double B, double epsilon );
int test_log_native_impl( double X );

double Root_NewtonsMethod( double a );
double quick_cosine( double angle_in_radians );


double quickexp( double base, int radix );
int factorial( int n );

#define APNATIVERANGE unsigned long long
#define NUMERICAL_DIGITS_ASCII_OFFSET 48

#endif
