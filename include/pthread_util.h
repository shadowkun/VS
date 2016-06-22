//
//  pthread_util.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/2.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef pthread_util_h
#define pthread_util_h

#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include <errno.h>


#include "functor.h"

namespace shadow
{
    
    class Mutex
    {
    private:
        pthread_mutex_t m_sect;
        
        Mutex(const Mutex&);
        Mutex& operator=(const Mutex&);
    public:
        Mutex()                 { ::pthread_mutex_init(&m_sect, NULL); }
        ~Mutex()                        { ::pthread_mutex_destroy(&m_sect); }
        bool Lock()                     { return 0 == ::pthread_mutex_lock(&m_sect); }
        bool Unlock()           { return 0 == ::pthread_mutex_unlock(&m_sect); }
        bool TryLock()          { return 0 == ::pthread_mutex_trylock(&m_sect); }
#ifdef __USE_XOPEN2K
        bool Lock(const struct timespec& abstime)       { return 0 == ::pthread_mutex_timedlock(&m_sect, &abstime); }
#endif
        /*
         extern int pthread_mutexattr_init (pthread_mutexattr_t *__attr) __THROW;
         extern int pthread_mutexattr_destroy (pthread_mutexattr_t *__attr) __THROW;
         extern int pthread_mutexattr_getpshared (__const pthread_mutexattr_t *
         __restrict __attr,
         int *__restrict __pshared) __THROW;
         extern int pthread_mutexattr_setpshared (pthread_mutexattr_t *__attr,
         int __pshared) __THROW;
         #ifdef __USE_UNIX98
         extern int pthread_mutexattr_settype (pthread_mutexattr_t *__attr, int __kind)
         __THROW;
         extern int pthread_mutexattr_gettype (__const pthread_mutexattr_t *__restrict
         __attr, int *__restrict __kind) __THROW;
         #endif
         */
        friend class Condition;
    };

    template < typename T >
    class Locker
    {
        T&      m_obj;
        
        Locker(const Locker&);
        Locker& operator=(const Locker&);
    public:
        Locker(T& obj)
        : m_obj(obj)
        {
            m_obj.Lock();
        }
        
        ~Locker()
        {
            m_obj.Unlock();
        }
    };
    
    typedef Locker<Mutex>   MutexLocker;
    
    /*
     class MutexLocker
     {
     Mutex& m_cs;
     
     MutexLocker(const MutexLocker&);
     MutexLocker& operator=(const MutexLocker&);
     
     public:
     MutexLocker(Mutex& cs)
     : m_cs(cs)
     {
     m_cs.Lock();
     }
     
     ~MutexLocker()
     {
     m_cs.Unlock();
     }
     };
     */
    class Condition
    {
        pthread_cond_t m_sect;
        
        Condition(const Condition&);
        Condition& operator=(const Condition&);
        
    public:
        Condition()                     { ::pthread_cond_init(&m_sect, NULL); }
        ~Condition()                    { ::pthread_cond_destroy(&m_sect); }
        bool Signal()                   { return 0 == ::pthread_cond_signal(&m_sect); }
        bool Broadcast()                { return 0 == ::pthread_cond_broadcast(&m_sect); }
        bool Wait(Mutex& m)     { return 0 == ::pthread_cond_wait(&m_sect, &m.m_sect); }
        bool Wait(Mutex& m, const struct timespec& abstime)
        {
            using namespace std;
            int r;
            do{
                r = ::pthread_cond_timedwait(&m_sect, &m.m_sect, &abstime);
            }while(r != 0 && r != ETIMEDOUT);
            return r == 0;
        }
        /*
         extern int pthread_condattr_init (pthread_condattr_t *__attr) __THROW;
         extern int pthread_condattr_destroy (pthread_condattr_t *__attr) __THROW;
         extern int pthread_condattr_getpshared (__const pthread_condattr_t *
         __restrict __attr,
         int *__restrict __pshared) __THROW;
         extern int pthread_condattr_setpshared (pthread_condattr_t *__attr,
         int __pshared) __THROW;
         */
    };
/*#if defined __USE_UNIX98 || defined __USE_XOPEN2K*/
    class ReadWriteLocker
    {
    private:
        pthread_rwlock_t m_sect;
        
        ReadWriteLocker(const ReadWriteLocker&);
        ReadWriteLocker& operator=(const ReadWriteLocker&);
    public:
        ReadWriteLocker()                       { ::pthread_rwlock_init(&m_sect, NULL); }
        ~ReadWriteLocker()                      { ::pthread_rwlock_destroy(&m_sect); }
        bool ReadLock()                         { return 0 == ::pthread_rwlock_rdlock(&m_sect); }
        bool WriteLock()                        { return 0 == ::pthread_rwlock_wrlock(&m_sect); }
        bool TryReadLock()                      { return 0 == ::pthread_rwlock_tryrdlock(&m_sect); }
        bool TryWriteLock()                     { return 0 == ::pthread_rwlock_trywrlock(&m_sect); }
        bool Unlock()                           { return 0 == ::pthread_rwlock_unlock(&m_sect); }
#ifdef __USE_XOPEN2K
        bool ReadLock(const struct timespec& abstime)   { return 0 == ::pthread_rwlock_timedrdlock(&m_sect, &abstime); }
        bool WriteLock(const struct timespec& abstime)  { return 0 == ::pthread_rwlock_timedwrlock(&m_sect, &abstime); }
#endif
        /*
         extern int pthread_rwlockattr_init (pthread_rwlockattr_t *__attr) __THROW;
         extern int pthread_rwlockattr_destroy (pthread_rwlockattr_t *__attr) __THROW;
         extern int pthread_rwlockattr_getpshared (__const pthread_rwlockattr_t *
         __restrict __attr,
         int *__restrict __pshared) __THROW;
         extern int pthread_rwlockattr_setpshared (pthread_rwlockattr_t *__attr,
         int __pshared) __THROW;
         extern int pthread_rwlockattr_getkind_np (__const pthread_rwlockattr_t *__attr,
         int *__pref) __THROW;
         extern int pthread_rwlockattr_setkind_np (pthread_rwlockattr_t *__attr,
         int __pref) __THROW;
         */
    };
    
    template < typename ReadWriteLockerT >
    class ReadWriteLockerRL
    {
        ReadWriteLockerT& m_cs;
        
        ReadWriteLockerRL(const ReadWriteLockerRL&);
        ReadWriteLockerRL& operator=(const ReadWriteLockerRL&);
    public:
        ReadWriteLockerRL(ReadWriteLockerT& cs)
        : m_cs(cs)
        {
            m_cs.ReadLock();
        }
        
        ~ReadWriteLockerRL()
        {
            m_cs.Unlock();
        }
    };
    
    template < typename ReadWriteLockerT >
    class ReadWriteLockerWL
    {
        ReadWriteLockerT& m_cs;
        
        ReadWriteLockerWL(const ReadWriteLockerWL&);
        ReadWriteLockerWL& operator=(const ReadWriteLockerWL&);
    public:
        ReadWriteLockerWL(ReadWriteLockerT& cs)
        : m_cs(cs)
        {
            m_cs.WriteLock();
        }
        
        ~ReadWriteLockerWL()
        {
            m_cs.Unlock();
        }
    };
    
    typedef ReadWriteLockerRL<ReadWriteLocker>      ReadLocker;
    typedef ReadWriteLockerWL<ReadWriteLocker>      WriteLocker;
    /*
     class ReadLocker
     {
     ReadWriteLocker& m_cs;
     
     ReadLocker(const ReadLocker&);
     ReadLocker& operator=(const ReadLocker&);
     
     public:
     ReadLocker(ReadWriteLocker& cs)
     : m_cs(cs)
     {
     m_cs.ReadLock();
     }
     
     ~ReadLocker()
     {
     m_cs.Unlock();
     }
     };
     
     class WriteLocker
     {
     ReadWriteLocker& m_cs;
     
     WriteLocker(const WriteLocker&);
     WriteLocker& operator=(const WriteLocker&);
     
     public:
     WriteLocker(ReadWriteLocker& cs)
     : m_cs(cs)
     {
     m_cs.WriteLock();
     }
     
     ~WriteLocker()
     {
     m_cs.Unlock();
     }
     };
     */
/*#endif*/
    
#ifdef __USE_XOPEN2K
    
    class Spin
    {
    private:
        pthread_spinlock_t m_sect;
        
        Spin(const Spin&);
        Spin& operator=(const Spin&);
    public:
        Spin(int pshared)                       { ::pthread_spin_init(&m_sect, pshared); }
        ~Spin()                                 { ::pthread_spin_destroy(&m_sect); }
        bool Lock()                                     { return 0 == ::pthread_spin_lock(&m_sect); }
        bool TryLock()                          { return 0 == ::pthread_spin_trylock(&m_sect); }
        bool Unlock()                           { return 0 == ::pthread_spin_unlock(&m_sect); }
    };
    
    typedef Locker<Spin>    SpinLocker;
    
    class Barrier
    {
    private:
        pthread_barrier_t m_sect;
        
        Barrier(const Barrier&);
        Barrier& operator=(const Barrier&);
    public:
        Barrier(unsigned int count)     { ::pthread_barrier_init(&m_sect, NULL, count); }
        ~Barrier()                                              { ::pthread_barrier_destroy(&m_sect); }
        int Wait()                                              { return ::pthread_barrier_wait(&m_sect); }
        /*
         extern int pthread_barrierattr_init (pthread_barrierattr_t *__attr) __THROW;
         
         extern int pthread_barrierattr_destroy (pthread_barrierattr_t *__attr) __THROW;
         
         extern int pthread_barrierattr_getpshared (__const pthread_barrierattr_t *
         __restrict __attr,
         int *__restrict __pshared) __THROW;
         
         extern int pthread_barrierattr_setpshared (pthread_barrierattr_t *__attr,
         int __pshared) __THROW;
         */
    };
    
#endif

    class Semaphore
    {
        sem_t   m_sem;
        
        Semaphore(const Semaphore&);
        Semaphore& operator=(const Semaphore&);
    public:
        explicit Semaphore(int pshared = 0, unsigned int value = 0)     { ::sem_init(&m_sem, pshared, value); }
        ~Semaphore()                                                            { ::sem_destroy(&m_sem); }
        
        bool Post()                                                                     { return 0 == ::sem_post(&m_sem); }
        void Wait()                                                                     { ::sem_wait(&m_sem); }
        bool TryWait()                                                          { return 0 == ::sem_trywait(&m_sem); }
        int GetValue()
        {
            int val = 0;
            ::sem_getvalue(&m_sem, &val);
            return val;
        }
    };
    
    //////////////////////////////////////////////////////////////////////////
    // null locker
    struct NullLocker
    {
        bool Lock()                     { return true; }
        bool Unlock()           { return true; }
        bool ReadLock()         { return true; }
        bool WriteLock()        { return true; }
    };
    
    //////////////////////////////////////////////////////////////////////////
    // lock pool
    
    template < typename T, typename LockT = Mutex, typename HashT = hash<T>, typename SequenceT = std::vector<LockT*> >
    class LockPool
    {
        HashT           _hash;
        SequenceT       _locks;
        bool            _own;
        typedef typename SequenceT::iterator SIT;
    public:
        typedef typename SequenceT::size_type size_type;
        
        explicit LockPool(const SequenceT& locks, bool own = false)
        : _locks(locks)
        , _own(own)
        {}
        
        explicit LockPool(size_type n)
        : _own(true)
        {
            _locks.resize(n);
            for(SIT i = _locks.begin(); i != _locks.end(); ++i)
                *i = new LockT();
        }
        
        ~LockPool()
        {
            if(_own){
                for(SIT i = _locks.begin(); i != _locks.end(); ++i)
                    delete *i;
            }
        }
        
        HashT Hasher() const    { return _hash; }
        
        void Lock(const T& v)
        {
            return LockH(_hash(v));
        }
        
        void Unlock(const T& v)
        {
            return UnlockH(_hash(v));
        }
        
        void LockH(size_type hash)
        {
            size_type h = hash % _locks.size();
            _locks[h]->Lock();
        }
        
        
        void UnlockH(size_type hash)
        {
            size_type h = hash % _locks.size();
            _locks[h]->Unlock();
        }
    };
    
    template < typename T, typename LockT = Mutex, typename HashT = hash<T>, typename SequenceT = std::vector<LockT*> >
    class LockPoolLocker
    {
        typedef typename SequenceT::size_type size_type;
    public:
        typedef LockPool<T, LockT, HashT, SequenceT>    LockPoolT;
        
        LockPoolLocker(LockPoolT& lp, const T& o)
        : _lp(lp)
        , _hash(lp.Hasher()(o))
        {
            _lp.LockH(_hash);
        }
        
        ~LockPoolLocker()
        {
            _lp.UnlockH(_hash);
        }
    private:
        LockPoolT&      _lp;
        size_type               _hash;
    };
    
}


#endif /* pthread_util_h */
