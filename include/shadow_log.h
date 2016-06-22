//
//  shadow_log.h
//  time2str
//
//  Created by 焱鲲甄 on 16/6/17.
//  Copyright © 2016年 焱鲲甄. All rights reserved.
//

#ifndef shadow_log_h
#define shadow_log_h



#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string>
#include <fstream>
#include <stdexcept>

namespace shadow {
    
    struct log_open_fail: public std::runtime_error{ log_open_fail(const std::string& s);};
    
    class CRollLog; class CDayLog;
 
    typedef CRollLog& (*__Roll_Func)(CRollLog&);
 
    typedef CDayLog& (*__Day_Func)(CDayLog&);
    
 
    class CRollLog
    {
    public:
        CRollLog();
        /**
         * no implementation
         */
        CRollLog(const CRollLog&);
        ~CRollLog();
        
    public:
 
        enum Log_Level{
            NO_LOG = 1,
            ERROR_LOG = 2,
            NORMAL_LOG = 3,
            DEBUG_LOG = 4
        };
 
        enum title_flag {
            F_Time = 0,
            F_Module = 1,
            F_PID = 2,
            F_DEBUGTIP = 3
        };
        
    public:
 
        void Init(const std::string& sPreFix,const std::string& module,size_t maxsize,size_t maxnum=10)
        throw(log_open_fail);
        
 
        void time_format(const std::string& format="[%Y-%m-%d %H:%M:%S]") {_time_format = format;}
        
 
        void set_module(const std::string& module){_module = module;}
        
 
        void set_level(Log_Level l){_setlevel = l;}
        
 
        void set_titleflag(title_flag f);
 
        void clear_titleflag(title_flag f) {_flags[f] = false;}
        
    public:
        CRollLog& operator<<(char n);
        CRollLog& operator<<(unsigned char n);
        
        CRollLog& operator<<(short n);
        CRollLog& operator<<(unsigned short n);
        
        CRollLog& operator<<(int n);
        CRollLog& operator<<(unsigned int n);
        
        CRollLog& operator<<(long n);
        CRollLog& operator<<(unsigned long n);
        
        CRollLog& operator<<(long long n);
        CRollLog& operator<<(unsigned long long n);
        
        CRollLog& operator<<(double n);
        CRollLog& operator<<(float n);
        
        CRollLog& operator<<(const std::string& s);
        CRollLog& operator<<(__Roll_Func func){ return (*func)(*this);}
        
    protected:
        bool check_level();
        std::string cur_time();
        
        friend CRollLog& endl(CRollLog& log);
        
        friend CRollLog& debug(CRollLog& log);
        friend CRollLog& error(CRollLog& log);
        friend CRollLog& normal(CRollLog& log);
        friend CRollLog& nolog(CRollLog& log);
        
    protected:
        static const unsigned FLUSH_COUNT = 32;
        
        //ofstream _os;
        int _fd;
        Log_Level _setlevel;
        Log_Level _level;
        unsigned _buf_count;
        
        unsigned _max_log_size;
        unsigned _max_log_num;
        
        std::string _module;
        std::string _filename;
        std::string _time_format;
        unsigned _pid;
        std::string _debugtip;
        
        bool _flags[4];
        bool _lock;
        std::string _log_buffer;
    };
    
 
    class CDayLog
    {
    public:
        CDayLog();
        /**
         * no implementation
         */
        CDayLog(const CDayLog&);
        ~CDayLog();
        
    public:
 
        enum title_flag {
            F_Time = 0,
            F_Module = 1,
            F_PID = 2
        };
        
    public:
        
 
        void Init(const std::string& sPreFix,const std::string& module) throw(log_open_fail);
        
 
        void time_format(const std::string& format="[%H:%M:%S]") {_time_format = format;}
        
 
        void set_titleflag(title_flag f);
        
 
        void clear_titleflag(title_flag f) {_flags[f] = false;}
        
    public:
        CDayLog& operator<<(char n);
        CDayLog& operator<<(unsigned char n);
        
        CDayLog& operator<<(short n);
        CDayLog& operator<<(unsigned short n);
        
        CDayLog& operator<<(int n);
        CDayLog& operator<<(unsigned int n);
        
        CDayLog& operator<<(long n);
        CDayLog& operator<<(unsigned long n);
        
        CDayLog& operator<<(long long n);
        CDayLog& operator<<(unsigned long long n);
        
        CDayLog& operator<<(double n);
        CDayLog& operator<<(float n);
        
        CDayLog& operator<<(const std::string& s);
        CDayLog& operator<<(__Day_Func func){ return (*func)(*this);}
        
    protected:
        std::string s_time(time_t t);
        std::string s_day(time_t t);
        time_t t_day(time_t t);
        bool is_same_day(time_t t1,time_t t2);
        bool Lock();
        void UnLock();//{if(_lock) {if(_os.is_open()) _os<<unlock; _lock = false;} }
        
        friend CDayLog& endl(CDayLog& log);
    protected:
        static const unsigned FLUSH_COUNT = 32;
        
        //	ofstream _os;
        int _fd;
        
        std::string _module;
        std::string _filename;
        std::string _time_format;
        unsigned _pid;
        
        bool _flags[3];
        bool _lock;
        
        time_t _last;
        std::string _log_buffer;
    };
    
 
    inline CRollLog& debug(CRollLog& log)
    {
        log._debugtip = "DEBUG:";
        log._level = CRollLog::DEBUG_LOG;
        return log;
    }
    
 
    inline CRollLog& normal(CRollLog& log)
    {
        log._debugtip = "";
        log._level = CRollLog::NORMAL_LOG;
        return log;
    }
    
 
    inline CRollLog& error(CRollLog& log)
    {
        log._debugtip = "ERROR:";
        log._level = CRollLog::ERROR_LOG;
        return log;
    }
    
 
    inline CRollLog& nolog(CRollLog& log)
    {
        log._debugtip = "";
        log._level = CRollLog::NO_LOG;
        return log;
    }
    
 
    inline CRollLog& endl(CRollLog& log)
    {
        if(!log.check_level()) return log;
        char tmpbuf[2]; tmpbuf[0] = '\n'; tmpbuf[1] = '\0';
        log._log_buffer += tmpbuf;
        write(log._fd,log._log_buffer.c_str(),log._log_buffer.length());
        log._lock = false;
        log._log_buffer = std::string();
        return log;
    }
    
 
    inline CDayLog& endl(CDayLog& log)
    {
        if(!log.Lock()) return log;
        char tmpbuf[2]; tmpbuf[0] = '\n'; tmpbuf[1] = '\0';
        log._log_buffer += tmpbuf;
        write(log._fd,log._log_buffer.c_str(),log._log_buffer.length());
        log._log_buffer = std::string();
        log.UnLock();
        return log;
    }
    
}



#endif /* shadow_log_h */
