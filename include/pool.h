//
//  pool.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/7.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef pool_h
#define pool_h





#include <iosfwd>

#include "thread_pool.h"
#include "blocking_queue.h"



namespace shadow

{
    
    /** default pool factory
     
     */
    
    template < typename T >
    
    struct default_pool_factory
    
    {
        
        bool is_valid(T * e)	{ return true; }
        
        T * create()			{ return new T(); }
        
        void destory(T * e)		{ delete e; }
        
    };
    
    
    
    /** default pool resize policy
     
     */
    
    struct default_pool_policy
    
    {
        
        typedef size_t	size_type;
        
        
        
        /** get grow count
         
         *  \param pool_size	pool size
         
         *  \param min_size		min object size
         
         *  \param max_size		max object size
         
         *  \param free_count	free object count
         
         *  \return				if pool need grow, return a positive number;
         
         *  					if need to reduce, return a negative number;
         
         *  					otherwise, return zero;
         
         */
        
        int get_grow_count(size_type pool_size, size_type min_size, size_type max_size, size_type free_count);
        
    };
    
    
    
    /** object pool
     
     */
    
    template < typename T,
    
    typename Factory = default_pool_factory<T>,
    
    typename BlockingQueue = blocking_queue<T*>,
    
    typename Policy = default_pool_policy >
    
    class pool
    
    {
        
    public:
        
        typedef T									value_type;
        
        typedef Factory								factory_type;
        
        typedef BlockingQueue						queue_type;
        
        typedef Policy								policy_type;
        
        typedef typename BlockingQueue::size_type	size_type;
        
    private:
        
        BlockingQueue		_queue;			///< object queue
        
        Factory				_factory;		///< object factory
        
        Policy				_policy;		///< resize policy
        
        thread_pool			_tp;			///< thread pool to create/destory object
        
        volatile size_type	_pool_size;		///< pool's object count
        
        Mutex				_mutex_ps;		///< locker for _pool_size
        
        size_type			_min_size;		///< min count of object
        
        size_type			_max_size;		///< max count of object
        
    public:
        
        /** constructor
         
         *  \param pool_size	pool size
         
         */
        
        explicit pool(size_type pool_size);
        
        
        
        /** constructor
         
         *  \param min_size			max count of object
         
         *  \param max_size			min count of object
         
         *  \param thread_count		thread count of create/destory thread pool
         
         */
        
        pool(size_type min_size, size_type max_size, size_type thread_count = 1);
        
        
        
        /// create an object and push it to pool
        
        void create_one();
        
        
        
        /// get pool's object count
        
        size_type pool_size() const		{ return _pool_size; }
        
        
        
        /// get min object count
        
        size_type min_size() const		{ return _min_size; }
        
        
        
        /// get max object count
        
        size_type max_size() const		{ return _max_size; }
        
        
        
        /// get free object count
        
        size_type free_count() const	{ return _queue.size(); }
        
        
        
        /// have some object free?
        
        bool empty() const				{ return _queue.empty(); }
        
        
        
        /// pop an object
        
        T * pop();
        
        
        
        /// push an object
        
        void push(T * e);
        
    private:
        
        int clean_up();
        
    };
    
    
    
    template < typename T,
    
    typename Factory,
    
    typename BlockingQueue,
    
    typename Policy,
    
    typename charT,
    
    typename traits>
    
    std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& ost, const pool<T, Factory, BlockingQueue, Policy>& pool)
    
    {
        
        ost << "{ pool_size: " << pool.pool_size()
        
        << ", min_size: " << pool.min_size()
        
        << ", max_size: " << pool.max_size()
        
        << ", free_count: " << pool.free_count()
        
        << "}";
        
        return ost;
        
    }
    
    
    
    /** class to helper pop/push an object from a pool.
     
     */
    
    template < typename T,
    
    typename Factory = default_pool_factory<T>,
    
    typename BlockingQueue = blocking_queue<T*>,
    
    typename Policy = default_pool_policy >
    
    class pool_object
    
    {
        
        pool<T, Factory, BlockingQueue>& _pl;	///< pool
        
        T * _e;									///< object pop from pool
        
        
        
        pool_object(const pool_object&);
        
        pool_object& operator=(const pool_object&);
        
    public:
        
        pool_object(pool<T, Factory, BlockingQueue>& pl)
        
        : _pl(pl)
        
        , _e(_pl.pop())
        
        {}
        
        
        
        pool_object(pool<T, Factory, BlockingQueue>* pl)
        
        : _pl(*pl)
        
        , _e(_pl.pop())
        
        {}
        
        
        
        ~pool_object() { _pl.push(_e); }
        
        
        
        operator T*()	{ return _e; }
        
        T& operator*()	{ return *_e; }
        
        T* operator->()	{ return _e; }
        
        T * get()		{ return _e; }
        
    };
    
    
    
    //////////////////////////////////////////////////////////////////////////
    
    // implementations
    
    
    
    inline int default_pool_policy::get_grow_count(size_type pool_size, size_type min_size, size_type max_size, size_type free_count)
    
    {
        
        if(pool_size < max_size && free_count <= 1)
            
            return 1;
        
        if(pool_size > min_size && pool_size - free_count < min_size / 2)
            
            return - static_cast<int>(pool_size - min_size) / 2;
        
        return 0;
        
    }
    
    /////////////////////////////////////////////////////////////////////////
    
    // pool
    
    template < typename T,
    
    typename Factory,
    
    typename BlockingQueue,
    
    typename Policy >
    
    inline pool<T, Factory, BlockingQueue, Policy>::pool(size_type pool_size)
    
    : _pool_size(pool_size)
    
    , _min_size(pool_size)
    
    , _max_size(pool_size)
    
    {
        
        _tp.start(1);
        
        for(size_type i = 0; i < _pool_size; ++i)
            
            _tp.execute_memfun(*this, &pool::create_one);
        
    }
    
    
    
    template < typename T, 
    
    typename Factory, 
    
    typename BlockingQueue,
    
    typename Policy >
    
    inline pool<T, Factory, BlockingQueue, Policy>::pool(size_type min_size, size_type max_size, size_type thread_count)
    
    : _pool_size(min_size)
    
    , _min_size(min_size)
    
    , _max_size(max_size)
    
    {
        
        _tp.start(thread_count);
        
        for(size_type i = 0; i < _pool_size; ++i)
            
            _tp.execute_memfun(*this, &pool::create_one);
        
    }
    
    
    
    template < typename T, 
    
    typename Factory, 
    
    typename BlockingQueue,
    
    typename Policy >
    
    inline void pool<T, Factory, BlockingQueue, Policy>::create_one()
    
    {
        
        T * e = _factory.create();
        
        _queue.push(e);
        
    }
    
    
    
    template < typename T, 
    
    typename Factory, 
    
    typename BlockingQueue,
    
    typename Policy >
    
    inline T * pool<T, Factory, BlockingQueue, Policy>::pop()
    
    {
        
        clean_up();
        
        while(true) {
            
            T * e = _queue.pop();
            
            if(_factory.is_valid(e))
                
                return e;
            
            _tp.execute_memfun(*this, &pool::create_one);
            
            _tp.execute_memfun(_factory, &Factory::destory, e);
            
        }
        
    }
    
    
    
    template < typename T, 
    
    typename Factory, 
    
    typename BlockingQueue,
    
    typename Policy >
    
    inline void pool<T, Factory, BlockingQueue, Policy>::push(T * e)
    
    {
        
        if(_factory.is_valid(e)){
            
            _queue.push(e);
            
        }else{
            
            _tp.execute_memfun(*this, &pool::create_one);
            
            _tp.execute_memfun(_factory, &Factory::destory, e);
            
        }
        
    }
    
    
    
    template < typename T, 
    
    typename Factory, 
    
    typename BlockingQueue,
    
    typename Policy >
    
    inline int pool<T, Factory, BlockingQueue, Policy>::clean_up()
    
    {
        
        _mutex_ps.Lock();
        
        int gn = _policy.get_grow_count(pool_size(), min_size(), max_size(), _queue.size());
        
        _pool_size += gn;
        
        _mutex_ps.Unlock();
        
        if(gn == 0)
            
            return 0;
        
        if(gn > 0){
            
            for(int i = 0; i < gn; ++i)
                
                _tp.execute_memfun(*this, &pool::create_one);
            
        }else{
            
            for(int i = 0; i > gn; --i){
                
                T * e = _queue.pop();
                
                _tp.execute_memfun(_factory, &Factory::destory, e);
                
            }
            
        }
        
        return gn;
        
    }
    
    
    
}


#endif /* pool_h */
