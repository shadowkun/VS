//
//  ref.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/2.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef ref_h
#define ref_h

#include "meta.h"

namespace shadow
{
    
    template<class T>
    class reference_wrapper
    {
    public:
        typedef T type;
        
        explicit reference_wrapper(T& t): _t(&t) {}
        
        operator T& () const { return *_t; }
        T& get() const { return *_t; }
        T* get_pointer() const { return _t; }
    private:
        T* _t;
    };
    
    template<class T>
    inline reference_wrapper<T> ref(T & t)
    {
        return reference_wrapper<T>(t);
    }
    
    template<class T>
    inline reference_wrapper<T const> cref(T const & t)
    {
        return reference_wrapper<T const>(t);
    }
    
    namespace ref_detail
    {
        using shadow::no_type;
        using shadow::yes_type;
        using shadow::type2type;
        
        no_type is_reference_wrapper_test(...);
        
        template < typename T >
        yes_type is_reference_wrapper_test(type2type< reference_wrapper<T> >);
    }
    
    template < typename T >
    struct is_reference_wrapper
    {
        enum { value = (sizeof(ref_detail::is_reference_wrapper_test(type2type<T>())) == sizeof(yes_type)) };
    };
    
} // namespace shadow


#endif /* ref_h */
