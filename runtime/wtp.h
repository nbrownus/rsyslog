/* Definition of the worker thread pool (wtp) object.
 *
 * Copyright 2008 Rainer Gerhards and Adiscon GmbH.
 *
 * This file is part of the rsyslog runtime library.
 *
 * The rsyslog runtime library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * The rsyslog runtime library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the rsyslog runtime library.  If not, see <http://www.gnu.org/licenses/>.
 *
 * A copy of the GPL can be found in the file "COPYING" in this distribution.
 * A copy of the LGPL can be found in the file "COPYING.LESSER" in this distribution.
 */

#ifndef WTP_H_INCLUDED
#define WTP_H_INCLUDED

#include <pthread.h>
#include "obj.h"

/* states for worker threads. */
#define WRKTHRD_STOPPED  FALSE
#define WRKTHRD_RUNNING  TRUE


/* possible states of a worker thread pool */
typedef enum {
	wtpState_RUNNING = 0,		/* runs in regular mode */
	wtpState_SHUTDOWN = 1,		/* worker threads shall shutdown when idle */
	wtpState_SHUTDOWN_IMMEDIATE = 2	/* worker threads shall shutdown ASAP, even if not idle */
} wtpState_t;


/* the worker thread pool (wtp) object */
struct wtp_s {
	BEGINobjInstance;
	wtpState_t wtpState;
	int 	iNumWorkerThreads;/* number of worker threads to use */
	int 	iCurNumWrkThrd;/* current number of active worker threads */
	struct wti_s **pWrkr;/* array with control structure for the worker thread(s) associated with this wtp */
	int	toWrkShutdown;	/* timeout for idle workers in ms, -1 means indefinite (0 is immediate) */
	rsRetVal (*pConsumer)(void *); /* user-supplied consumer function for dewtpd messages */
	/* synchronization variables */
	pthread_mutex_t mutWtp; /* mutex for the wtp's thread management */
	pthread_cond_t condThrdTrm;/* signalled when threads terminate */
	/* end sync variables */
	/* user objects */
	void *pUsr;		/* pointer to user object (in this case, the queue the wtp belongs to) */
	pthread_attr_t attrThrd;/* attribute for new threads (created just once and cached here) */
	pthread_mutex_t *pmutUsr;
	pthread_cond_t *pcondBusy; /* condition the user will signal "busy again, keep runing" on (awakes worker) */
	rsRetVal (*pfChkStopWrkr)(void *pUsr, int);
	rsRetVal (*pfGetDeqBatchSize)(void *pUsr, int*); /* obtains max dequeue count from queue config */
	rsRetVal (*pfObjProcessed)(void *pUsr, wti_t *pWti); /* indicate user object is processed */
	rsRetVal (*pfRateLimiter)(void *pUsr);
	rsRetVal (*pfIsIdle)(void *pUsr, wtp_t *pWtp);
	rsRetVal (*pfDoWork)(void *pUsr, void *pWti);
	rsRetVal (*pfOnIdle)(void *pUsr, int);
	rsRetVal (*pfOnWorkerCancel)(void *pUsr, void*pWti);
	rsRetVal (*pfOnWorkerStartup)(void *pUsr);
	rsRetVal (*pfOnWorkerShutdown)(void *pUsr);
	/* end user objects */
	uchar *pszDbgHdr;	/* header string for debug messages */
};

/* some symbolic constants for easier reference */


/* prototypes */
rsRetVal wtpConstruct(wtp_t **ppThis);
rsRetVal wtpConstructFinalize(wtp_t *pThis);
rsRetVal wtpDestruct(wtp_t **ppThis);
rsRetVal wtpAdviseMaxWorkers(wtp_t *pThis, int nMaxWrkr);
rsRetVal wtpProcessThrdChanges(wtp_t *pThis);
rsRetVal wtpChkStopWrkr(wtp_t *pThis, int bLockUsrMutex);
rsRetVal wtpSetState(wtp_t *pThis, wtpState_t iNewState);
rsRetVal wtpWakeupAllWrkr(wtp_t *pThis);
rsRetVal wtpCancelAll(wtp_t *pThis);
rsRetVal wtpSetDbgHdr(wtp_t *pThis, uchar *pszMsg, size_t lenMsg);
rsRetVal wtpShutdownAll(wtp_t *pThis, wtpState_t tShutdownCmd, struct timespec *ptTimeout);
PROTOTYPEObjClassInit(wtp);
PROTOTYPEpropSetMethFP(wtp, pfChkStopWrkr, rsRetVal(*pVal)(void*, int));
PROTOTYPEpropSetMethFP(wtp, pfRateLimiter, rsRetVal(*pVal)(void*));
PROTOTYPEpropSetMethFP(wtp, pfGetDeqBatchSize, rsRetVal(*pVal)(void*, int*));
PROTOTYPEpropSetMethFP(wtp, pfIsIdle, rsRetVal(*pVal)(void*, wtp_t*));
PROTOTYPEpropSetMethFP(wtp, pfDoWork, rsRetVal(*pVal)(void*, void*));
PROTOTYPEpropSetMethFP(wtp, pfObjProcessed, rsRetVal(*pVal)(void*, wti_t*));
PROTOTYPEpropSetMethFP(wtp, pfOnIdle, rsRetVal(*pVal)(void*, int));
PROTOTYPEpropSetMethFP(wtp, pfOnWorkerCancel, rsRetVal(*pVal)(void*,void*));
PROTOTYPEpropSetMethFP(wtp, pfOnWorkerStartup, rsRetVal(*pVal)(void*));
PROTOTYPEpropSetMethFP(wtp, pfOnWorkerShutdown, rsRetVal(*pVal)(void*));
PROTOTYPEpropSetMeth(wtp, toWrkShutdown, long);
PROTOTYPEpropSetMeth(wtp, wtpState, wtpState_t);
PROTOTYPEpropSetMeth(wtp, iMaxWorkerThreads, int);
PROTOTYPEpropSetMeth(wtp, pUsr, void*);
PROTOTYPEpropSetMeth(wtp, iNumWorkerThreads, int);
PROTOTYPEpropSetMethPTR(wtp, pmutUsr, pthread_mutex_t);
PROTOTYPEpropSetMethPTR(wtp, pcondBusy, pthread_cond_t);

#endif /* #ifndef WTP_H_INCLUDED */
