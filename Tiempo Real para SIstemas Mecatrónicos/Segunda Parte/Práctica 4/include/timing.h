/*
 * File: timing.h
 *
 * This file is part of the simusil library
 * Author: Sergio Romero Montiel <sromero@uma.es>
 *
 * Created on October 31th, 2016
 */

#ifndef _TIMING_H_
#define _TIMING_H_

/* TIME DEFS */
#define NSECS_PER_SEC 1000000000UL
#define TS_1us   ((struct timespec){0,     1000})
#define TS_10us  ((struct timespec){0,    10000})
#define TS_100us ((struct timespec){0,   100000})
#define TS_1ms   ((struct timespec){0,  1000000})
#define TS_10ms  ((struct timespec){0, 10000000})
#define TS_25ms  ((struct timespec){0, 25000000})
#define TS_100ms ((struct timespec){0,100000000})

/* static inline functions */
/* should be defined here (.h) or in .c file? */
static inline long diff_ts(struct timespec a, struct timespec b)
{
  return ((a.tv_sec-b.tv_sec)*NSECS_PER_SEC+(a.tv_nsec-b.tv_nsec));
}
static inline double diff_ts_d(struct timespec a, struct timespec b)
{
  return ((double)(a.tv_sec-b.tv_sec)+(double)(a.tv_nsec-b.tv_nsec)/NSECS_PER_SEC);
}
static inline struct timespec sub_ts(struct timespec a, struct timespec b)
{
  struct timespec t;
  t.tv_sec=a.tv_sec-b.tv_sec;
  t.tv_nsec=a.tv_nsec-b.tv_nsec;
  if (t.tv_nsec<0)
  {
    t.tv_nsec+=NSECS_PER_SEC;
    t.tv_sec--;
  }
  return t;
}
static inline struct timespec add_ts(struct timespec a, struct timespec b)
{
  struct timespec t;
  t.tv_sec=a.tv_sec+b.tv_sec;
  t.tv_nsec=a.tv_nsec+b.tv_nsec;
  if (t.tv_nsec>NSECS_PER_SEC)
  {
    t.tv_nsec-=NSECS_PER_SEC;
    t.tv_sec++;
  }
  return t;
}
static inline struct timespec dtots(double a)
{
  struct timespec t;
  t.tv_sec=(long)(a+0.5e-9); /* half the epsilon of ts (round!)*/
  t.tv_nsec=(a-t.tv_sec)*NSECS_PER_SEC;
  return t;
}
static inline long ts_nsec(struct timespec a)
{
  return (a.tv_sec*NSECS_PER_SEC+a.tv_nsec);
}
static inline double ts_sec(struct timespec a)
{
  return ((double)a.tv_sec+(double)a.tv_nsec/NSECS_PER_SEC);
}

#endif /* _TIMING_H_ */
