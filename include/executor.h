//
//  executor.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/2.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef executor_h
#define executor_h

#include "runnable.h"

namespace shadow

{
    
    
    
    class executor
    
    {
        
    protected:
        
        executor() {}
        
    public:
        
        virtual ~executor() {}
        
        
        
        virtual void execute(runnable * r) = 0;
        
        
        
        template < typename FUN >
        
        void execute_fun(FUN fun)
        
        {
            
            execute(make_fun_runnable(fun));
            
        }
        
        
        
        template < typename FUN, typename PARA >
        
        void execute_fun(FUN fun, PARA para)
        
        {
            
            execute(make_fun_runnable(fun, para));
            
        }
        
        
        
        template < typename FUN, typename PARA1, typename PARA2 >
        
        void execute_fun(FUN fun, PARA1 para1, PARA2 para2)
        
        {
            
            execute(make_fun_runnable(fun, para1, para2));
            
        }
        
        
        
        template < typename FUN, typename PARA1, typename PARA2, typename PARA3 >
        
        void execute_fun(FUN fun, PARA1 para1, PARA2 para2, PARA3 para3)
        
        {
            
            execute(make_fun_runnable(fun, para1, para2, para3));
            
        }
        
        
        
        template < typename FUN, typename PARA1, typename PARA2, typename PARA3, typename PARA4 >
        
        void execute_fun(FUN fun, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4)
        
        {
            
            execute(make_fun_runnable(fun, para1, para2, para3, para4));
            
        }
        
        
        
        template < typename FUN, typename PARA1, typename PARA2, typename PARA3, typename PARA4, typename PARA5 >
        
        void execute_fun(FUN fun, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, PARA5 para5)
        
        {
            
            execute(make_fun_runnable(fun, para1, para2, para3, para4, para5));
            
        }
        
        
        
        template < typename FUN, typename PARA1, typename PARA2, typename PARA3, typename PARA4, typename PARA5, typename PARA6 >
        
        void execute_fun(FUN fun, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, PARA5 para5, PARA6 para6)
        
        {
            
            execute(make_fun_runnable(fun, para1, para2, para3, para4, para5, para6));
            
        }
        
        
        
        template < typename T, typename T2, typename RT >
        
        void execute_memfun(T& obj, RT (T2::* mf)(), typename disable_if<is_const_type<T> >::type* dummy = 0)
        
        {
            
            execute(make_memfun_runnable(obj, mf));
            
        }
        
        
        
        template < typename T, typename T2, typename RT >
        
        void execute_memfun(const T& obj, RT (T2::* mf)() const)
        
        {
            
            execute(make_memfun_runnable(obj, mf));
            
        }
        
        
        
        template < typename T, typename T2, typename RT, typename P, typename PARA >
        
        void execute_memfun(T& obj, RT (T2::* mf)(P), PARA para, typename disable_if<is_const_type<T> >::type* dummy = 0)
        
        {
            
            execute(make_memfun_runnable(obj, mf, para));
            
        }
        
        
        
        template < typename T, typename T2, typename RT, typename P, typename PARA >
        
        void execute_memfun(const T& obj, RT (T2::* mf)(P) const, PARA para)
        
        {
            
            execute(make_memfun_runnable(obj, mf, para));
            
        }
        
        
        
        template < typename T, typename T2, typename RT, typename P1, typename P2, typename PARA1, typename PARA2 >
        
        void execute_memfun(T& obj, RT (T2::* mf)(P1, P2), PARA1 para1, PARA2 para2, typename disable_if<is_const_type<T> >::type* dummy = 0)
        
        {
            
            execute(make_memfun_runnable(obj, mf, para1, para2));
            
        }
        
        
        
        template < typename T, typename T2, typename RT, typename P1, typename P2, typename PARA1, typename PARA2 >
        
        void execute_memfun(const T& obj, RT (T2::* mf)(P1, P2) const, PARA1 para1, PARA2 para2)
        
        {
            
            execute(make_memfun_runnable(obj, mf, para1, para2));
            
        }
        
        
        
        template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3,
        
        typename PARA1, typename PARA2, typename PARA3 >
        
        void execute_memfun(T& obj, RT (T2::* mf)(P1, P2, P3), PARA1 para1, PARA2 para2, PARA3 para3, typename disable_if<is_const_type<T> >::type* dummy = 0)
        
        {
            
            execute(make_memfun_runnable(obj, mf, para1, para2, para3));
            
        }
        
        
        
        template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3,
        
        typename PARA1, typename PARA2, typename PARA3 >
        
        void execute_memfun(const T& obj, RT (T2::* mf)(P1, P2, P3) const, PARA1 para1, PARA2 para2, PARA3 para3)
        
        {
            
            execute(make_memfun_runnable(obj, mf, para1, para2, para3));
            
        }
        
        
        
        template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3, typename P4,
        
        typename PARA1, typename PARA2, typename PARA3, typename PARA4 >
        
        void execute_memfun(T& obj, RT (T2::* mf)(P1, P2, P3, P4), PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, typename disable_if<is_const_type<T> >::type* dummy = 0)
        
        {
            
            execute(make_memfun_runnable(obj, mf, para1, para2, para3, para4));
            
        }
        
        
        
        template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3, typename P4,
        
        typename PARA1, typename PARA2, typename PARA3, typename PARA4 >
        
        void execute_memfun(const T& obj, RT (T2::* mf)(P1, P2, P3, P4) const, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4)
        
        {
            
            execute(make_memfun_runnable(obj, mf, para1, para2, para3, para4));
            
        }
        
        
        
        template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5,
        
        typename PARA1, typename PARA2, typename PARA3, typename PARA4, typename PARA5 >
        
        void execute_memfun(T& obj, RT (T2::* mf)(P1, P2, P3, P4, P5), PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, PARA5 para5, typename disable_if<is_const_type<T> >::type* dummy = 0)
        
        {
            
            execute(make_memfun_runnable(obj, mf, para1, para2, para3, para4, para5));
            
        }
        
        
        
        template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5,
        
        typename PARA1, typename PARA2, typename PARA3, typename PARA4, typename PARA5 >
        
        void execute_memfun(const T& obj, RT (T2::* mf)(P1, P2, P3, P4, P5) const, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, PARA5 para5)
        
        {
            
            execute(make_memfun_runnable(obj, mf, para1, para2, para3, para4, para5));
            
        }
        
    };
    
    
    
}




#endif /* executor_h */
