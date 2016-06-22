//
//  cache.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/6.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef cache_h
#define cache_h

#include <map>
#include <time.h>

#include "meta.h"
#include "timer.h"
#include "runnable.h"
#include "pthread_util.h"

namespace shadow
{
    template < typename CacheT >
    class cache_iterator_impl
    {
        typedef typename CacheT::node_type	node_type;
        typedef typename CacheT::locker_type	locker_type;
        CacheT *	_cache;		///< cache
        node_type *	_node;		///< current node
    public:
        inline cache_iterator_impl()
        : _cache(NULL)
        , _node(NULL)
        {}
        
        inline explicit cache_iterator_impl(CacheT * c, node_type * node)
        : _cache(c)
        , _node(node)
        {
            node_type::add_ref(_node);
        }
        
        inline cache_iterator_impl(const cache_iterator_impl& v)
        : _cache(v._cache)
        , _node(v._node)
        {
            node_type::add_ref(_node);
        }
        
        inline const cache_iterator_impl& operator=(const cache_iterator_impl& v)
        {
            if(this != &v){
                node_type::remove_ref(_node);
                _cache = v._cache;
                _node = v._node;
                node_type::add_ref(_node);
            }
            return *this;
        }
        
        inline ~cache_iterator_impl()
        {
            node_type::remove_ref(_node);
        }
        
        inline locker_type& get_lock() const	{ return _cache->iterator_lock(); }
        
        inline void reset()
        {
            node_type::remove_ref(_node);
            _node = NULL;
        }
        
        inline node_type& operator*()
        {
            return *_node;
        }
        
        inline const node_type& operator*() const
        {
            return *_node;
        }
        
        inline node_type* operator->()
        {
            return _node;
        }
        
        inline const node_type* operator->() const
        {
            return _node;
        }
        
        inline node_type* get_node()
        {
            return _node;
        }
        
        inline const node_type* get_node() const
        {
            return _node;
        }
        
        inline bool operator==(const cache_iterator_impl& o) const
        {
            return _node == o._node;
        }
        
        inline bool operator!=(const cache_iterator_impl& o) const
        {
            return !(*this == o);
        }
        
        inline bool is_end() const
        {
            return _node == NULL;
        }
        
        inline cache_iterator_impl& operator++()
        {
            node_type * p = _node;
            _node = _node->_next;
            node_type::add_ref(_node);
            node_type::remove_ref(p);
            return *this;
        }
        
        inline cache_iterator_impl operator++(int)
        {
            cache_iterator_impl it(*this);
            ++*this;
            return it;
        }
        
        inline cache_iterator_impl& operator--()
        {
            node_type * p = _node;
            _node = _node->_prev;
            node_type::add_ref(_node);
            node_type::remove_ref(p);
            return *this;
        }
        
        inline cache_iterator_impl operator--(int)
        {
            cache_iterator_impl it(*this);
            --*this;
            return it;
        }
    };
    
    template < typename CacheT >
    class cache_iterator
    {
        typedef cache_iterator_impl<CacheT>	cache_iterator_impl;
        typedef typename CacheT::node_type	node_type;
        typedef typename CacheT::locker_type	locker_type;
        typedef Locker<locker_type>		LockLockerT;
        cache_iterator_impl	_impl;		///< the cache_iterator_impl point to current position
    public:
        inline cache_iterator(const cache_iterator_impl& impl)
        : _impl(impl)
        {}
        
        inline cache_iterator(const cache_iterator& v)
        {
            LockLockerT lock(v._impl.get_lock());
            _impl = v._impl;
        }
        
        const cache_iterator& operator=(const cache_iterator& v)
        {
            if(this != &v){
                typename CacheT::locker_type * c1 = &_impl.get_lock();
                typename CacheT::locker_type * c2 = &v._impl.get_lock();
                if(c1 > c2)
                {
                    LockLockerT lock2(*c2);
                    LockLockerT lock(*c1);
                    _impl = v._impl;
                }
                else if(c1 < c2)
                {
                    LockLockerT lock(*c1);
                    LockLockerT lock2(*c2);
                    _impl = v._impl;
                }
                else
                {
                    LockLockerT lock(*c1);
                    _impl = v._impl;
                }
            }
            return *this;
        }
        
        inline ~cache_iterator()
        {
            LockLockerT lock(_impl.get_lock());
            _impl.reset();
        }
        
        inline node_type& operator*()
        {
            return *_impl;
        }
        
        inline const node_type& operator*() const
        {
            return *_impl;
        }
        
        inline node_type* operator->()
        {
            return _impl.operator->();
        }
        
        inline const node_type* operator->() const
        {
            return _impl.operator->();
        }
        
        inline node_type* get_node()
        {
            return _impl.get_node();
        }
        
        inline const node_type* get_node() const
        {
            return _impl.get_node();
        }
        
        inline bool operator==(const cache_iterator& o) const
        {
            return _impl == o._impl;
        }
        
        inline bool operator!=(const cache_iterator& o) const
        {
            return !(*this == o);
        }
        
        inline bool is_end() const
        {
            return _impl.is_end();
        }
        
        // 隐式转换到 bool
        typedef bool (cache_iterator::*unspecified_bool_type)() const;
        inline operator unspecified_bool_type() const
        {
            return _impl.is_end() ? false : &cache_iterator::is_end;
        }
        
        inline cache_iterator& operator++()
        {
            LockLockerT lock(_impl.get_lock());
            ++_impl;
            return *this;
        }
        
        inline cache_iterator operator++(int)
        {
            LockLockerT lock(_impl.get_lock());
            cache_iterator it(_impl);
            ++*this;
            return it;
        }
        
        inline cache_iterator& operator--()
        {
            LockLockerT lock(_impl.get_lock());
            --_impl;
            return *this;
        }
        
        inline cache_iterator operator--(int)
        {
            LockLockerT lock(_impl.get_lock());
            cache_iterator it(_impl);
            --*this;
            return it;
        }
    };
    
    template < typename CacheT >
    class cache_reverse_iterator
    {
        typedef cache_iterator_impl<CacheT>	cache_iterator_impl;
        typedef typename CacheT::node_type	node_type;
        typedef typename CacheT::locker_type	locker_type;
        typedef Locker<locker_type>		LockLockerT;
        cache_iterator_impl	_impl;		///< the cache_iterator_impl point to current position
    public:
        inline cache_reverse_iterator(const cache_iterator_impl& impl)
        : _impl(impl)
        {}
        
        inline cache_reverse_iterator(const cache_reverse_iterator& v)
        {
            LockLockerT lock(v._impl.get_lock());
            _impl = v._impl;
        }
        
        const cache_reverse_iterator& operator=(const cache_reverse_iterator& v)
        {
            if(this != &v){
                typename CacheT::locker_type * c1 = &_impl.get_lock();
                typename CacheT::locker_type * c2 = &v._impl.get_lock();
                if(c1 > c2)
                {
                    LockLockerT lock2(*c2);
                    LockLockerT lock(*c1);
                    _impl = v._impl;
                }
                else if(c1 < c2)
                {
                    LockLockerT lock(*c1);
                    LockLockerT lock2(*c2);
                    _impl = v._impl;
                }
                else
                {
                    LockLockerT lock(*c1);
                    _impl = v._impl;
                }
            }
            return *this;
        }
        
        inline ~cache_reverse_iterator()
        {
            LockLockerT lock(_impl.get_lock());
            _impl.reset();
        }
        
        inline cache_iterator<CacheT> base()
        {
            LockLockerT lock(_impl.get_lock());
            return cache_iterator<CacheT>(_impl);
        }
        
        inline node_type& operator*()
        {
            return *_impl;
        }
        
        inline const node_type& operator*() const
        {
            return *_impl;
        }
        
        inline node_type* operator->()
        {
            return _impl.operator->();
        }
        
        inline const node_type* operator->() const
        {
            return _impl.operator->();
        }
        
        inline node_type* get_node()
        {
            return _impl.get_node();
        }
        
        inline const node_type* get_node() const
        {
            return _impl.get_node();
        }
        
        inline bool operator==(const cache_reverse_iterator& o) const
        {
            return _impl == o._impl;
        }
        
        inline bool operator!=(const cache_reverse_iterator& o) const
        {
            return !(*this == o);
        }
        
        inline bool is_end() const
        {
            return _impl.is_end();
        }
        
        // 隐式转换到 bool
        typedef bool (cache_reverse_iterator::*unspecified_bool_type)() const;
        inline operator unspecified_bool_type() const
        {
            return _impl._node == 0 ? 0 : &cache_reverse_iterator::is_end;
        }
        
        inline cache_reverse_iterator& operator++()
        {
            LockLockerT lock(_impl.get_lock());
            --_impl;
            return *this;
        }
        
        inline cache_reverse_iterator operator++(int)
        {
            LockLockerT lock(_impl.get_lock());
            cache_reverse_iterator it(_impl);
            --*this;
            return it;
        }
        
        inline cache_reverse_iterator& operator--()
        {
            LockLockerT lock(_impl.get_lock());
            ++_impl;
            return *this;
        }
        
        inline cache_reverse_iterator operator--(int)
        {
            LockLockerT lock(_impl.get_lock());
            cache_reverse_iterator it(_impl);
            ++*this;
            return it;
        }
    };
    
    /** cache node base.
     *  the cache node must be derive from this class, and this class can not be construct singly.
     *  this class provide some list operation and maintian a reference count of the node.
     */
    template < typename NodeT >
    class cache_node_base
    {
        template < typename T >
        friend class cache_node_list;
        
        template < typename T >
        friend class cache_iterator_impl;
    protected:
        NodeT *	_next;			///< next node
        NodeT *	_prev;			///< prev node
        int		_ref_count;	///< reference count of this node
        
        inline cache_node_base()
        : _next(0)
        , _prev(0)
        , _ref_count(0)
        {}
        
        inline ~cache_node_base(){}
        
        inline void set_next(NodeT * next)
        {
            remove_ref(_next);
            _next = next;
            add_ref(next);
        }
        
        inline void set_prev(NodeT * prev)
        {
            remove_ref(_prev);
            _prev = prev;
            add_ref(prev);
        }
        
        static inline void add_ref(NodeT * p)
        {
            if(p != NULL)
                ++p->_ref_count;
        }
        
        static inline void remove_ref(NodeT * p)
        {
            if(p != NULL && --p->_ref_count == 0){
                p->set_prev(NULL);
                p->set_next(NULL);
                delete p;
            }
        }
    };
    
    /** cache node list.
     *  provide some list operation for cache
     */
    template < typename NodeT >
    class cache_node_list
    {
    public:
        typedef NodeT	node_type;
    private:
        node_type *		_head;		///< head of list
        node_type *		_tail;		///< tail of list
    public:
        explicit cache_node_list()
        : _head(NULL)
        , _tail(NULL)
        {}
        
        inline node_type * get_head() const	{ return _head; }
        inline node_type * get_tail() const	{ return _tail; }
        
        inline void set_head(node_type * p)
        {
            node_type::remove_ref(_head);
            node_type::add_ref(p);
            _head = p;
        }
        
        inline void set_tail(node_type * p)
        {
            node_type::remove_ref(_tail);
            node_type::add_ref(p);
            _tail = p;
        }
        
        inline void clear()
        {
            while(_head != NULL){
                if(_head->_next != NULL){
                    _head->_next->set_prev(NULL);
                    set_head(_head->_next);
                }else{
                    set_head(NULL);
                    set_tail(NULL);
                    break;
                }
            }
        }
        
        inline void remove_from_list(node_type * p)
        {
            if(p->_next != NULL){
                p->_next->set_prev(p->_prev);
            }else{
                set_tail(p->_prev);
                if(_tail != NULL)
                    _tail->set_next(NULL);
            }
            if(p->_ref_count > 0){
                if(p->_prev != NULL){
                    p->_prev->set_next(p->_next);
                }else{
                    set_head(p->_next);
                    if(_head != NULL)
                        _head->set_prev(NULL);
                }
            }
        }
        
        inline void add_to_list(node_type * p)
        {
            if(_head == NULL){
                set_head(p);
                set_tail(p);
            }else{
                p->set_next(_head);
                _head->set_prev(p);
                set_head(p);
                _head->set_prev(NULL);
            }
        }
        
        inline void move_to_head(node_type * p)
        {
            if(_head != _tail) {
                remove_from_list(p);
                add_to_list(p);
            }
        }
    };
    
    /** default cache node
     */
    template < typename _Key, typename _Tp >
    class default_cache_node : protected cache_node_base< default_cache_node<_Key, _Tp> >
    {
        const _Key	_key;	///< key
        _Tp		_val;	///< value
        time_t	_create;	///< create time of the node
        time_t	_last_access;	///< last access of the node
    public:
        typedef _Key	key_type;
        typedef _Tp	value_type;
        
        inline explicit default_cache_node(const key_type& k, const value_type& v)
        : _key(k)
        , _val(v)
        , _create(::time(NULL))
        , _last_access(_create)
        {}
        inline const key_type& key() const	{ return _key; }
        inline const value_type& value() const	{ return _val; }
        inline value_type& value()	{ return _val; }
        inline void touch()	{ _last_access = ::time(NULL); }
        inline time_t create_time() const	{ return _create; }
        inline time_t last_access_time() const { return _last_access; }
    };
    
    /** cache node only save create time
     */
    template < typename _Key, typename _Tp >
    class cache_node_only_create_time : protected cache_node_base< cache_node_only_create_time<_Key, _Tp> >
    {
        const _Key	_key;	///< key
        _Tp		_val;	///< value
        time_t	_create;	///< create time of the node
    public:
        typedef _Key	key_type;
        typedef _Tp	value_type;
        
        inline explicit cache_node_only_create_time(const key_type& k, const value_type& v)
        : _key(k)
        , _val(v)
        , _create(::time(NULL))
        {}
        inline const key_type& key() const	{ return _key; }
        inline const value_type& value() const	{ return _val; }
        inline value_type& value()	{ return _val; }
        inline void touch()	{}
        inline time_t create_time() const	{ return _create; }
    };
    
    /** cache node not save create time or last access time
     */
    template < typename _Key, typename _Tp >
    class cache_node_non_save_time : protected cache_node_base< cache_node_non_save_time<_Key, _Tp> >
    {
        const _Key	_key;	///< key
        _Tp		_val;	///< value
    public:
        typedef _Key	key_type;
        typedef _Tp	value_type;
        
        inline explicit cache_node_non_save_time(const key_type& k, const value_type& v)
        : _key(k)
        , _val(v)
        {}
        inline const key_type& key() const	{ return _key; }
        inline const value_type& value() const	{ return _val; }
        inline value_type& value()	{ return _val; }
        inline void touch()	{}
    };
    
    struct sort_by_last_access_time
    {
        template < typename _Node >
        inline static void on_access(cache_node_list<_Node>& list, _Node * p)
        {
            list.move_to_head(p);
        }
    };
    
    struct sort_by_create_time
    {
        template < typename _Node >
        inline static void on_access(cache_node_list<_Node>& list, _Node * p)
        {}
    };
    
    /** cache
     */
    template < typename _Key,								// key type
    typename _Tp,										// value type
    typename _Locker = NullLocker, 					// locker type, if you want it thread safe, use Mutex
    typename _Policy = sort_by_last_access_time,		// sort policy, can be sort_by_last_access_time/sort_by_create_time
    typename _Node = default_cache_node<_Key, _Tp>,	// node type, can be default_cache_node/cache_node_only_create_time/cache_node_non_save_time, or you can provide it yourself, and it must be derive from cache_node_base
    typename _Map = std::map<_Key, _Node *>			// map type, can be map/hash_map, or you can provide it yourslef
    >
    class cache
    {
        typedef cache<_Key, _Tp, _Locker, _Policy, _Node, _Map>	this_type;
        friend class cache_iterator_impl<this_type>;
        typedef cache_iterator_impl<this_type>	cache_iterator_impl;
    public:
        typedef _Node	node_type;
        typedef _Key	key_type;
        typedef _Tp		value_type;
        typedef _Map	map_type;
        typedef _Locker	locker_type;
        typedef _Policy	policy_type;
        typedef typename map_type::size_type	size_type;
        typedef cache_iterator<this_type>	iterator;
        typedef cache_reverse_iterator<this_type>	reverse_iterator;
    private:
        typedef Locker<locker_type>			LockLockerT;
        typedef typename map_type::iterator	MIT;
        map_type			_map;		///< map
        cache_node_list<node_type>	_list;		///< list
        mutable locker_type		_lock;	///< lock
        size_type			_query_count;	///< query count
        size_type			_hit_count;	///< hit count
        
        /// get the locker to lock iteration
        inline locker_type& iterator_lock()	{ return _lock; }
        
        struct counter_op
        {
            size_type _count;
            inline counter_op()
            : _count(0)
            {}
            inline bool operator()(const node_type& n)
            {
                ++_count;
                return true;
            }
        };
    public:
        cache()
        : _query_count(0)
        , _hit_count(0)
        {}
        ~cache() { clear(); }
        
        /** get by key
         *  \param key	key
         *  \return		iterator point to the element associate the key
         */
        iterator get(const key_type& key)
        {
            node_type * p = NULL;
            LockLockerT l(_lock);
            ++_query_count;
            MIT i = _map.find(key);
            if(i != _map.end()){
                ++_hit_count;
                p = i->second;
                p->touch();
                cache_iterator_impl it(this, p);
                //_list.move_to_head(p);
                policy_type::on_access(_list, p);
                return it;
            }
            return cache_iterator_impl(this, p);
        }
        
        inline size_type get_query_count() const	{ return _query_count; }
        inline size_type get_hit_count() const		{ return _hit_count; }
        
        /// remove all element
        inline void clear()
        {
            LockLockerT l(_lock);
            _map.clear();
            _list.clear();
        }
        
        /** put a element to cache
         *  \param key		key
         *  \param val		value
         *  \return			old element
         */
        iterator put(const key_type& key, const value_type& val)
        {
            LockLockerT l(_lock);
            node_type*& n = _map[key];
            cache_iterator_impl vw(this, n);
            if(n != NULL)
                _list.remove_from_list(n);
            n = new node_type(key, val);
            _list.add_to_list(n);
            return vw;
        }
        
        /** put a node to cache
         *  \param node		the new node; this pointer will delete by cache
         *  \return			old element
         */
        iterator put(node_type * node)
        {
            LockLockerT l(_lock);
            node_type*& n = _map[node->key()];
            cache_iterator_impl vw(this, n);
            if(n != NULL)
                _list.remove_from_list(n);
            n = node;
            _list.add_to_list(n);
            return vw;
        }
        
        /** remove by key
         *  \param key	key
         *  \return		the element associate to key
         */
        iterator remove(const key_type& key)
        {
            node_type * p = NULL;
            LockLockerT l(_lock);
            MIT i = _map.find(key);
            if(i != _map.end()){
                p = i->second;
                _map.erase(i);
                cache_iterator_impl it(this, p);
                _list.remove_from_list(p);
                return it;
            }
            return cache_iterator_impl(this, NULL);
        }
        
        /** remove by iterator
         *  \param it	position of element to remove
         *  \return		if success, return true, otherwise return false
         */
        bool remove(iterator it)
        {
            LockLockerT l(_lock);
            MIT i = _map.find(it->key());
            if(i != _map.end()
               && i->second == it.get_node()){
                _map.erase(i);
                _list.remove_from_list(i->second);
                return true;
            }
            return false;
        }
        
        /** remove a range
         *  \param f	first
         *  \param l	last
         */
        void remove(iterator f, iterator l)
        {
            for(; f != l; ++f){
                LockLockerT l(_lock);
                MIT i = _map.find(f->key());
                if(i != _map.end()
                   && i->second == f.get_node()){
                    _map.erase(i);
                    _list.remove_from_list(i->second);
                }
            }
        }
        
        /** remove by reverse_iterator
         *  \it			position of element to remove
         *  \return		if success, return true, otherwise return false
         */
        bool remove(reverse_iterator it)
        {
            LockLockerT l(_lock);
            MIT i = _map.find(it->key());
            if(i != _map.end()
               && i->second == it.get_node()){
                _map.erase(i);
                _list.remove_from_list(i->second);
                return true;
            }
            return false;
        }
        
        /** remove a range
         *  \param f	first
         *  \param l	last
         */
        void remove(reverse_iterator f, reverse_iterator l)
        {
            for(; f != l; ++f){
                LockLockerT l(_lock);
                MIT i = _map.find(f->key());
                if(i != _map.end()
                   && i->second == f.get_node()){
                    _map.erase(i);
                    _list.remove_from_list(i->second);
                }
            }
        }
        
        /** remove the last n element
         *  \param n	remove count
         *  \param pr	the operator to call before remove; this operator must be return bool, if true, checker will remove the node, if false, checker will not remove node.
         *  \return		the pr
         */
        template < typename Pr >
        Pr remove_n(size_type n, Pr pr)
        {
            for(reverse_iterator i = rbegin(); n > 0; --n, ++i){
                if(i.is_end())
                    return pr;
                if(pr(*i))
                    remove(i);
            }
            return pr;
        }
        
        /** remove the last n element
         *  \param n	remove count
         *  \return		actual remove count
         */
        inline size_type remove_n(size_type n)
        {
            return remove_n(n, counter_op())._count;
        }
        
        /** remove all except the first n element
         *  \param n	reserve count
         *  \param pr	the operator to call before remove; this operator must be return bool, if true, checker will remove the node, if false, checker will not remove node.
         *  \return		the pr
         */
        template < typename Pr >
        Pr remove_except_n(size_type n, Pr pr)
        {
            for(reverse_iterator i = rbegin(); ; ++i){
                if(i.is_end() || size() <= n)
                    return pr;
                if(pr(*i))
                    remove(i);
            }
            return pr;
        }
        
        /** remove all except the first n element
         *  \param n	reserve count
         *  \return		actual remove count
         */
        inline size_type remove_except_n(size_type n)
        {
            return remove_except_n(n, counter_op())._count;
        }
        
        /// get size of cache
        inline size_type size() const
        {
            LockLockerT l(_lock);
            return _map.size(); 
        }
        
        ///< is empty
        inline bool empty() const
        {
            //LockLockerT l(_lock);
            return _list.get_head() == NULL;
        }
        
        /// @name iteration
        //@{
        inline iterator begin()
        {
            LockLockerT l(_lock);
            return cache_iterator_impl(this, _list.get_head());
        }
        
        inline iterator end()	{ return cache_iterator_impl(this, NULL); }
        
        inline reverse_iterator rbegin() 
        {
            LockLockerT l(_lock);
            return cache_iterator_impl(this, _list.get_tail());
        }
        
        inline reverse_iterator rend()	{ return cache_iterator_impl(this, NULL); }
        //@}
    };
    
    template < typename _Key,
    typename _Tp,		
    typename _Locker,
    typename _Policy,
    typename _Node,
    typename _Map
    >
    inline std::ostream& operator<<(std::ostream& ost, const cache<_Key, _Tp, _Locker, _Policy, _Node, _Map>& c)
    {
        ost << "{size: " << c.size() << ", query_count: " << c.get_query_count() << ", hit_count: " << c.get_hit_count() << "}";
        return ost;
    }
    
    struct checkable
    {
        virtual void check() = 0;
    };
    
    namespace cache_detail
    {
        struct nothing_op
        {
            template < typename T >
            bool operator()(const T& v)
            {
                return true;
            }
        };
        
        template < typename _Key,
        typename _Tp,		
        typename _Locker,
        typename _Policy,
        typename _Node,
        typename _Map,
        typename _Pr
        >
        class size_checker_impl : public checkable
        {
            typedef cache<_Key, _Tp, _Locker, _Policy, _Node, _Map>	cache_type;
            typedef typename cache_type::size_type size_type;
            cache_type&	_c;
            size_type	_max_size;
            size_type	_reserve_size;
            _Pr			_pr;
        public:
            size_checker_impl(cache_type& c, size_type max_size, size_type reserve_size, _Pr pr)
            : _c(c)
            , _max_size(max_size)
            , _reserve_size(reserve_size)
            , _pr(pr)
            {}
            
            void check()
            {
                if(_c.size() > _max_size)
                    _c.remove_except_n(_reserve_size, _pr);
            }
        };
        
        template < typename _Key,
        typename _Tp,		
        typename _Locker,
        typename _Policy,
        typename _Node,
        typename _Map,
        typename _Pr
        >
        class timeout_checker_impl : public checkable
        {
            typedef cache<_Key, _Tp, _Locker, _Policy, _Node, _Map>	cache_type;
            typedef typename cache_type::size_type size_type;
            cache_type&	_c;
            time_t	_timeout;
            _Pr			_pr;
            
            struct check_from_end
            {
                static void check(cache_type& c, time_t timeout, _Pr pr)
                {
                    typedef typename cache_type::reverse_iterator IT;
                    IT ib = c.rbegin(), ie = c.rend();
                    time_t now = time(NULL);
                    for(; ib != ie; ++ib){
                        if(now - ib->create_time() < timeout)
                            break;
                        if(pr(*ib))
                            c.remove(ib);
                    }
                }
            };
            
            struct check_all
            {
                static void check(cache_type& c, time_t timeout, _Pr pr)
                {
                    typedef typename cache_type::iterator IT;
                    IT ib = c.begin(), ie = c.end();
                    time_t now = time(NULL);
                    for(; ib != ie; ++ib){
                        if(now - ib->create_time() >= timeout){
                            if(pr(*ib))
                                c.remove(ib);
                        }
                    }
                }
            };
        public:
            timeout_checker_impl(cache_type& c, time_t timeout, _Pr pr)
            : _c(c)
            , _timeout(timeout)
            , _pr(pr)
            {}
            
            void check()
            {
                const bool sort_by_create = is_same_type<_Policy, sort_by_create_time>::value;
                typedef typename type_selector<sort_by_create, check_from_end, check_all>::Type checker_type;
                checker_type::check(_c, _timeout, _pr);
            }
        };
    }
    
    /** cache checker thread
     */
    class cache_checker_thread
    {
        class check_info
        {
            checkable *	c;
            time_t		interval;
            time_t		next_check_time;
        public:
            check_info(checkable *p, time_t check_interval)
            : c(p)
            , interval(check_interval)
            , next_check_time(time(NULL) + interval)
            {}
            
            void free() { delete c; }
            
            void check()
            {
                if(time(NULL) >= next_check_time){
                    c->check();
                    next_check_time = time(NULL) + interval;
                }
            }
        };
        typedef std::vector<check_info>	VecT;
        typedef VecT::size_type				size_type;
        typedef VecT::iterator				VIT;
        VecT		_cs;		///< the checkable
        time_t		_interval;	///< check interval
        timer		_timer;		///< timer
    public:
        explicit cache_checker_thread(time_t check_interval = 60)
        : _interval(check_interval)
        {}
        
        ~cache_checker_thread()
        {
            for(VIT i = _cs.begin(); i != _cs.end(); ++i)
                i->free();
        }
        
        /// start the checher thread, you should call it after all the add/add_size_checker/add_timeout_checker operation
        void start()
        {
            _timer.schedule(make_memfun_runnable(*this, &cache_checker_thread::check), _interval);
        }
        
        /** add a checker
         *  \param c				checkable interface
         *  \param check_interval	check interval
         */
        void add(checkable * c, time_t check_interval)
        {
            check_info ci(c, check_interval);
            _cs.push_back(ci);
        }
        
        /** add timeout checker
         *  \param c				the cache to check
         *  \param timeout			timeout
         *  \param check_interval	check interval
         *  \param pr				the operator to call while the node timeout; this operator must be return bool, if true, checker will remove the node, if false, checker will not remove node.
         */
        template < typename _Key,
        typename _Tp,		
        typename _Locker,
        typename _Policy,
        typename _Node,
        typename _Map,
        typename _Pr
        >
        void add_timeout_checker(cache<_Key, _Tp, _Locker, _Policy, _Node, _Map>& c, time_t timeout, time_t check_interval, _Pr pr)
        {
            typedef cache_detail::timeout_checker_impl<_Key, _Tp, _Locker, _Policy, _Node, _Map, _Pr>	impl;
            checkable * p = new impl(c, timeout, pr);
            add(p, check_interval);
        }
        
        /** add timeout checker
         *  \param c				the cache to check
         *  \param timeout			timeout
         *  \param check_interval	check interval
         */
        template < typename _Key,
        typename _Tp,		
        typename _Locker,
        typename _Policy,
        typename _Node,
        typename _Map
        >
        void add_timeout_checker(cache<_Key, _Tp, _Locker, _Policy, _Node, _Map>& c, time_t timeout, time_t check_interval)
        {
            add_timeout_checker(c, timeout, check_interval, cache_detail::nothing_op());
        }
        
        /** add size checker
         *  \param c				the cache to check
         *  \param max_size			when cache's size reach max_size, do cleanup
         *  \param reserve_size		when do cleanup, remove some node and reserve reserve_size
         *  \param check_interval	check interval
         *  \param pr				the operator to call while the node timeout; this operator must be return bool, if true, checker will remove the node, if false, checker will not remove node.
         */
        template < typename _Key,
        typename _Tp,		
        typename _Locker,
        typename _Policy,
        typename _Node,
        typename _Map,
        typename _Pr
        >
        void add_size_checker(cache<_Key, _Tp, _Locker, _Policy, _Node, _Map>& c, size_type max_size, size_type reserve_size, time_t check_interval, _Pr pr)
        {
            typedef cache_detail::size_checker_impl<_Key, _Tp, _Locker, _Policy, _Node, _Map, _Pr>	impl;
            checkable * p = new impl(c, max_size, reserve_size, pr);
            add(p, check_interval);
        }
        
        /** add size checker
         *  \param c				the cache to check
         *  \param max_size			when cache's size reach max_size, do cleanup
         *  \param reserve_size		when do cleanup, remove some node and reserve reserve_size
         *  \param check_interval	check interval
         */
        template < typename _Key,
        typename _Tp,		
        typename _Locker,
        typename _Policy,
        typename _Node,
        typename _Map
        >
        void add_size_checker(cache<_Key, _Tp, _Locker, _Policy, _Node, _Map>& c, size_type max_size, size_type reserve_size, time_t check_interval)
        {
            add_size_checker(c, max_size, reserve_size, check_interval, cache_detail::nothing_op());
        }
        
        /// check cache
        void check()
        {
            for(VIT i = _cs.begin(); i != _cs.end(); ++i)
                i->check();
        }
    };
    
}



#endif /* cache_h */
