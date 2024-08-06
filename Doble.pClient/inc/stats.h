/************************************************************************/
/* SISCO SOFTWARE MODULE HEADER *****************************************/
/************************************************************************/
/*      (c) Copyright Systems Integration Specialists Company, Inc.,    */
/*      2016, All Rights Reserved                                       */
/*                                                                      */
/* MODULE NAME : stats.h                                                */
/* PRODUCT(S)  : MMS-EASE Lite                                          */
/*                                                                      */
/* MODULE DESCRIPTION :                                                 */
/*      Structures and functions for Stats API.                         */
/*                                                                      */
/* MODIFICATION LOG :                                                   */
/*  Date     Who                        Comments                        */
/* --------  ---  ----------------------------------------------------  */
/* 02/07/18  DSF  No C++ comments                                       */
/* 01/21/16  DSF  Initial Release                                       */
/************************************************************************/

#ifndef STATS_INCLUDED
#define STATS_INCLUDED

#if defined (_WIN32)
#pragma warning(disable : 4996)
#endif

#include "glbtypes.h"
#include "sysincs.h"
#include "mem_chk.h"

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* STATS_VALUE                                                          */
/************************************************************************/

typedef struct
  {
  ST_DOUBLE       value;
  ST_DOUBLE       timestamp;
  } STATS_VALUE;

/************************************************************************/
/* STATS_TIMING                                                         */
/************************************************************************/

typedef struct
  {
  ST_DOUBLE threshold;     /* maximum allowed */
  ST_DOUBLE stopThreshold; /* Stop if larger than this */

  ST_DOUBLE startTime;
  ST_UINT64 count;
  ST_DOUBLE sum;   /* sum of values since reset */
  ST_DOUBLE max;   /* max since reset */
  ST_DOUBLE min;   /* min since reset */
  ST_UINT64 exceededCount;

  ST_BOOLEAN periodCountEnable;
  ST_DOUBLE periodStartTime;
  ST_DOUBLE periodEndTime;
  ST_DOUBLE periodElapsedTime;
  ST_UINT64 periodCount; /* number of values since period started */
  ST_DOUBLE periodSum;   /* sum of values */
  ST_DOUBLE periodMin;   /* minimum */
  ST_DOUBLE periodMax;   /* maximum */
  ST_UINT64 periodExceededCount;

  ST_BOOLEAN recordValuesEnable;
  ST_INT    maxRecordedValues;
  ST_INT    recordedValuesCount;
  STATS_VALUE *pRecordedValues;

  /* Running Mean - StdDev */
  ST_DOUBLE oldMean;
  ST_DOUBLE newMean;
  ST_DOUBLE oldStdDev;
  ST_DOUBLE newStdDev;
  } STATS_TIMING;

ST_VOID statsTimingInitialize (STATS_TIMING *pTiming);
ST_VOID statsTimingReset (STATS_TIMING *pTiming);              /* Reset the counters, flush the recorded values */
ST_VOID statsTimingSetThreshold (STATS_TIMING *pTiming, ST_DOUBLE threshold);
ST_VOID statsTimingSetStopThreshold (STATS_TIMING *pTiming, ST_DOUBLE threshold);
ST_BOOLEAN statsTimingAddValue (STATS_TIMING *pTiming, ST_DOUBLE value);            /* Add a value */
ST_BOOLEAN statsTimingAddValueTimestamp (STATS_TIMING *pTiming, ST_DOUBLE value, ST_DOUBLE timestamp); /* Add a value w/timestamp */

/* Totals since reset */
ST_UINT64 statsTimingGetCount (STATS_TIMING *pTiming);    /* get number of values since reset */
ST_DOUBLE statsTimingGetRate (STATS_TIMING *pTiming);    /* get rate of values since reset */
ST_DOUBLE statsTimingGetSum (STATS_TIMING *pTiming);    /* get sum of values since reset */
ST_DOUBLE statsTimingGetMax (STATS_TIMING *pTiming);    /* get max since reset */
ST_DOUBLE statsTimingGetMin (STATS_TIMING *pTiming);    /* get min since reset */
ST_DOUBLE statsTimingGetMean (STATS_TIMING *pTiming);    /* get average since reset */
ST_DOUBLE statsTimingGetStdDev (STATS_TIMING *pTiming);    /* get standard deviation since reset */
ST_UINT64 statsTimingGetExceededCount (STATS_TIMING *pTiming);
ST_DOUBLE statsTimingGetExceededPercent (STATS_TIMING *pTiming);

/* Period Methods */
ST_VOID statsTimingResetPeriod (STATS_TIMING *pTiming);
ST_VOID statsTimingStartPeriod (STATS_TIMING *pTiming, ST_DOUBLE t);
ST_VOID statsTimingEndPeriod (STATS_TIMING *pTiming, ST_DOUBLE t);
ST_UINT64 statsTimingGetPeriodCount (STATS_TIMING *pTiming);  /* get number of values */
ST_DOUBLE statsTimingGetPeriodRate (STATS_TIMING *pTiming);  /* get measurement rate for period */
ST_DOUBLE statsTimingGetPeriodMax (STATS_TIMING *pTiming);  /* get max for period */
ST_DOUBLE statsTimingGetPeriodMin (STATS_TIMING *pTiming);  /* get min for period */
ST_UINT64 statsTimingGetPeriodExceededCount (STATS_TIMING *pTiming);
ST_DOUBLE statsTimingGetPeriodExceededPercent (STATS_TIMING *pTiming);


/* Value recording */
ST_VOID statsTimingStartRecordingValues (STATS_TIMING *pTiming, ST_INT maxValues);
ST_INT statsTimingStopRecordingValues (STATS_TIMING *pTiming);
ST_INT statsTimingListRecordedValues (STATS_TIMING *pTiming, ST_CHAR *pDestPath);


/************************************************************************/
/* STATS_COUNTER                                                        */
/************************************************************************/

typedef struct
  {
  ST_UINT64 count;

  ST_BOOLEAN periodCountEnable;
  ST_DOUBLE periodStartTime;
  ST_DOUBLE periodEndTime;
  ST_UINT64 periodCount; /* number of values since period started */
  } STATS_COUNTER;

ST_VOID statsCounterInitialize (STATS_COUNTER *pCounter);
ST_VOID statsCounterReset (STATS_COUNTER *pCounter);            /* Reset the counters, flush the recorded values */
ST_VOID statsCounterIncrementCount (STATS_COUNTER *pCounter, ST_INT n);        /* Increment count by n */

/* Totals since reset */
ST_UINT64 statsCounterGetCount (STATS_COUNTER *pCounter);    /* get number of values */

/* Period Methods */
ST_VOID statsCounterStartPeriod (STATS_COUNTER *pCounter, ST_DOUBLE timestamp);
ST_VOID statsCounterEndPeriod (STATS_COUNTER *pCounter, ST_DOUBLE timestamp);
ST_UINT64 statsCounterGetPeriodCount (STATS_COUNTER *pCounter);  /* get number of values */
ST_DOUBLE statsCounterGetPeriodRate (STATS_COUNTER *pCounter);   /* get number of values */


#ifdef __cplusplus
}
#endif
#endif