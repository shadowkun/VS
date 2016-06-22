//
//  object_counter.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/7.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef object_counter_h
#define object_counter_h


#include <set>

#include "pthread_util.h"

namespace shadow
{
    template < typename T, typename sizeT = size_t, typename MutexT = NullLocker >
    class object_counter
    {
    public:
        typedef sizeT size_type;
    private:
        static size_type 	_count;
        static MutexT		_lock;
    protected:
        object_counter()
        {
            Locker<MutexT> l(_lock);
            ++_count;
        }
        object_counter(const object_counter&)
        {
            Locker<MutexT> l(_lock);
            ++_count;
        }
        ~object_counter()
        {
            Locker<MutexT> l(_lock);
            --_count;
        }
    public:
        static size_type get_object_count() { return _count; }
    };
    
    template < typename T, typename sizeT, typename MutexT >
    typename object_counter<T, sizeT, MutexT>::size_type object_counter<T, sizeT, MutexT>::_count = 0;
    
    template < typename T, typename sizeT, typename MutexT >
    MutexT object_counter<T, sizeT, MutexT>::_lock;
    
    template < typename T, typename sizeT = size_t, typename MutexT = NullLocker >
    class object_reporter
    {
        static std::set<object_reporter*>	_objs;
        static MutexT		_lock;
    protected:
        object_reporter()
        {
            Locker<MutexT> l(_lock);
            _objs.insert(this);
        }
        object_reporter(const object_reporter&)
        {
            Locker<MutexT> l(_lock);
            _objs.insert(this);
        }
        ~object_reporter()
        {
            Locker<MutexT> l(_lock);
            _objs.erase(this);
        }
    public:
        static size_t get_object_count() { return _objs.size(); }
        
        template < typename Pr >
        static Pr for_each_object(Pr pr)
        {
            Locker<MutexT> l(_lock);
            typename std::set<object_reporter*>::iterator b = _objs.begin(), e = _objs.end();
            for(; b != e; ++b){
                T * p = (T *) *b;
                pr(*p);
            }
            return pr;
        }
    };
    
    template < typename T, typename sizeT, typename MutexT >
    std::set<object_reporter<T, sizeT, MutexT>*> object_reporter<T, sizeT, MutexT>::_objs;
    
    template < typename T, typename sizeT, typename MutexT >
    MutexT object_reporter<T, sizeT, MutexT>::_lock;
    
}



#endif /* object_counter_h */
