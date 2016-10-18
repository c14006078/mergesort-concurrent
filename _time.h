//#ifndef _TIME_H//FIXME: I cannot use it, it will omit all
//#define _TIME_H

#include <time.h>
#include <sys/time.h>

#include "debug.h"

/*man clock_gettime()*/
#define CLOCK_ID CLOCK_REALTIME
///Still other " CLOCK_REALTIME_COARSE CLOCK_MONOTONIC CLOCK_MONOTONIC_RAW "

/**
 *	struct timespec {
 *		time_t   tv_sec; //sec
 *		long     tv_nsec;//nanosec
 * 	};
 */

///FIXME:I think the `static` useless in here
//static inline __attribute__((always_inline))
void gettime(struct timespec *tp)
{
    /*int clock_gettime(clockid_t clk_id, struct timespec *tp);*/
    syserr(clock_gettime(CLOCK_ID, tp) < 0, "clock_gettime");
}

//static inline __attribute__((always_inline))
long diff_in_us(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec * 1000000.0 + diff.tv_nsec / 1000.0);
}

//#endif
