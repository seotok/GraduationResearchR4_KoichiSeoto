#ifndef _const_h_
#define _const_h_

#define PI		(3.141592654)
#define PI2		(PI*2)
#define LOG10_2		(0.301030)
#define EMPHASIS	(0.97)

#define HANNING         (0)
#define HAMMING         (1)
#define RECT            (2)
#define HALFRECT        (3)
#define BLACKMAN        (4)
#define TRIANGLE        (5)

/* overlap-add で元に戻るための波形拡大率 */
#define RC_HANNING      (2.0)
#define RC_HAMMING      (2.0)
#define RC_RECT         (1.0)
#define RC_HALFRECT     (2.0)

#endif
