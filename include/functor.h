//
//  functor.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/2.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef functor_h
#define functor_h



#include <functional>
#include <utility>
#include <string>

namespace shadow
{
    
    //////////////////////////////////////////////////////////////////////////
    // memory utility
    
    /** delete operation
     */
    struct checked_delete
    {
        template < typename T >
        void operator()(T *p) const
        {
            try{
                delete p;
            }catch (...) {
            }
        }
    };
    
    /** delete array operation
     */
    struct checked_array_delete
    {
        template < typename T >
        void operator()(T *p) const
        {
            try{
                delete[] p;
            }catch (...) {
            }
        }
    };
    
    //////////////////////////////////////////////////////////////////////////
    // pointer compare
    
    /** pointer less compare
     */
    struct ptr_less
    {
        template < typename Ptr >
        bool operator()(const Ptr l, const Ptr r) const
        {
            return *l < *r;
        }
    };
    
    /** pointer greater compare
     */
    struct ptr_greater
    {
        template < typename Ptr >
        bool operator()(const Ptr l, const Ptr r) const
        {
            return *l > *r;
        }
    };
    
    /** pointer equal compare
     */
    struct ptr_equal
    {
        template < typename Ptr >
        bool operator()(const Ptr l, const Ptr r) const
        {
            return *l == *r;
        }
    };
    
    /** pointer less equal compare
     */
    struct ptr_less_equal
    {
        template < typename Ptr >
        bool operator()(const Ptr l, const Ptr r) const
        {
            return *l <= *r;
        }
    };
    
    /** pointer greater equal compare
     */
    struct ptr_greater_equal
    {
        template < typename Ptr >
        bool operator()(const Ptr l, const Ptr r) const
        {
            return *l >= *r;
        }
    };
    
    //////////////////////////////////////////////////////////////////////////
    // pointer & reference
    
    /** get reference from pointer
     */
    struct ptr2ref
    {
        template < typename T >
        T& operator()(T *p) const
        {
            return *p;
        }
        
        template < typename SmartPtrT >
        typename SmartPtrT::element_type& operator()(SmartPtrT& p) const
        {
            return *p;
        }
        
        template < typename SmartPtrT >
        const typename SmartPtrT::element_type& operator()(const SmartPtrT& p) const
        {
            return *p;
        }
    };
    
    /** get pointer from reference
     */
    struct ref2ptr
    {
        template < typename T >
        T* operator()(T& p) const
        {
            return &p;
        }
    };
    
    typedef ref2ptr	addressof;
    
    //////////////////////////////////////////////////////////////////////////
    // pair selector
    
    /** select std::pair's first field
     */
    struct select1st
    {
        template < typename F, typename S >
        F& operator()(std::pair<F, S>& p) const
        {
            return p.first;
        }
        
        template < typename F, typename S >
        const F& operator()(const std::pair<F, S>& p) const
        {
            return p.first;
        }
    };
    
    /** select std::pair's second field
     */
    struct select2nd
    {
        template < typename F, typename S >
        S& operator()(std::pair<F, S>& p) const
        {
            return p.second;
        }
        
        template < typename F, typename S >
        const S& operator()(const std::pair<F, S>& p) const
        {
            return p.second;
        }
    };
    
    struct less_1st
    {
        template < typename F, typename S1, typename S2 >
        bool operator()(const std::pair<F, S1>& p1, const std::pair<F, S2>& p2) const
        {
            return p1.first < p2.first;
        }
    };
    
    struct less_2nd
    {
        template < typename F1, typename F2, typename S >
        bool operator()(const std::pair<F1, S>& p1, const std::pair<F2, S>& p2) const
        {
            return p1.second < p2.second;
        }
    };
    
    struct greater_1st
    {
        template < typename F, typename S1, typename S2 >
        bool operator()(const std::pair<F, S1>& p1, const std::pair<F, S2>& p2) const
        {
            return p1.first > p2.first;
        }
    };
    
    struct greater_2nd
    {
        template < typename F1, typename F2, typename S >
        bool operator()(const std::pair<F1, S>& p1, const std::pair<F2, S>& p2) const
        {
            return p1.second > p2.second;
        }
    };
    
    struct equal_1st
    {
        template < typename F, typename S1, typename S2 >
        bool operator()(const std::pair<F, S1>& p1, const std::pair<F, S2>& p2) const
        {
            return p1.first == p2.first;
        }
    };
    
    struct equal_2nd
    {
        template < typename F1, typename F2, typename S >
        bool operator()(const std::pair<F1, S>& p1, const std::pair<F2, S>& p2) const
        {
            return p1.second == p2.second;
        }
    };
    
    struct less_equal_1st
    {
        template < typename F, typename S1, typename S2 >
        bool operator()(const std::pair<F, S1>& p1, const std::pair<F, S2>& p2) const
        {
            return p1.first <= p2.first;
        }
    };
    
    struct less_equal_2nd
    {
        template < typename F1, typename F2, typename S >
        bool operator()(const std::pair<F1, S>& p1, const std::pair<F2, S>& p2) const
        {
            return p1.second <= p2.second;
        }
    };
    
    struct greater_equal_1st
    {
        template < typename F, typename S1, typename S2 >
        bool operator()(const std::pair<F, S1>& p1, const std::pair<F, S2>& p2) const
        {
            return p1.first >= p2.first;
        }
    };
    
    struct greater_equal_2nd
    {
        template < typename F1, typename F2, typename S >
        bool operator()(const std::pair<F1, S>& p1, const std::pair<F2, S>& p2) const
        {
            return p1.second >= p2.second;
        }
    };
    
    //////////////////////////////////////////////////////////////////////////
    // hash
    
    template <class _Key> struct hash { };
    
    template<> struct hash<char> {
        size_t operator()(char __x) const { return static_cast<std::size_t>(__x); }
    };
    template<> struct hash<unsigned char> {
        size_t operator()(unsigned char __x) const { return static_cast<std::size_t>(__x); }
    };
    template<> struct hash<signed char> {
        size_t operator()(unsigned char __x) const { return static_cast<std::size_t>(__x); }
    };
    template<> struct hash<wchar_t> {
        size_t operator()(char __x) const { return static_cast<std::size_t>(__x); }
    };
    template<> struct hash<short> {
        size_t operator()(short __x) const { return static_cast<std::size_t>(__x); }
    };
    template<> struct hash<unsigned short> {
        size_t operator()(unsigned short __x) const { return static_cast<std::size_t>(__x); }
    };
    template<> struct hash<int> {
        size_t operator()(int __x) const { return static_cast<std::size_t>(__x); }
    };
    template<> struct hash<unsigned int> {
        size_t operator()(unsigned int __x) const { return static_cast<std::size_t>(__x); }
    };
    template<> struct hash<long> {
        size_t operator()(long __x) const { return static_cast<std::size_t>(__x); }
    };
    template<> struct hash<unsigned long> {
        size_t operator()(unsigned long __x) const { return static_cast<std::size_t>(__x); }
    };
    
    template <class T>
    inline void hash_combine(size_t& seed, T const& v)
    {
        hash<T> h;
        seed ^= h(v)
        + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }
    
    template <class It>
    inline size_t hash_range(It first, It last)
    {
        size_t seed = 0;
        for(; first != last; ++first)
            hash_combine(seed, *first);
        return seed;
    }
    
    inline size_t hash_string(const wchar_t * s)
    {
        size_t seed = 0;
        for(; *s != L'\0'; ++s)
            hash_combine(seed, *s);
        return seed;
    }
    
    inline size_t hash_string(const char * s)
    {
        size_t seed = 0;
        for(; *s != '\0'; ++s)
            hash_combine(seed, *s);
        return seed;
    }
    template<> struct hash<char*> {
        size_t operator()(const char* __s) const { return hash_string(__s); }
    };
    template<> struct hash<const char*> {
        size_t operator()(const char* __s) const { return hash_string(__s); }
    };
    template<> struct hash<wchar_t*> {
        size_t operator()(const wchar_t* __s) const { return hash_string(__s); }
    };
    template<> struct hash<const wchar_t*> {
        size_t operator()(const wchar_t* __s) const { return hash_string(__s); }
    };
    template<> struct hash<std::string> {
        size_t operator()(const std::string& __s) const { return hash_range(__s.begin(), __s.end()); }
    };
    template<> struct hash<std::wstring> {
        size_t operator()(const std::wstring& __s) const { return hash_range(__s.begin(), __s.end()); }
    };
    
    //////////////////////////////////////////////////////////////////////////
    // compose
    template <class _Operation1, class _Operation2>
    class unary_compose : public std::unary_function<typename _Operation2::argument_type,
    typename _Operation1::result_type>
    {
    protected:
        _Operation1 _M_fn1;
        _Operation2 _M_fn2;
    public:
        unary_compose(const _Operation1& __x, const _Operation2& __y)
        : _M_fn1(__x), _M_fn2(__y)
        {}
        
        typename _Operation1::result_type operator()(const typename _Operation2::argument_type& __x) const 
        {
            return _M_fn1(_M_fn2(__x));
        }
    };
    
    template <class _Operation1, class _Operation2>
    inline unary_compose<_Operation1,_Operation2> compose1(const _Operation1& __fn1, 
                                                           const _Operation2& __fn2)
    {
        return unary_compose<_Operation1,_Operation2>(__fn1, __fn2);
    }
    
    template <class _Operation1, class _Operation2, class _Operation3>
    class binary_compose : public std::unary_function<typename _Operation2::argument_type,
    typename _Operation1::result_type>
    {
    protected:
        _Operation1 _M_fn1;
        _Operation2 _M_fn2;
        _Operation3 _M_fn3;
    public:
        binary_compose(const _Operation1& __x, const _Operation2& __y, const _Operation3& __z)
        : _M_fn1(__x), _M_fn2(__y), _M_fn3(__z) 
        {}
        
        typename _Operation1::result_type operator()(const typename _Operation2::argument_type& __x) const
        {
            return _M_fn1(_M_fn2(__x), _M_fn3(__x));
        }
    };
    
    template <class _Operation1, class _Operation2, class _Operation3>
    inline binary_compose<_Operation1, _Operation2, _Operation3> compose2(const _Operation1& __fn1,
                                                                          const _Operation2& __fn2, const _Operation3& __fn3)
    {
        return binary_compose<_Operation1,_Operation2,_Operation3>(__fn1, __fn2, __fn3);
    }
}

#endif /* functor_h */
