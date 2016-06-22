//
//  thread_pool.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/2.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef thread_pool_h
#define thread_pool_h

#include <vector>
#include <iosfwd>


#include "meta.h"
#include "executor.h"
#include "blocking_queue.h"

namespace shadow
{
    
    /** thread data
     */
    struct thread_data
    {
        virtual ~thread_data() {}
    };
    
    /** thread pool
     */
    class thread_pool : public executor
    {
        typedef blocking_queue<runnable *>              BlockingQueue;
        
        enum { eRunning, eWaiting, eStoped };
        struct thread_info
        {
            pthread_t               id;                     ///< thread id
            int                             status;         ///< thread status, one of eRunning, eWaiting, eStoped
            thread_data *   data;           ///< thread private data
            bool                    own_data;       ///< own the thread data?
            
            thread_info()
            : id(0), status(eRunning), data(0), own_data(false)
            {}
        };
        typedef std::vector<thread_info>::iterator      VIT;
        typedef std::vector<thread_info>::const_iterator        CVIT;
        
        BlockingQueue                           _queue;         ///< task queue
        std::vector<thread_info>        _tid;           ///< thread info
        volatile bool                           _stop_all;      ///< stop all thread flag
        runnable *                                      _init;          ///< thread initializer
        static pthread_key_t            _tkey;          ///< thread key
        
        thread_pool(const thread_pool&);
        thread_pool& operator=(const thread_pool&);
    public:
        typedef std::vector<thread_info>::size_type     size_type;
        
        /** constructor
         *  \param thread_count thread count
         *  \param initOp       initializer
         */
        template < typename Op >
        thread_pool(int thread_count, Op initOp)
        : _stop_all(false)
        , _init(NULL)
        {
            start(thread_count, initOp);
        }
        
        /** constructor
         *  \param thread_count thread count
         */
        explicit thread_pool(int thread_count);
        
        thread_pool();
        ~thread_pool();
        
        /** set thread's private data
         *  \param data         new thread data
         *  \param own          own the data
         *  \return                     the old thread data
         */
        static void set_thread_data(thread_data * data, bool own = false);
        
        /// get thread's private data
        static thread_data * get_thread_data();
        
        /// get thread's private data
        template < typename T >
        static T * get_thread_data()
        {
            return static_cast<T*>(get_thread_data());
        }
        
        /** start the thread pool
         *  \param thread_count thread count
         *  \param initOp       initializer
         *  \param dummy        useless parameter, you should ignore it; only use for overload function
         *  \return success?
         */
        template < typename Op >
        bool start(int thread_count, Op initOp, typename disable_if<is_convertible<Op, runnable*> >::type* dummy = 0)
        {
            return start_impl(thread_count, make_fun_runnable(initOp));
        }
        
        /** start the thread pool
         *  \param thread_count thread count
         *  \param initOp       initializer
         *  \return success?
         */
        bool start(int thread_count, runnable * initOp)
        {
            return start_impl(thread_count, initOp);
        }
        
        /** start the thread pool
         *  \param thread_count thread count
         *  \return success?
         */
        bool start(int thread_count)
        {
            return start_impl(thread_count);
        }
        
        /// get thread count of the thread pool
        size_type get_thread_count() const
        {
            return _tid.size();
        }
        
        /// get active thread count
        size_type get_active_thread_count() const
        {
            return count(eRunning);
        }
        
        /// get waiting queue's length
        size_type get_queue_length() const
        {
            return _queue.size();
        }
        
        /// execute a task
        void execute(runnable * r)
        {
            _queue.push(r);
        }
        
        /// stop all thread
        void stop();
        
        /// wait for all task run finish
        void wait();
    private:
        bool start_impl(int thread_count, runnable * init = NULL);
        size_type count(int status) const;
        void on_thread_init(thread_info * ti);
        static void* proc(void* pPara);
        static void create_thread_key();
        static void free_thread_key(void * data);
    };
    
    std::ostream& operator<<(std::ostream& ost, const thread_pool& tp);
    
    /** thread executor.
     *  this executor create a thread very time.
     */
    class thread_executor : public executor
    {
        thread_executor(const thread_executor&);
        thread_executor& operator=(const thread_executor&);
    public:
        thread_executor(){}
        void execute(runnable * r);
    };
    
    std::ostream& operator<<(std::ostream& ost, const thread_executor& te);
    
    /** synchronous executor
     */
    class sync_executor : public executor
    {
        sync_executor(const sync_executor&);
        sync_executor& operator=(const sync_executor&);
    public:
        sync_executor() {}
        void execute(runnable * r)
        {
            std::auto_ptr<runnable> rr(r);
            rr->run();
        }
        
        void wait(){}
    };
    
    std::ostream& operator<<(std::ostream& ost, const sync_executor& te);
    
}



#endif /* thread_pool_h */
