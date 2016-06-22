//
//  timer.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/6.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef timer_h
#define timer_h

#include <pthread.h>

#include "runnable.h"


namespace shadow
{
    
    /** timer
     */
    class timer
    {
        runnable *	_task;			///< time task
        pthread_t	_tid;			///< timer thread id
        size_t		_interval;		///< interval in milliseconds before task is to be executed
    public:
        timer();
        ~timer();
        
        /// cancel the timer
        void cancel();
        
        /** schedule
         *  \param task		the task to schedule
         *  \param interval	interval in milliseconds before task is to be executed
         *  \return			return false if task is null or interval is zero.
         */
        bool schedule(runnable * task, size_t interval);
        //void schedule(runnable * task, long interval, long period);
    private:
        static void * proc(void * pPara);
    };
    
}

#endif /* timer_h */
