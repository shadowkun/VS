//
//  blocking_queue.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/2.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef blocking_queue_h
#define blocking_queue_h

#include <queue>
#include <sys/time.h>


#include "pthread_util.h"

namespace shadow
{
    /** blocking queue
     */
    template <typename T, typename Sequence = std::deque<T> >
    class blocking_queue
    {
        typedef std::queue<T, Sequence> QueueT;
        QueueT                          m_queue;        ///< queue
        mutable Mutex           m_mutex;        ///< mutex to lock queue
        mutable Condition       m_cond;         ///< condition for mutex
    public:
        typedef T                       value_type;
        typedef T*                      pointer;
        typedef const T*        const_pointer;
        typedef T&                      reference;
        typedef const T&        const_reference;
        typedef typename QueueT::size_type              size_type;
        
        blocking_queue()
        {}
        
        template < typename FI >
        blocking_queue(FI first, FI last)
        {
            MutexLocker lock(m_mutex);
            for(; first != last; ++ first)
                m_queue.push(*first);
                }
        
        /** push a element to queue
         *  \param e            the element to push
         */
        void push(const T& e)
        {
            MutexLocker lock(m_mutex);
            //if(!m_queue.empty())
            //      m_cond.Signal();
            m_queue.push(e);
            m_cond.Signal();
        }
        
        /// pop a element from queue, wait when empty
        T pop()
        {
            MutexLocker lock(m_mutex);
            while(m_queue.empty())
                m_cond.Wait(m_mutex);
                T t = m_queue.front();
                m_queue.pop();
                return t;
        }
        
        /** pop a element with timeout
         *  \param t            element to pop
         *  \param timeout      timeout in millisecond
         *  \return                     if element is poped, return true; otherwise timeout, return false
         */
        bool pop(T& t, size_type timeout)
        {
            struct timespec to;
            struct timeval now;
            MutexLocker lock(m_mutex);
            gettimeofday(&now, NULL);
            to.tv_sec = now.tv_sec + timeout / 1000;
            to.tv_nsec = now.tv_usec * 1000 + (timeout % 1000) * 1000 * 1000;
            bool r = true;
            while(m_queue.empty() && r)
                r = m_cond.Wait(m_mutex, to);
                if(!r)
                    return false;
            t = m_queue.front();
            m_queue.pop();
            return true;
        }
        
        /// pop a element from queue if op(result)
        template < typename Op >
        bool pop_if(T& result, Op op)
        {
            MutexLocker lock(m_mutex);
            while(m_queue.empty())
                m_cond.Wait(m_mutex);
                result = m_queue.front();
                bool b = op(result);
                if(b)
                    m_queue.pop();
                    return b;
        }
        
        /// clear all
        void clear()
        {
            MutexLocker lock(m_mutex);
            while(!m_queue.empty())
                m_queue.pop();
                }
        
        /// is empty?
        bool empty() const
        {
            MutexLocker lock(m_mutex);
            return m_queue.empty();
        }
        
        /// get size of queue
        size_type size() const
        {
            MutexLocker lock(m_mutex);
            return m_queue.size();
        }
    };
    
    /** priority blocking queue
     */
    template <typename T, typename Sequence = std::vector<T>,
    typename Compare  = std::less<typename Sequence::value_type> >
    class priority_blocking_queue
    {
        typedef std::priority_queue<T, Sequence, Compare> QueueT;
        QueueT                          m_queue;        ///< queue
        mutable Mutex           m_mutex;        ///< mutex to lock queue
        mutable Condition       m_cond;         ///< condition for mutex
    public:
        typedef T                       value_type;
        typedef T*                      pointer;
        typedef const T*        const_pointer;
        typedef T&                      reference;
        typedef const T&        const_reference;
        typedef typename QueueT::size_type              size_type;
        
        priority_blocking_queue()
        {}
        
        template < typename FI >
        priority_blocking_queue(FI first, FI last)
        {
            MutexLocker lock(m_mutex);
            for(; first != last; ++ first)
                m_queue.push(*first);
                }
        
        /** push a element to queue
         *  \param e            the element to push
         */
        void push(const T& e)
        {
            MutexLocker lock(m_mutex);
            m_queue.push(e);
            m_cond.Signal();
        }
        
        /// pop a element from queue, wait when empty
        T pop()
        {
            MutexLocker lock(m_mutex);
            while(m_queue.empty())
                m_cond.Wait(m_mutex);
                T t = m_queue.top();
                m_queue.pop();
                return t;
        }
        
        /** pop a element with timeout
         *  \param t            element to pop
         *  \param timeout      timeout in millisecond
         *  \return                     if element is poped, return true; otherwise timeout, return false
         */
        bool pop(T& t, size_type timeout)
        {
            struct timespec to;
            struct timeval now;
            MutexLocker lock(m_mutex);
            gettimeofday(&now, NULL);
            to.tv_sec = now.tv_sec + timeout / 1000;
            to.tv_nsec = now.tv_usec * 1000 + (timeout % 1000) * 1000 * 1000;
            bool r = true;
            while(m_queue.empty() && r)
                r = m_cond.Wait(m_mutex, to);
                if(!r)
                    return false;
            t = m_queue.top();
            m_queue.pop();
            return true;
        }
        
        /// pop a element from queue if op(result)      
        template < typename Op >
        bool pop_if(T& result, Op op)
        {
            MutexLocker lock(m_mutex);
            while(m_queue.empty())
                m_cond.Wait(m_mutex);
                result = m_queue.top();
                bool b = op(result);
                if(b)
                    m_queue.pop();
                    return b;
        }
        
        /// clear all
        void clear()
        {
            MutexLocker lock(m_mutex);
            while(!m_queue.empty())
                m_queue.pop();
                }
        
        /// is empty?
        bool empty() const
        {
            MutexLocker lock(m_mutex);
            return m_queue.empty();
        }
        
        /// get size of queue
        size_type size() const
        {
            MutexLocker lock(m_mutex);
            return m_queue.size();
        }
    };
}



#endif /* blocking_queue_h */
