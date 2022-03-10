#ifndef datatype_h
#define datatype_h

#include <stdio.h>

typedef struct Cmplx {
        double r;
        double i;
} Cmplx;

typedef struct FloatCmplx {
        float r;
        float i;
} FloatCmplx;

typedef struct ShortCmplx {
        short r;
        short i;
} ShortCmplx;

typedef struct LongCmplx {
        long r;
        long i;
} LongCmplx;


typedef short WAVE_TYPE;
typedef unsigned char KRL_PARAMETER;
typedef unsigned char Uchar;
typedef unsigned long Ulong;
typedef unsigned long ulong;
typedef unsigned int Uint;
typedef unsigned int uint;
typedef unsigned short Ushort;

typedef FloatCmplx FCmplx;
typedef Cmplx DbLgCmplx;
typedef Cmplx Cmplx_OR_ShortCmplx;
typedef Cmplx Cmplx_OR_ShCmplx;
typedef Cmplx DbShCmplx;
typedef double DOUBLE_OR_SHORT;
typedef double DOUBLE_OR_USHORT;
typedef double DOUBLE_OR_LONG;
typedef double DOUBLE_OR_ULONG;
typedef double DOUBLE_OR_INT;
typedef float FLOAT_OR_ULONG;
typedef float FLOAT_OR_LONG;
typedef float FLOAT_OR_SHORT;
typedef float FLOAT_OR_USHORT;
typedef float FLOAT_OR_INT;
typedef float FLOAT_OR_UINT;
typedef float POWER_TYPE;
typedef float POWER_LOG_TYPE;
typedef float BPF_TYPE;


#define BAIRITU_DB 		(1)
#define FFT_WGT_BIT    		(0)
#define FFT_WGT        		(1)
#define SHORT_WGT_BIT    	(0)
#define SHORT_WGT        	(1)

typedef struct CARG {
        int     ac;
        char**  av;
} CARG;


#endif
