//
//  runnable.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/2.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef runnable_h
#define runnable_h



#include "meta.h"



namespace shadow

{
    
    /** runnable interface
     
     */
    
    struct runnable
    
    {
        
        virtual void run() = 0;
        
        virtual ~runnable() {};
        
    };
    
    
    
    ///////////////////////////////////////////////////////////////////////////////
    
    // implements
    
    namespace runnable_detail
    
    {
        
        template < typename FUN >
        
        class fun_call : public runnable
        
        {
            
            FUN	m_fun;
            
        public:
            
            fun_call(FUN f) : m_fun(f) {}
            
            virtual void run() { m_fun(); }
            
        };
        
        
        
        template < typename FUN, typename PARA >
        
        class fun_call1 : public runnable
        
        {
            
            FUN		m_fun;
            
            const PARA	m_para;
            
        public:
            
            fun_call1(FUN f, PARA para) : m_fun(f), m_para(para) {}
            
            virtual void run() { m_fun(m_para); }
            
        };
        
        
        
        template < typename FUN, typename PARA1, typename PARA2 >
        
        class fun_call2 : public runnable
        
        {
            
            FUN		m_fun;
            
            const PARA1	m_para1;
            
            const PARA2	m_para2;
            
        public:
            
            fun_call2(FUN f, PARA1 para1, PARA2 para2)
            
            : m_fun(f), m_para1(para1), m_para2(para2)
            
            {}
            
            virtual void run() { m_fun(m_para1, m_para2); }
            
        };
        
        
        
        template < typename FUN, typename PARA1, typename PARA2, typename PARA3 >
        
        class fun_call3 : public runnable
        
        {
            
            FUN		m_fun;
            
            const PARA1	m_para1;
            
            const PARA2	m_para2;
            
            const PARA3	m_para3;
            
        public:
            
            fun_call3(FUN f, PARA1 para1, PARA2 para2, PARA3 para3)
            
            : m_fun(f), m_para1(para1), m_para2(para2), m_para3(para3)
            
            {}
            
            virtual void run() { m_fun(m_para1, m_para2, m_para3); }
            
        };
        
        
        
        template < typename FUN, typename PARA1, typename PARA2, typename PARA3, typename PARA4 >
        
        class fun_call4 : public runnable
        
        {
            
            FUN		m_fun;
            
            const PARA1	m_para1;
            
            const PARA2	m_para2;
            
            const PARA3	m_para3;
            
            const PARA4	m_para4;
            
        public:
            
            fun_call4(FUN f, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4)
            
            : m_fun(f), m_para1(para1), m_para2(para2), m_para3(para3), m_para4(para4)
            
            {}
            
            virtual void run() { m_fun(m_para1, m_para2, m_para3, m_para4); }
            
        };
        
        
        
        template < typename FUN, typename PARA1, typename PARA2, typename PARA3, typename PARA4, typename PARA5 >
        
        class fun_call5 : public runnable
        
        {
            
            FUN		m_fun;
            
            const PARA1	m_para1;
            
            const PARA2	m_para2;
            
            const PARA3	m_para3;
            
            const PARA4	m_para4;
            
            const PARA5	m_para5;
            
        public:
            
            fun_call5(FUN f, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, PARA5 para5)
            
            : m_fun(f), m_para1(para1), m_para2(para2), m_para3(para3), m_para4(para4), m_para5(para5)
            
            {}
            
            virtual void run() { m_fun(m_para1, m_para2, m_para3, m_para4, m_para5); }
            
        };
        
        
        
        template < typename FUN, typename PARA1, typename PARA2, typename PARA3, typename PARA4, typename PARA5, typename PARA6 >
        
        class fun_call6 : public runnable
        
        {
            
            FUN		m_fun;
            
            const PARA1	m_para1;
            
            const PARA2	m_para2;
            
            const PARA3	m_para3;
            
            const PARA4	m_para4;
            
            const PARA5	m_para5;
            
            const PARA6	m_para6;
            
        public:
            
            fun_call6(FUN f, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, PARA5 para5, PARA6 para6)
            
            : m_fun(f), m_para1(para1), m_para2(para2), m_para3(para3), m_para4(para4), m_para5(para5), m_para6(para6)
            
            {}
            
            virtual void run() { m_fun(m_para1, m_para2, m_para3, m_para4, m_para5, m_para6); }
            
        };
        
        
        
        template < class T, class T2, class RT >
        
        class memfun_call: public runnable
        
        {
            
        public:
            
            typedef T ObjectT;
            
            typedef typename type_selector<is_const_type<T*>::value,
            
            RT (T2::*)() const,
            
            RT (T2::*)()>::Type	ActionT;
            
            
            
            memfun_call(T& obj, ActionT mf)
            
            : m_obj(&obj), m_fun(mf)
            
            {}
            
            
            
            virtual void run() { (m_obj->*m_fun)(); }
            
        private:
            
            T*		m_obj;
            
            ActionT	m_fun;
            
        };
        
        
        
        template < class T, class T2, class RT, typename P, class PARA >
        
        class memfun_call1: public runnable
        
        {
            
        public:
            
            typedef T ObjectT;
            
            typedef PARA ParameterT;
            
            typedef typename type_selector<is_const_type<T*>::value,
            
            RT (T2::*)(P) const,
            
            RT (T2::*)(P)>::Type	ActionT;
            
            
            
            memfun_call1(T& obj, ActionT mf, PARA para)
            
            : m_obj(&obj), m_fun(mf), m_para(para)
            
            {}
            
            
            
            virtual void run() { (m_obj->*m_fun)(m_para); }
            
        private:
            
            T*		m_obj;
            
            ActionT	m_fun;
            
            const PARA	m_para;
            
        };
        
        
        
        template < class T, class T2, class RT, class P1, class P2, class PARA1, class PARA2 >
        
        class memfun_call2: public runnable
        
        {
            
        public:
            
            typedef T ObjectT;
            
            typedef PARA1 FirstParameterT;
            
            typedef PARA2 SecondParameterT;
            
            typedef typename type_selector<is_const_type<T*>::value,
            
            RT (T2::*)(P1, P2) const,
            
            RT (T2::*)(P1, P2)>::Type	ActionT;
            
            
            
            memfun_call2(T& obj, ActionT mf, PARA1 para1, PARA2 para2)
            
            : m_obj(&obj), m_fun(mf), m_para1(para1), m_para2(para2)
            
            {}
            
            
            
            virtual void run() { (m_obj->*m_fun)(m_para1, m_para2); }
            
        private:
            
            T*		m_obj;
            
            ActionT	m_fun;
            
            const PARA1	m_para1;
            
            const PARA2	m_para2;
            
        };
        
        
        
        template < class T, class T2, class RT, class P1, class P2, class P3, class PARA1, class PARA2, class PARA3 >
        
        class memfun_call3: public runnable
        
        {
            
        public:
            
            typedef T ObjectT;
            
            typedef PARA1 FirstParameterT;
            
            typedef PARA2 SecondParameterT;
            
            typedef PARA3 ThirdParameterT;
            
            typedef typename type_selector<is_const_type<T*>::value,
            
            RT (T2::*)(P1, P2, P3) const,
            
            RT (T2::*)(P1, P2, P3)>::Type	ActionT;
            
            
            
            memfun_call3(T& obj, ActionT mf, PARA1 para1, PARA2 para2, PARA3 para3)
            
            : m_obj(&obj), m_fun(mf), m_para1(para1), m_para2(para2), m_para3(para3)
            
            {}
            
            
            
            virtual void run() { (m_obj->*m_fun)(m_para1, m_para2, m_para3); }
            
        private:
            
            T*		m_obj;
            
            ActionT	m_fun;
            
            const PARA1	m_para1;
            
            const PARA2	m_para2;
            
            const PARA3	m_para3;
            
        };
        
        
        
        template < class T, class T2, class RT, class P1, class P2, class P3, class P4,
        
        class PARA1, class PARA2, class PARA3, class PARA4 >
        
        class memfun_call4: public runnable
        
        {
            
        public:
            
            typedef T ObjectT;
            
            typedef PARA1 FirstParameterT;
            
            typedef PARA2 SecondParameterT;
            
            typedef PARA3 ThirdParameterT;
            
            typedef PARA4 FourthParameterT;
            
            typedef typename type_selector<is_const_type<T*>::value,
            
            RT (T2::*)(P1, P2, P3, P4) const,
            
            RT (T2::*)(P1, P2, P3, P4)>::Type	ActionT;
            
            
            
            memfun_call4(T& obj, ActionT mf, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4)
            
            : m_obj(&obj), m_fun(mf), m_para1(para1), m_para2(para2), m_para3(para3), m_para4(para4)
            
            {}
            
            
            
            virtual void run() { (m_obj->*m_fun)(m_para1, m_para2, m_para3, m_para4); }
            
        private:
            
            T*		m_obj;
            
            ActionT	m_fun;
            
            const PARA1	m_para1;
            
            const PARA2	m_para2;
            
            const PARA3	m_para3;
            
            const PARA4	m_para4;
            
        };
        
        
        
        template < class T, class T2, class RT, class P1, class P2, class P3, class P4, class P5,
        
        class PARA1, class PARA2, class PARA3, class PARA4, class PARA5 >
        
        class memfun_call5: public runnable
        
        {
            
        public:
            
            typedef T ObjectT;
            
            typedef PARA1 FirstParameterT;
            
            typedef PARA2 SecondParameterT;
            
            typedef PARA3 ThirdParameterT;
            
            typedef PARA4 FourthParameterT;
            
            typedef PARA5 FifthParameterT;
            
            typedef typename type_selector<is_const_type<T*>::value,
            
            RT (T2::*)(P1, P2, P3, P4, P5) const,
            
            RT (T2::*)(P1, P2, P3, P4, P5)>::Type	ActionT;
            
            
            
            memfun_call5(T& obj, ActionT mf, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, PARA5 para5)
            
            : m_obj(&obj), m_fun(mf), m_para1(para1), m_para2(para2), m_para3(para3), m_para4(para4), m_para5(para5)
            
            {}
            
            
            
            virtual void run() { (m_obj->*m_fun)(m_para1, m_para2, m_para3, m_para4, m_para5); }
            
        private:
            
            T*		m_obj;
            
            ActionT	m_fun;
            
            const PARA1	m_para1;
            
            const PARA2	m_para2;
            
            const PARA3	m_para3;
            
            const PARA4	m_para4;
            
            const PARA5	m_para5;
            
        };
        
    }
    
    
    
    ///////////////////////////////////////////////////////////////////////////////
    
    // make_fun_runnable
    
    template < typename FUN >
    
    inline runnable * make_fun_runnable(FUN fun)
    
    {
        
        return new runnable_detail::fun_call<FUN>(fun);
        
    }
    
    
    
    template < typename FUN, typename PARA >
    
    inline runnable * make_fun_runnable(FUN fun, PARA para)
    
    {
        
        return new runnable_detail::fun_call1<FUN, PARA>(fun, para);
        
    }
    
    
    
    template < typename FUN, typename PARA1, typename PARA2 >
    
    inline runnable * make_fun_runnable(FUN fun, PARA1 para1, PARA2 para2)
    
    {
        
        return new runnable_detail::fun_call2<FUN, PARA1, PARA2>(fun, para1, para2);
        
    }
    
    
    
    template < typename FUN, typename PARA1, typename PARA2, typename PARA3 >
    
    inline runnable * make_fun_runnable(FUN fun, PARA1 para1, PARA2 para2, PARA3 para3)
    
    {
        
        return new runnable_detail::fun_call3<FUN, PARA1, PARA2, PARA3>(fun, para1, para2, para3);
        
    }
    
    
    
    template < typename FUN, typename PARA1, typename PARA2, typename PARA3, typename PARA4 >
    
    inline runnable * make_fun_runnable(FUN fun, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4)
    
    {
        
        return new runnable_detail::fun_call4<FUN, PARA1, PARA2, PARA3, PARA4>(fun, para1, para2, para3, para4);
        
    }
    
    
    
    template < typename FUN, typename PARA1, typename PARA2, typename PARA3, typename PARA4, typename PARA5 >
    
    inline runnable * make_fun_runnable(FUN fun, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, PARA5 para5)
    
    {
        
        return new runnable_detail::fun_call5<FUN, PARA1, PARA2, PARA3, PARA4, PARA5>(fun, para1, para2, para3, para4, para5);
        
    }
    
    
    
    template < typename FUN, typename PARA1, typename PARA2, typename PARA3, typename PARA4, typename PARA5, typename PARA6 >
    
    inline runnable * make_fun_runnable(FUN fun, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, PARA5 para5, PARA6 para6)
    
    {
        
        return new runnable_detail::fun_call6<FUN, PARA1, PARA2, PARA3, PARA4, PARA5, PARA6>(fun, para1, para2, para3, para4, para5, para6);
        
    }
    
    
    
    ///////////////////////////////////////////////////////////////////////////////
    
    // make_memfun_runnable
    
    template < typename T, typename T2, typename RT >
    
    inline runnable * make_memfun_runnable(T& obj, RT (T2::* mf)(), typename disable_if<is_const_type<T> >::type* dummy = 0)
    
    {
        
        typedef runnable_detail::memfun_call<T, T2, RT>	MFT;
        
        return new MFT(obj, mf);
        
    }
    
    
    
    template < typename T, typename T2, typename RT >
    
    inline runnable * make_memfun_runnable(const T& obj, RT (T2::* mf)() const)
    
    {
        
        typedef runnable_detail::memfun_call<const T, const T2, RT>	MFT;
        
        return new MFT(obj, mf);
        
    }
    
    
    
    template < typename T, typename T2, typename RT, typename P, typename PARA >
    
    inline runnable * make_memfun_runnable(T& obj, RT (T2::* mf)(P), PARA para, typename disable_if<is_const_type<T> >::type* dummy = 0)
    
    {
        
        typedef runnable_detail::memfun_call1<T, T2, RT, P, PARA> MFT;
        
        return new MFT(obj, mf, para);
        
    }
    
    
    
    template < typename T, typename T2, typename RT, typename P, typename PARA >
    
    inline runnable * make_memfun_runnable(const T& obj, RT (T2::* mf)(P) const, PARA para)
    
    {
        
        typedef runnable_detail::memfun_call1<const T, const T2, RT, P, PARA> MFT;
        
        return new MFT(obj, mf, para);
        
    }
    
    
    
    template < typename T, typename T2, typename RT, typename P1, typename P2, typename PARA1, typename PARA2 >
    
    inline runnable * make_memfun_runnable(T& obj, RT (T2::* mf)(P1, P2), PARA1 para1, PARA2 para2, typename disable_if<is_const_type<T> >::type* dummy = 0)
    
    {
        
        typedef runnable_detail::memfun_call2<T, T2, RT, P1, P2, PARA1, PARA2>	MFT;
        
        return new MFT(obj, mf, para1, para2);
        
    }
    
    
    
    template < typename T, typename T2, typename RT, typename P1, typename P2, typename PARA1, typename PARA2 >
    
    inline runnable * make_memfun_runnable(const T& obj, RT (T2::* mf)(P1, P2) const, PARA1 para1, PARA2 para2)
    
    {
        
        typedef runnable_detail::memfun_call2<const T, const T2, RT, P1, P2, PARA1, PARA2>	MFT;
        
        return new MFT(obj, mf, para1, para2);
        
    }
    
    
    
    template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3,
    
    typename PARA1, typename PARA2, typename PARA3 >
    
    inline runnable * make_memfun_runnable(T& obj, RT (T2::* mf)(P1, P2, P3), PARA1 para1, PARA2 para2, PARA3 para3, typename disable_if<is_const_type<T> >::type* dummy = 0)
    
    {
        
        typedef runnable_detail::memfun_call3<T, T2, RT, P1, P2, P3, PARA1, PARA2, PARA3>	MFT;
        
        return new MFT(obj, mf, para1, para2, para3);
        
    }
    
    
    
    template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3,
    
    typename PARA1, typename PARA2, typename PARA3 >
    
    inline runnable * make_memfun_runnable(const T& obj, RT (T2::* mf)(P1, P2, P3) const, PARA1 para1, PARA2 para2, PARA3 para3)
    
    {
        
        typedef runnable_detail::memfun_call3<const T, const T2, RT, P1, P2, P3, PARA1, PARA2, PARA3>		MFT;
        
        return new MFT(obj, mf, para1, para2, para3);
        
    }
    
    
    
    template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3, typename P4,
    
    typename PARA1, typename PARA2, typename PARA3, typename PARA4 >
    
    inline runnable * make_memfun_runnable(T& obj, RT (T2::* mf)(P1, P2, P3, P4), PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, typename disable_if<is_const_type<T> >::type* dummy = 0)
    
    {
        
        typedef runnable_detail::memfun_call4<T, T2, RT, P1, P2, P3, P4, PARA1, PARA2, PARA3, PARA4>		MFT;
        
        return new MFT(obj, mf, para1, para2, para3, para4);
        
    }
    
    
    
    template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3, typename P4,
    
    typename PARA1, typename PARA2, typename PARA3, typename PARA4 >
    
    inline runnable * make_memfun_runnable(const T& obj, RT (T2::* mf)(P1, P2, P3, P4) const, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4)
    
    {
        
        typedef runnable_detail::memfun_call4<const T, const T2, RT, P1, P2, P3, P4, PARA1, PARA2, PARA3, PARA4>		MFT;
        
        return new MFT(obj, mf, para1, para2, para3, para4);
        
    }
    
    
    
    template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5,
    
    typename PARA1, typename PARA2, typename PARA3, typename PARA4, typename PARA5 >
    
    inline runnable * make_memfun_runnable(T& obj, RT (T2::* mf)(P1, P2, P3, P4, P5), PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, PARA5 para5, typename disable_if<is_const_type<T> >::type* dummy = 0)
    
    {
        
        typedef runnable_detail::memfun_call5<T, T2, RT, P1, P2, P3, P4, P5, PARA1, PARA2, PARA3, PARA4, PARA5>		MFT;
        
        return new MFT(obj, mf, para1, para2, para3, para4, para5);
        
    }
    
    
    
    
    
    template < typename T, typename T2, typename RT, typename P1, typename P2, typename P3, typename P4, typename P5,
    
    typename PARA1, typename PARA2, typename PARA3, typename PARA4, typename PARA5 >
    
    inline runnable * make_memfun_runnable(const T& obj, RT (T2::* mf)(P1, P2, P3, P4, P5) const, PARA1 para1, PARA2 para2, PARA3 para3, PARA4 para4, PARA5 para5)
    
    {
        
        typedef runnable_detail::memfun_call5<const T, const T2, RT, P1, P2, P3, P4, P5, PARA1, PARA2, PARA3, PARA4, PARA5>		MFT;
        
        return new MFT(obj, mf, para1, para2, para3, para4, para5);
        
    }
    
    
    
}



#endif /* runnable_h */
